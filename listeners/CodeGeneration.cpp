#include "CodeGeneration.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <assert.h>

#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/FunctionSymbol.h"
#include "../symbolTable/VariableSymbol.h"
#include "../symbolTable/utils.h"

using namespace bluefin;
using namespace llvm;

using LLVMType = llvm::Type;

using std::dynamic_pointer_cast;

// TODO: I want to refactor the way VarDecl and StructDef's are handled, especially its ctor. This is getting messy.

CodeGeneration::CodeGeneration(SymbolTable& symTab, const map<ParseTree*, TypeContext>& typeCxts, const string moduleName) : 
    symbolTable{ symTab }, typeContexts{ typeCxts }	{
    // Open a new context and module.
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>(moduleName, *TheContext);

    // Create a new builder for the module.
    Builder = std::make_unique<IRBuilder<>>(*TheContext);
}

/* For global variable declarations, the expr() may not be a compile-time constant. We wouldn't know
* until after evaluating the node. If expr() doesn't exist simply create a global and set it to 0. 
* If it does exist, then create an internal function whose purpose is to update the global. 
* Now, if the expr() turns out to evaluate to a Constant (in exitVarDecl), then simply change the global's value from 0 
* to the constant's value and erase its internal function (since it's no longer needed).
* If the expr() is not a Constant, then push the internal function to a vector. Upon exiting the program,
* we will generate a global function inside llvm.global_ctors that calls all the internal VarDecl functions, which 
* ensures all global non-compile-time VarDecl are updated
*/
void CodeGeneration::enterVarDecl(bluefinParser::VarDeclContext* ctx)
{
    string id = ctx->ID()->getText();
    shared_ptr<Symbol> sym = symbolTable.getSymbol(ctx);
	Type varDeclType = symbolTable.getSymbol(ctx)->getType();
    LLVMType* llvmType = getLLVMType(varDeclType);
    shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);

    // This if stmt checks whether we're operating on a global VarDecl, local VarDecl, or struct def member
    if (varDeclScope->getEnclosingScope() == nullptr) {
        // global variable

        Constant* defaultVal = nullptr;

        if (varDeclType == Type::BOOL()) 
            defaultVal = Builder->getFalse();
        else if (varDeclType == Type::INT()) 
            defaultVal = Builder->getInt32(0);
        else if (varDeclType == Type::FLOAT()) 
            defaultVal = ConstantFP::get(*TheContext, APFloat(0.0f));
        else if (varDeclType.isUserDefinedType()) 
            defaultVal = nullptr;

        assert(llvmType != nullptr);

        GlobalVariable* v = new GlobalVariable(*TheModule, llvmType, false, 
            GlobalVariable::LinkageTypes::ExternalLinkage, defaultVal, id);

		resolvedSymAndValues.emplace(sym, v);

        if (ctx->expr() || varDeclType.isUserDefinedType()) { // if there's an expression, it either can be constant folded or requires calling 
            // other funcs or primaryIds, which would generate code. We don't know which right now, so we create
            // new internal function for it. The expr could also be a primaryInt/primaryFloat/etc, which we could find out here,
            // but for consistency, we choose to deal with that case same way as above.
            // If the variable is a structDef, also create the internal function and call the struct's ctor
            
            string funcName = "internalGlobalVarDecl_" + id;
            vector<LLVMType*> noParams;
			FunctionType* FT = FunctionType::get(LLVMType::getVoidTy(*TheContext), noParams, false);
			Function* internalF = Function::Create(FT, Function::InternalLinkage, funcName, TheModule.get());
            BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", internalF);
            Builder->SetInsertPoint(BB);

            if (varDeclType.isUserDefinedType() && !ctx->expr()) { // simply a bare global var. eg) Normal norm;
                shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(varDeclType));
                Function* ctor = structHelper.getCtor(structSym);
                Builder->CreateCall(ctor, v);
				Builder->CreateRetVoid(); 
				internalFunctionsForVarDeclExpr.push_back(internalF);
            }
        }
    }
    else if (dynamic_pointer_cast<StructSymbol>(varDeclScope) == nullptr) { // local varDecl 
        // local variables are allocated on the stack
        Value* allocaVal = Builder->CreateAlloca(llvmType, nullptr, "alloctmp_" + id); // NOTE, we're storing the AllocaInst here
        values.emplace(ctx, allocaVal);
		resolvedSymAndValues.emplace(sym, allocaVal); 

        // if the type is a struct, then call the struct's its corresponding ctor
        if (varDeclType.isUserDefinedType()) {
            
            shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(varDeclType));
            Function* ctor = structHelper.getCtor(structSym);
            Builder->CreateCall(ctor, allocaVal);
        }
    }
    else {
        // struct member field
        // if the type is a user-defined type. eg) Foo foo, then associate it into resolvedSymAnd
    }
}

void CodeGeneration::exitVarDecl(bluefinParser::VarDeclContext* ctx)
{
    string id = ctx->ID()->getText();
    shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);

    // if the variable declaration is for a global and it contains an expr, then we will need
    // to update the global's value (since it was originally set to 0)
    if (varDeclScope->getEnclosingScope() == nullptr && ctx->expr()) {
        Value* val = values.at(ctx->expr());
		Function* internalFunction = Builder->GetInsertBlock()->getParent();
        if (isa<Constant>(val)) {
            Constant* c = dyn_cast<Constant>(val);
            TheModule->getNamedGlobal(id)->setInitializer(c);
            // Remove the internal function since the expr() is a compile-time constant and we can update vardecl 
            internalFunction->eraseFromParent();

        }
        else { // non-compile-time assignment. Eg) int d = foo();
            // At this point, we finished evaluating the expr and our insertion point is still inside the internal function
            // So store the expr's evaluated value to the global var, and then close it off with a ret void
			Builder->CreateStore(val, TheModule->getNamedGlobal(id));
			Builder->CreateRetVoid(); 

            internalFunctionsForVarDeclExpr.push_back(internalFunction);
        }
    }
    else if (dynamic_pointer_cast<StructSymbol>(varDeclScope) == nullptr && ctx->expr()) {
        // local var with an expr. Store the value of the expr into the local var
        Value* exprVal = values.at(ctx->expr());
        Value* allocaInst = values.at(ctx);

        Builder->CreateStore(exprVal, allocaInst);
    }
    else if (isStructField(ctx, symbolTable) /* && ctx->expr() */ ) {
        // struct member field with expr. 
        // if we're in here, that means the current BB insertion point should be in the ctor.

        // if user-defined type, then call ctor
		Type varDeclType = symbolTable.getSymbol(ctx)->getType();
        if (varDeclType.isUserDefinedType()) {
            string str = dump();
            shared_ptr<StructSymbol> structOfVarDeclType = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(varDeclType));
            Function* ctor = structHelper.getCtor(structOfVarDeclType);

            // We also need to pass in the address of the field. 
            shared_ptr<StructSymbol> structSym = getContainingStruct(ctx, symbolTable);
            LLVMType* structType = getLLVMType(structSym->getType());

            Value* thisPtr = structHelper.getCtorThisPtr(structSym);

            Value* zero = Builder->getInt32(0);
            size_t memberIndex = structSym->getFieldIndex(id);
            Value* index = Builder->getInt32(memberIndex);
            Value* indices[2] = { zero, index };

            Value* memberPtr = Builder->CreateGEP(structType, thisPtr, indices, "memPtr_" + id);

            Builder->CreateCall(ctor, memberPtr);
        }
        if (ctx->expr()) {
            assert(!varDeclType.isUserDefinedType()); // for now, we don't allow struct assignment inside vardecl. eg) Foo foo = foobar;
            shared_ptr<StructSymbol> structSym = getContainingStruct(ctx, symbolTable);
            LLVMType* structType = getLLVMType(structSym->getType());

            Value* thisPtr = structHelper.getCtorThisPtr(structSym);

            Value* zero = Builder->getInt32(0);
            size_t memberIndex = structSym->getFieldIndex(id);
            Value* index = Builder->getInt32(memberIndex);
            Value* indices[2] = { zero, index };

            Value* memberPtr = Builder->CreateGEP(structType, thisPtr, indices, "memPtr_" + id);

            Value* exprVal = values.at(ctx->expr());
            Builder->CreateStore(exprVal, memberPtr);
        }
    }
}

void CodeGeneration::enterFuncDef(bluefinParser::FuncDefContext* ctx) {

    bool isMethod = isStructMethod(ctx, symbolTable);
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getSymbol(ctx));
    assert(funcSym != nullptr);

    // If the funcDef is a method, then its declaration must have already been generated by enterStructDef
    // so no need to generate signature, simply fill in the alloca for non-this args
    if (isMethod) {
        Function* F = methodToLLVMFunctions.at(funcSym);
        BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", F);
        Builder->SetInsertPoint(BB);

		vector<shared_ptr<Symbol>> params = funcSym->getParams();
        for (int i = 1; i < F->arg_size(); i++) { // start at 1 not 0, because no need to allocate for "this" pointer
            Argument* arg = F->getArg(i);
            AllocaInst* allocaInst = Builder->CreateAlloca(arg->getType(), nullptr, "alloctmp_" + arg->getName());
            Builder->CreateStore(arg, allocaInst);
            resolvedSymAndValues.emplace(params[i-1], allocaInst);
        }
        currentMethodThis = F->getArg(0);
    }
    else {
        Type retType = funcSym->getType();
        assert(retType.isUserDefinedType() == false && retType != Type::STRING()); // for now, only allow some types to be generated

        vector<LLVMType*> llvmParamTypes;
        vector<shared_ptr<Symbol>> params = funcSym->getParams();

        for (const auto param : params) {
            Type paramType = param->getType();
            assert(paramType.isUserDefinedType() == false && paramType != Type::STRING() && paramType != Type::VOID()); // for now, only allow some types to be generated
            llvmParamTypes.push_back(getLLVMType(paramType));
        }

        FunctionType* FT = FunctionType::get(getLLVMType(retType), llvmParamTypes, false);
        Function* F = Function::Create(FT, Function::ExternalLinkage, funcSym->getName(), TheModule.get());

        assert(params.size() == llvmParamTypes.size());

        BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", F);
        Builder->SetInsertPoint(BB);

        // since the args can be mutated, create space on the stack that will contain their (possibly mutated) value
        // Future references to the args will refer to the allocated space
        // Add the values to resolvedSymsAndValues, and set the param names for LLVM's code generation (to avoid awkward names like %0, %1)
        for (int i = 0; i < params.size(); i++) {
            AllocaInst* allocaInst = Builder->CreateAlloca(llvmParamTypes[i], nullptr, "alloctmp_" + params[i]->getName());
            Builder->CreateStore(F->getArg(i), allocaInst);
            F->getArg(i)->setName(params[i]->getName());
            resolvedSymAndValues.emplace(params[i], allocaInst);
        }
    }
}

void CodeGeneration::exitFuncDef(bluefinParser::FuncDefContext* ctx)
{
    // Since all basic blocks must end with either a branching instruction or a return, 
    // even for void functions, if this function has return type of void, then
    // generate a ret void instruction. If the user already had a ret instruction,
    // then it would have already generated a new basic block, into which this ret statement might be in,
    // so that no basic block would contain two terminators

    shared_ptr<Symbol> sym = symbolTable.getSymbol(ctx);
    if (sym->getType() == Type::VOID()) {
        Builder->CreateRetVoid();
    }

    vector<BasicBlock*> impossibleBB;
	Function* TheFunction = Builder->GetInsertBlock()->getParent();
    for (auto& block : TheFunction->getBasicBlockList()) {
        if (block.getName().startswith("impossible")) {
            impossibleBB.push_back(&block);
        }
    }

	for (BasicBlock* b : impossibleBB) 
		b->eraseFromParent();

    if (isStructMethod(ctx, symbolTable)) {
        shared_ptr<StructSymbol> structSym = getContainingStruct(ctx, symbolTable);
        currentMethodThis = structHelper.getCtorThisPtr(structSym); // reset the "this" to refer back to the ctor
        Builder->SetInsertPoint(&(structHelper.getCtor(structSym))->getEntryBlock());
    }
}

/* For all struct def, we create the struct here and then start constructing its ctor, 
All fields/aka, VarDecl with expr() will generate code inside the ctor to update their values
In exitStructDef, we close off the ctor.
*/
void CodeGeneration::enterStructDef(bluefinParser::StructDefContext* ctx)
{
    // Since some some the structDef's varDecl's contain assignment, we create a ctor for every struct def
    // so that those assignment expressions can go into the ctor function
    shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbol(ctx));

    vector<LLVMType*> members;
    for (auto varSym : structSym->getFields()) {
		members.push_back(getLLVMType(varSym->getType()));
    }

    StructType* structType = StructType::create(*TheContext, members, structSym->getName());
    bluefinToLLVMTypes.emplace(structSym->getType(), structType);   // store this StructType as the type is needed if 

    Function* F = createCtor(structSym);
    structHelper.addStructDef(structSym, F);
    currentMethodThis = F->getArg(0); // referring to ctor

    for (auto funcSym : structSym->getMethods()) {
        // Create the Function/method here so that later, internal VarDecl (fields) can forward refernce them

		Type retType = funcSym->getType();
		assert(retType.isUserDefinedType() == false && retType != Type::STRING()); // for now, only allow some types to be generated

        vector<LLVMType*> llvmParamTypes;
        vector<shared_ptr<Symbol>> params = funcSym->getParams();

        llvmParamTypes.push_back(structType->getPointerTo());
        for (const auto param : params) {
            Type paramType = param->getType();
            assert(paramType.isUserDefinedType() == false && paramType != Type::STRING() && paramType != Type::VOID()); // for now, only allow some types to be generated
            llvmParamTypes.push_back(getLLVMType(paramType));
        }

		FunctionType* FT = FunctionType::get(getLLVMType(retType), llvmParamTypes, false);
		Function* F = Function::Create(FT, Function::ExternalLinkage, "_" + structSym->getName() + "_" + funcSym->getName(), TheModule.get());
        F->getArg(0)->setName("this");
        for (int i = 0; i < params.size(); i++) {
            F->getArg(i + 1)->setName(params[i]->getName());
        }

        methodToLLVMFunctions.emplace(funcSym, F);
    }
}

void CodeGeneration::exitStructDef(bluefinParser::StructDefContext* ctx)
{
    shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbol(ctx));
    Function* ctor = structHelper.getCtor(structSym);

    Builder->CreateRetVoid();
}

void CodeGeneration::enterPrimaryBool(bluefinParser::PrimaryBoolContext* ctx)
{
    string text = ctx->BOOL()->getText();
    assert(text == "true" || text == "false");

    ConstantInt* constantInt = nullptr;
    if (text == "true")
		constantInt = Builder->getTrue();
    else 
		constantInt = Builder->getFalse();

    values.emplace(ctx, constantInt);
}

void CodeGeneration::enterPrimaryInt(bluefinParser::PrimaryIntContext* ctx)
{
    // Since an integer could be automatically promoted to a float (eg, 3+4.5), 
    // Instead of creating an integer, we check if it would be promoted and if so, create a float instead
    // This way, we don't need to turn our ConstantInt Value* into a float later in exitAddExpr or other methods
    string text = ctx->INT()->getText();
    int i32Val = std::stoi(text);

    if (typeContexts.at(ctx).getPromotionType() == Type::INT()) // stay as an int. eg) 3+4
		values.emplace(ctx, Builder->getInt32(i32Val));
    else
		values.emplace(ctx, ConstantFP::get(*TheContext, APFloat((float)i32Val)));
}

void CodeGeneration::enterPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx)
{
    string text = ctx->FLOAT()->getText();
    float floatVal = std::stof(text);

    values.emplace(ctx, ConstantFP::get(*TheContext, APFloat(floatVal)));
}

void CodeGeneration::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx)
{
    /* For primary IDs, they may need to go thru type promotion. 
    * Eg) int a; a + 6.5;, in which the primaryId 'a' is promoted from int to float
    * This means we must look not only at the existing type, bu also that specific TypeContext's promotion type and cast it
    */
	string varName = ctx->ID()->getText();
	shared_ptr<Symbol> resolvedSym = symbolTable.getResolvedSymbol(ctx);
    bool isResolvedSymStructField = (dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(resolvedSym)) != nullptr);
    Value* val = nullptr;

    // TODO: for now, we assume no external struct member access nor chaining
	// the (mutable) primary Id must be either an internal struct member, local stack or is a global. For latter two, we load it into memory
    if (isResolvedSymStructField) {

        // if the resolved symbol is a struct field, then right now, this primary id 
        // would be generated either into either a ctor (inside a vardecl's expr()) or a method

        shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(resolvedSym));

        LLVMType* structType = getLLVMType(structSym->getType());
        Value* thisPtr = currentMethodThis; // structHelper.getCtorThisPtr(structSym); // this primaryId may be in a method and not a ctor. How to distinguish which to get?

		Value* zero  = Builder->getInt32(0);
		size_t memberIndex = structSym->getFieldIndex(varName);
		Value* index  = Builder->getInt32(memberIndex);
		Value* indices[2] = { zero, index };

		Value* memberPtr = Builder->CreateGEP(structType, thisPtr, indices, "memPtr_" + varName);
		assert(isa<GetElementPtrInst>(memberPtr));
		GetElementPtrInst* castedMemAddr = dyn_cast<GetElementPtrInst>(memberPtr);

        elementPtrs.emplace(ctx, castedMemAddr);

		val = Builder->CreateLoad(memberPtr, "loadtmp_" + varName);
	}
    else {
        val = resolvedSymAndValues.at(resolvedSym);
        if (val != nullptr && isa<AllocaInst, GlobalVariable>(val) == false)
            assert(false);
        val = Builder->CreateLoad(val, "loadtmp_" + varName);
    }

	Type varIdEvalType = typeContexts.at(ctx).getEvalType();
	Type varIdPromoType = typeContexts.at(ctx).getPromotionType();

	if (varIdPromoType == Type::FLOAT() && varIdEvalType == Type::INT()) { // cast left and right to float
		val = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, val, LLVMType::getFloatTy(*TheContext), "casttmp");
	}

	values.emplace(ctx, val);
}

void CodeGeneration::exitPrimaryParenth(bluefinParser::PrimaryParenthContext* ctx)
{
    Value* val = values.at(ctx->expr());
    values.emplace(ctx, val); // place same Value* as its child (which the parenth surrounds)
}

void CodeGeneration::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx)
{
    ParseTree* childParseTree = ctx->expr();
    Value* val = values.at(childParseTree);
    LLVMType* t = val->getType();
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(childParseTree).getPromotionType());
    Type exprType = typeContexts.at(ctx).getPromotionType();

    string opText = ctx->op->getText();
    assert(opText == "!" || opText == "-");

    // TODO: handle !
    Value* expr = nullptr;
    if (opText == "-") {
        if (exprType == Type::INT())
			expr = Builder->CreateNeg(val, "negtmp");
        else
			expr = Builder->CreateFNeg(val, "negtmp");
    }
    else
        expr = Builder->CreateNot(val, "nottmp");
        
    values.emplace(ctx, expr);
}

void CodeGeneration::exitMultiExpr(bluefinParser::MultiExprContext* ctx)
{
    ParseTree* leftParseTree = ctx->expr(0);
    ParseTree* rightParseTree = ctx->expr(1);
    assert(typeContexts.at(leftParseTree).getPromotionType() == typeContexts.at(rightParseTree).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(rightParseTree).getPromotionType());
    Type exprEvalType = typeContexts.at(ctx).getEvalType();
    Type exprPromoType = typeContexts.at(ctx).getPromotionType();

    Value* left = values.at(leftParseTree);
    Value* right = values.at(rightParseTree);

    string opText = ctx->op->getText();
    assert(opText == "*" || opText == "/");

    // for now, only int. TODO: add float
    Value* expr = nullptr;
    if (opText == "*") {
        if (exprEvalType == Type::INT())
			expr = Builder->CreateMul(left, right, "multmp");
        else
			expr = Builder->CreateFMul(left, right, "multmp");
    }
    else {
        if (exprEvalType == Type::INT())
			expr = Builder->CreateSDiv(left, right, "divtmp");
        else
			expr = Builder->CreateFDiv(left, right, "divtmp");
    }

    // The *'s expr node may have a promotion type of float, whereas the individual subnodes' promotion type is to int
    // eg) int val; a*2.0 - val/2, for val/2, both have promotion type to int, but the expression is float, so 
    // we need to convert the final expr to float
    if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { // cast left and right to float
        expr = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, expr, LLVMType::getFloatTy(*TheContext), "casttmp");
    }

    values.emplace(ctx, expr);
}

void CodeGeneration::exitAddExpr(bluefinParser::AddExprContext* ctx)
{
    ParseTree* leftParseTree = ctx->expr(0);
    ParseTree* rightParseTree = ctx->expr(1);
    assert(typeContexts.at(leftParseTree).getPromotionType() == typeContexts.at(rightParseTree).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(rightParseTree).getPromotionType());
    Type exprEvalType = typeContexts.at(ctx).getEvalType();
    Type exprPromoType = typeContexts.at(ctx).getPromotionType();

    Value* left = values.at(leftParseTree);
    Value* right = values.at(rightParseTree);

    string opText = ctx->op->getText();
    assert(opText == "+" || opText == "-");

    // for now, only int
    Value* expr = nullptr;
    if (opText == "+") {
        if (exprEvalType == Type::INT())
            expr = Builder->CreateAdd(left, right, "addtmp");
        else 
			expr = Builder->CreateFAdd(left, right, "addtmp");
    }
    else {
        if (exprEvalType == Type::INT())
            expr = Builder->CreateSub(left, right, "subtmp");
        else
			expr = Builder->CreateFSub(left, right, "subtmp");
    }

    if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { // cast left and right to float
        expr = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, expr, LLVMType::getFloatTy(*TheContext), "casttmp");
    }

    values.emplace(ctx, expr);
}

void CodeGeneration::exitRelExpr(bluefinParser::RelExprContext* ctx)
{
    ParseTree* leftParseTree = ctx->expr(0);
    ParseTree* rightParseTree = ctx->expr(1);

    assert(typeContexts.at(leftParseTree).getPromotionType() == typeContexts.at(rightParseTree).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(ctx).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == Type::BOOL());
    Type operandType = typeContexts.at(leftParseTree).getPromotionType(); // same as right one

    Value* left = values.at(ctx->expr(0));
    Value* right = values.at(ctx->expr(1));

    string opText = ctx->op->getText();
    assert(opText == "<" || opText == "<=" || opText == ">" || opText == ">=");

    Value* expr = nullptr;
    if (operandType == Type::INT()) {
        if (opText == "<")
            expr = Builder->CreateICmpSLT(left, right, "cmpSLTtmp");
        else if (opText == "<=")
            expr = Builder->CreateICmpSLE(left, right, "cmpSLEtmp");
        else if (opText == ">")
            expr = Builder->CreateICmpSGT(left, right, "cmpSGTtmp");
        else
            expr = Builder->CreateICmpSGE(left, right, "cmpSGEtmp");
    }
    else {
        if (opText == "<")
            expr = Builder->CreateFCmpOLT(left, right, "cmpFLTtmp");
        else if (opText == "<=")
            expr = Builder->CreateFCmpOLE(left, right, "cmpFLEtmp");
        else if (opText == ">")
            expr = Builder->CreateFCmpOGT(left, right, "cmpFGTtmp");
        else
            expr = Builder->CreateFCmpOGE(left, right, "cmpFGEtmp");
    }

    values.emplace(ctx, expr);
}

void CodeGeneration::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx)
{
    ParseTree* leftParseTree = ctx->expr(0);
    ParseTree* rightParseTree = ctx->expr(1);
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(ctx).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == Type::BOOL());
    Type operandType = typeContexts.at(leftParseTree).getPromotionType(); // same as right one

    Value* left = values.at(ctx->expr(0));
    Value* right = values.at(ctx->expr(1));

    string opText = ctx->op->getText();
    assert(opText == "==" || opText == "!=");

    Value* expr = nullptr;
    if (operandType == Type::INT() || operandType == Type::BOOL()) {
		if (opText == "==")
			expr = Builder->CreateICmpEQ(left, right, "cmpEQtmp");
		else
			expr = Builder->CreateICmpNE(left, right, "cmpNEtmp");
    }
    else {
		if (opText == "==")
			expr = Builder->CreateFCmpOEQ(left, right, "cmpEQtmp");
		else
			expr = Builder->CreateFCmpONE(left, right, "cmpNEtmp");
    }

    values.emplace(ctx, expr);
}

void CodeGeneration::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx)
{
    Value* left = values.at(ctx->expr(0));
    Value* right = values.at(ctx->expr(1));

    Value* expr = Builder->CreateAnd(left, right, "andtmp");
    values.emplace(ctx, expr);
}

void CodeGeneration::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx)
{
    Value* left = values.at(ctx->expr(0));
    Value* right = values.at(ctx->expr(1));

    Value* expr = Builder->CreateOr(left, right, "ortmp");
    values.emplace(ctx, expr);
}

// TODO: We don't check l-value or r-value, so an assignmetn like 'a+3 = 6' will lead
// to an unresolved expr for ctx->expr(0) and will crash the code generator.
// Note: Visiting the primaryId on the lhs will automatically generate a load for it
// even though it is totally useless and won't be used
void CodeGeneration::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx)
{
    Value* address = nullptr;

    if (auto leftCtx = dynamic_cast<bluefinParser::MemberAccessContext*>(ctx->expr(0)))
        address = elementPtrs.at(leftCtx);
    else {
        // left sym is a primaryId (the root, eg, the 'a' in 'a.b.c')
        // Set address to the globalVar or local address of the root
		shared_ptr<Symbol> leftSym = symbolTable.getResolvedSymbol(ctx->expr(0));
		address = resolvedSymAndValues.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(address) == false)
			assert(false);
    }

	Value* exprVal = values.at(ctx->expr(1));
	LLVMType* t = exprVal->getType();
	Builder->CreateStore(exprVal, address);

	// Since we allow chaining assignments together, this Context* node may be the rhs child of another 
	// SimpleAssignExprContext* node or a VarDeclContext* node. As a result, associate this node with the
	// value of its rhs expr and do cast the rhs if needed
	//. eg) floatVal = intVal = 8;, where 'intVal = 8' should be cast to a float
	Type exprEvalType = typeContexts.at(ctx).getEvalType();
	Type exprPromoType = typeContexts.at(ctx).getPromotionType();
	if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { // cast left and right to float
		exprVal = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, exprVal, LLVMType::getFloatTy(*TheContext), "casttmp");
	}
	values.emplace(ctx, exprVal);
}

void CodeGeneration::exitFuncCall(bluefinParser::FuncCallContext* ctx)
{
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getResolvedSymbol(ctx));
    assert(funcSym != nullptr);

    string id = ctx->ID()->getText();
    Function* F = nullptr;
    vector<Value*> args;
    // The resolved FunctionSymbol that corresponds to this funcCall is either a global function or a struct's method
    // If it's a struct's method, that means we must currently be inside the ctor or a method itself 
    // In either case, we must remember to pass in the "this" variable

    if (isStructMethod(funcSym, symbolTable)) {
        F = methodToLLVMFunctions.at(funcSym);
        args.push_back(currentMethodThis);
    }
    else {
        F = TheModule->getFunction(id);
    }

    // NOTE: argList() is null is no children, so we use funcSym->getParams().size() instead
    for (int i = 0; i < funcSym->getParams().size(); i++) {
        Value* val = values.at(ctx->argList()->expr(i));
        args.push_back(val);
    }

    string str = dump();

    string twine = (F->getReturnType() == LLVMType::getVoidTy(*TheContext)) ? "" : "calltmp"; // if the function ret type is void
    // the function call must not have a name (other the return value doesn't exist). Otherwise, Builder->CreateCall(..) will trigger an assertion
    Value* funcRetVal = Builder->CreateCall(F, args, twine);
    Type exprEvalType = typeContexts.at(ctx).getEvalType();
    Type exprPromoType = typeContexts.at(ctx).getPromotionType();

    // We want to check whether the return value of the function call needs to be promoted
    if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { // cast left and right to float
        funcRetVal = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, funcRetVal, LLVMType::getFloatTy(*TheContext), "casttmp");
    }
    values.emplace(ctx, funcRetVal);
}

/*
The logic here requires some explanation. LLVM requires that all BasicBlocks contain only one terminator (ret or br), which
must be at the last instruction. As a result, after the IR of a return statement is generated and placed into the BB,
to prevent further instructions from piling in (which would cause the return's IR to no longer be the last instruction),
we immediately create a new BB and insert point to store other instructions.

We also know that all other instructions in the same block as the return statement can never be executed. As a result,
we call the BB "impossible". Now, upon exiting a funcDef, we remove all "impossible" instructions since they can't ever
be reached. (also, the last impossible label may not have a terminator)
*/
void CodeGeneration::exitStmtReturn(bluefinParser::StmtReturnContext* ctx)
{
    if (ctx->expr()) {
        Value* val = values.at(ctx->expr());
        Builder->CreateRet(val);
    }
    else
        Builder->CreateRetVoid();


    Function* TheFunction = Builder->GetInsertBlock()->getParent();
    BasicBlock* impossibleBB = BasicBlock::Create(*TheContext, "impossible", TheFunction);
    Builder->SetInsertPoint(impossibleBB);
}

void CodeGeneration::exitMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
    // memberAccess calls getelementptr, which requires the StructType* and the memory location of the left val.
    // How to get StructType*? From our previous passes, we can use symbol table to get the left sides's resolved symbol, then the 
    // StructSymbol that matches with its type, and get its StructType.
    // To get the memory location, if this is the base (eg, 'a' in 'a.b.c'), we simply look at resolvedSymAndValue
    // Otherwise, we look at elementPtrs
	shared_ptr<Symbol> leftSym = symbolTable.getResolvedSymbol(ctx->expr());
    Value* address = nullptr;

    string here = dump();
    // if the left val corresponds to an elementPtr (either we're in a chained external member access or the root
    // which is always a primaryId, refers to an internal member (so we're inside a field varDecl. Then use that 
    // as the address
    //if (auto leftCtx = dynamic_cast<bluefinParser::MemberAccessContext*>(ctx->expr()))
        //address = elementPtrs.at(leftCtx);
    if (elementPtrs.count(ctx->expr()) == 1)
        address = elementPtrs.at(ctx->expr());
    else {
        // left sym is a primaryId (the root, eg, the 'a' in 'a.b.c')
        // Set address to the globalVar or local address of the root
		address = resolvedSymAndValues.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(address) == false) 
			assert(false);
		// AMAZINGLY, this is a bug! assert(isa<AllocaInst, GlobalVariable>(lhs)); 
    }

    shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(leftSym->getType()));
    assert(structSym != nullptr);

    LLVMType* structType = getLLVMType(structSym->getType());

    string memName = ctx->ID()->getText();
	Value* zero  = Builder->getInt32(0);
	size_t memberIndex = structSym->getFieldIndex(memName);
	Value* index  = Builder->getInt32(memberIndex);
	Value* indices[2] = { zero, index };

    Value* memAddr = Builder->CreateGEP(structType, address, indices, "memPtr_" + memName);
    assert(isa<GetElementPtrInst>(memAddr));
    GetElementPtrInst* castedMemAddr = dyn_cast<GetElementPtrInst>(memAddr);

    elementPtrs.emplace(ctx, castedMemAddr);

    // TODO: Refactor. To allow this member access to be used, we load it. However, this means for each chained member access
    // we load intermediate members, whose values would never actually be used, which is unnecessary redundancy
    // eg) for "a.b.c";, load will be created for "b" and "c" even though the load for "b" isn't needed (actually, primaryId "a' 
    // will also trigger a load
	Value* val = Builder->CreateLoad(memAddr, "loadtmp_" + memName);
	Type varIdEvalType = typeContexts.at(ctx).getEvalType();
	Type varIdPromoType = typeContexts.at(ctx).getPromotionType();

	if (varIdPromoType == Type::FLOAT() && varIdEvalType == Type::INT()) { // cast left and right to float
		val = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, val, LLVMType::getFloatTy(*TheContext), "casttmp");
	}

	values.emplace(ctx, val);
}

void CodeGeneration::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
    // NOTE: for now, we don't handle chained method calls. eg) no 'a.b().c()', only 'a.b()' and 'a.b.c()'
    // To make the method call, we need the relevant pointer to the struct instance, the params, and 
    // of course, the function itself.
    shared_ptr<Symbol> leftSymbol = symbolTable.getResolvedSymbol(ctx->expr());
    shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(leftSymbol->getType()));
    string memName = ctx->ID()->getText();
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.resolveMember(memName, structSym));
    assert(funcSym != nullptr);

    Function* F = methodToLLVMFunctions.at(funcSym);

    Value* leftAddr = nullptr;
    if (elementPtrs.count(ctx->expr()) == 1)
        leftAddr = elementPtrs.at(ctx->expr());
    else {
        // left sym is a primaryId (the root, eg, the 'a' in 'a.b.c')
        // Set address to the globalVar or local address of the root
		shared_ptr<Symbol> leftSym = symbolTable.getResolvedSymbol(ctx->expr());
		leftAddr = resolvedSymAndValues.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(leftAddr) == false)
			assert(false);
    }
    vector<Value*> args;
    args.push_back(leftAddr);
	for (int i = 0; i < funcSym->getParams().size(); i++) {
		Value* val = values.at(ctx->argList()->expr(i));
		args.push_back(val);
	}

    string twine = (F->getReturnType() == LLVMType::getVoidTy(*TheContext)) ? "" : "calltmp"; // if the function ret type is void
	Value* funcRetVal = Builder->CreateCall(F, args, twine);
    Type exprEvalType = typeContexts.at(ctx).getEvalType();
    Type exprPromoType = typeContexts.at(ctx).getPromotionType();

    // We want to check whether the return value of the function call needs to be promoted
    if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { // cast left and right to float
        funcRetVal = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, funcRetVal, LLVMType::getFloatTy(*TheContext), "casttmp");
    }
    values.emplace(ctx, funcRetVal);
}

void CodeGeneration::enterStmtIf(bluefinParser::StmtIfContext* ctx) {

    bool containsElseBlock = ctx->block().size() == 2;
	Function* TheFunction = Builder->GetInsertBlock()->getParent();

    if (containsElseBlock) {
		BasicBlock* thenBB = BasicBlock::Create(*TheContext, "then", TheFunction); 
		BasicBlock* elseBB = BasicBlock::Create(*TheContext, "else", TheFunction);
		BasicBlock* mergeBB = BasicBlock::Create(*TheContext, "merge", TheFunction);

        ifStmtHelper.addIfElseStmt({ ctx->block(0), thenBB }, { ctx->block(1), elseBB }, mergeBB, ctx->expr(), ctx);
    }
    else {
		BasicBlock* thenBB = BasicBlock::Create(*TheContext, "then", TheFunction); 
		BasicBlock* mergeBB = BasicBlock::Create(*TheContext, "merge", TheFunction);

        ifStmtHelper.addIfStmt({ ctx->block(0), thenBB }, mergeBB, ctx->expr(), ctx);
    }
}

void CodeGeneration::exitStmtIf(bluefinParser::StmtIfContext* ctx)
{
    BasicBlock* mergeBB = ifStmtHelper.getBBForMerge(ctx);
    assert(mergeBB != nullptr);
	Builder->SetInsertPoint(mergeBB);
}

void CodeGeneration::enterStmtWhile(bluefinParser::StmtWhileContext* ctx)
{
	Function* TheFunction = Builder->GetInsertBlock()->getParent();
	BasicBlock* whileCmpBB = BasicBlock::Create(*TheContext, "whileLoopCmp", TheFunction); 
	BasicBlock* whileBodyBB = BasicBlock::Create(*TheContext, "whileLoopBody", TheFunction);
	BasicBlock* afterWhileBB = BasicBlock::Create(*TheContext, "afterWhileLoop", TheFunction);

    Builder->CreateBr(whileCmpBB);
    Builder->SetInsertPoint(whileCmpBB);

    whileStmtHelper.addWhileStmt(whileCmpBB, whileBodyBB, afterWhileBB, ctx->expr(), ctx->block(), ctx);
}

void CodeGeneration::exitStmtWhile(bluefinParser::StmtWhileContext* ctx)
{
    BasicBlock* afterWhileBB = whileStmtHelper.getBBForAfterLoop(ctx);
    BasicBlock* whileCmpBB = whileStmtHelper.getBBForLoopCmp(ctx);
    Builder->CreateBr(whileCmpBB);
    Builder->SetInsertPoint(afterWhileBB);
}

void CodeGeneration::enterBlock(bluefinParser::BlockContext* ctx)
{ 
    if (ifStmtHelper.isThenBlockNode(ctx)) {
        bluefinParser::ExprContext* expr = ifStmtHelper.getComparisonExpr(ctx);
        Value* val = values.at(expr);

        llvm::BasicBlock* thenBB = ifStmtHelper.getBBForThen(ctx);
        
        llvm::BasicBlock* nextBB = nullptr; // if the "if" stmt has an else block, set to the else label. If it doesn't, set to merge label
        if (ifStmtHelper.hasCorrespondingElseNode(ctx))
            nextBB = ifStmtHelper.getBBForElse(ctx);
        else
            nextBB = ifStmtHelper.getBBForMerge(ctx);

        Builder->CreateCondBr(val, thenBB, nextBB);
        Builder->SetInsertPoint(thenBB);
    }
    else if (whileStmtHelper.isWhileBlockNode(ctx)) {
        bluefinParser::ExprContext* expr = whileStmtHelper.getComparisonExpr(ctx);
        Value* val = values.at(expr);
        
        BasicBlock* whileBodyBB = whileStmtHelper.getBBForLoopBody(ctx);
        BasicBlock* afterWhileBB = whileStmtHelper.getBBForAfterLoop(ctx);

        Builder->CreateCondBr(val, whileBodyBB, afterWhileBB);
        Builder->SetInsertPoint(whileBodyBB);
    }
}

void CodeGeneration::exitBlock(bluefinParser::BlockContext* ctx)
{
    if (ifStmtHelper.isThenBlockNode(ctx) || ifStmtHelper.isElseBlockNode(ctx)) {
        llvm::BasicBlock* mergeBB = ifStmtHelper.getBBForMerge(ctx);
		Builder->CreateBr(mergeBB);

		if (ifStmtHelper.isThenBlockNode(ctx) && ifStmtHelper.hasCorrespondingElseNode(ctx)) {
			llvm::BasicBlock* elseBlock = ifStmtHelper.getBBForElse(ctx);
			Builder->SetInsertPoint(elseBlock);
		}
    }
}

void CodeGeneration::exitProgram(bluefinParser::ProgramContext* ctx)
{
    // Here, we create an internal function that will be put into llvm.global_ctor.
    // This function calls every internal function that had been created during
    // global VarDecl initializations (aka, VarDecls whose expr was not a compile-time constant)

    if (!internalFunctionsForVarDeclExpr.empty()) {
        Function* globalInitFunction = llvm::getOrCreateInitFunction(*TheModule, "_VarDeclInitializations");
        globalInitFunction->setLinkage(GlobalValue::LinkageTypes::InternalLinkage);
        BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", globalInitFunction);
        Builder->SetInsertPoint(BB);

        for (Function* func : internalFunctionsForVarDeclExpr) {
            Builder->CreateCall(func);
        }

        Builder->CreateRetVoid();
    }
}

string CodeGeneration::dump() {

    string str;
    raw_string_ostream stream(str);
    
    TheModule->print(stream, nullptr);
    stream.flush();
    return str;

    //TheModule->dump();
}

bool CodeGeneration::isCodeGenOK()
{
    //raw_ostream stream;
    raw_fd_ostream& stream = llvm::outs();

    bool isBroken = verifyModule(*TheModule, &stream);
    stream.flush();
    return !isBroken;
}

LLVMType* CodeGeneration::getLLVMType(Type t) const
{
    assert(t != Type::STRING()); // we're not ready to implement strings yet

    if (t == Type::BOOL()) return LLVMType::getInt1Ty(*TheContext);
    if (t == Type::FLOAT()) return LLVMType::getFloatTy(*TheContext);
    if (t == Type::INT()) return LLVMType::getInt32Ty(*TheContext);
    if (t == Type::VOID()) return LLVMType::getVoidTy(*TheContext);

    return bluefinToLLVMTypes.at(t);
}

Function* CodeGeneration::createCtor(shared_ptr<StructSymbol> structSym)
{
    LLVMType* t = getLLVMType(structSym->getType());
	StructType* structType = dyn_cast<StructType>(t);
    assert(structType != nullptr);

    vector<LLVMType*> ctorParam; // since this is the ctor, the type should be only
    ctorParam.push_back(structType->getPointerTo());

    FunctionType* FT = FunctionType::get(getLLVMType(Type::VOID()), ctorParam, false);
    Function* ctor = Function::Create(FT, Function::ExternalLinkage, "_ctor_" + structSym->getName(), TheModule.get());
    ctor->getArg(0)->setName("this");

    BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", ctor);
    Builder->SetInsertPoint(BB);
    return ctor;
}

/// ================================== IfStmHelper ========================================
 
bool IfStmtHelper::isThenBlockNode(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.thenBlockNode == ctx) {
            return true;
        }
    }
    return false;
}

bool IfStmtHelper::isElseBlockNode(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.elseBlockNode == ctx) {
            return true;
        }
    }
    return false;
}

// ctx must correspond to the if block's ctx. 
bluefinParser::ExprContext* IfStmtHelper::getComparisonExpr(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.thenBlockNode == ctx) {
            return info.exprNode;
        }
    }
    return nullptr;
}

void IfStmtHelper::addIfElseStmt(pair<bluefinParser::BlockContext*, llvm::BasicBlock*> thenPair, pair<bluefinParser::BlockContext*, llvm::BasicBlock*> elsePair, 
    llvm::BasicBlock* mergeBlock, bluefinParser::ExprContext* exprNode, bluefinParser::StmtIfContext* ifNode)
{
    IfStmtInfo info;
    info.thenBlockNode = thenPair.first;
    info.llvmThenBlockLabel = thenPair.second;
    info.elseBlockNode = elsePair.first;
    info.llvmElseBlockLabel = elsePair.second;
    info.llvmMergeBlockLabel = mergeBlock;
    info.exprNode = exprNode;;
    info.ifNode = ifNode;
   
    infos.push_back(info);
}

void IfStmtHelper::addIfStmt(pair<bluefinParser::BlockContext*, llvm::BasicBlock*> thenPair, 
    llvm::BasicBlock* mergeBlock, bluefinParser::ExprContext* exprNode, bluefinParser::StmtIfContext* ifNode)
{
    IfStmtInfo info;
    info.thenBlockNode = thenPair.first;
    info.llvmThenBlockLabel = thenPair.second;
    info.elseBlockNode = nullptr;
    info.llvmElseBlockLabel = nullptr;
    info.llvmMergeBlockLabel = mergeBlock;
    info.exprNode = exprNode;;
    info.ifNode = ifNode;
   
    infos.push_back(info);
}

// Given the BlockContext* for a 'if' block, check whether there will be an 'else' block
// The result is later used for retrieving either the else block's label or merge's label
bool IfStmtHelper::hasCorrespondingElseNode(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.thenBlockNode == ctx) {
            // If else block is not null, return it. Otherwise, return the merge block. 
            if (info.elseBlockNode) return true;
        }
    }
    return false;
}

llvm::BasicBlock* IfStmtHelper::getBBForElse(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.elseBlockNode == ctx || info.thenBlockNode == ctx) {
            return info.llvmElseBlockLabel;
        }
    }

    return nullptr;
}

llvm::BasicBlock* IfStmtHelper::getBBForThen(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.thenBlockNode == ctx) {
            return info.llvmThenBlockLabel;
        }
    }

    return nullptr;
}

llvm::BasicBlock* IfStmtHelper::getBBForMerge(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.thenBlockNode == ctx || info.elseBlockNode == ctx) {
            return info.llvmMergeBlockLabel;
        }
    }

    return nullptr;
}

llvm::BasicBlock* IfStmtHelper::getBBForMerge(bluefinParser::StmtIfContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.ifNode == ctx) {
            return info.llvmMergeBlockLabel;
        }
    }

    return nullptr;
}

// ============================== WhileStmtHelper ===============================

void WhileStmtHelper::addWhileStmt(llvm::BasicBlock* llvmWhileLoopCmpLabel, llvm::BasicBlock* llvmWhileLoopBodyLabel, llvm::BasicBlock* llvmAfterWhileLoopLabel, 
    bluefinParser::ExprContext* exprNode, bluefinParser::BlockContext* whileBlockNode, bluefinParser::StmtWhileContext* whileStmtNode)
{
    WhileStmtInfo info;
    info.llvmWhileLoopCmpLabel = llvmWhileLoopCmpLabel;
    info.llvmWhileLoopBodyLabel = llvmWhileLoopBodyLabel;
    info.llvmAfterWhileLoopLabel = llvmAfterWhileLoopLabel;
    info.exprNode = exprNode;
    info.whileBlockNode = whileBlockNode;
    info.whileStmtNode = whileStmtNode;

    infos.push_back(info);
}

bluefinParser::ExprContext* WhileStmtHelper::getComparisonExpr(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.whileBlockNode == ctx)
            return info.exprNode;
    }

    return nullptr;
}

llvm::BasicBlock* WhileStmtHelper::getBBForLoopCmp(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.whileBlockNode == ctx)
            return info.llvmWhileLoopCmpLabel;
    }

    return nullptr;
}

llvm::BasicBlock* WhileStmtHelper::getBBForLoopBody(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.whileBlockNode == ctx)
            return info.llvmWhileLoopBodyLabel;
    }

    return nullptr;
}

llvm::BasicBlock* WhileStmtHelper::getBBForAfterLoop(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.whileBlockNode == ctx)
            return info.llvmAfterWhileLoopLabel;
    }

    return nullptr;
}

llvm::BasicBlock* WhileStmtHelper::getBBForLoopCmp(bluefinParser::StmtWhileContext* ctx) const
{
    return getBBForLoopCmp(ctx->block());
}

llvm::BasicBlock* WhileStmtHelper::getBBForAfterLoop(bluefinParser::StmtWhileContext* ctx) const
{
    return getBBForAfterLoop(ctx->block());
}

bool WhileStmtHelper::isWhileBlockNode(bluefinParser::BlockContext* ctx) const
{
    for (const auto& info : infos) {
        if (info.whileBlockNode == ctx)
            return true;
    }

    return false;
}

void StructHelper::addStructDef(shared_ptr<StructSymbol> structSym, llvm::Function* ctor)
{
    structSymbolAndFuncs.emplace(structSym, ctor);
}

llvm::Function* StructHelper::getCtor(shared_ptr<StructSymbol> structSym)
{
    return structSymbolAndFuncs.at(structSym);
}

llvm::Value* StructHelper::getCtorThisPtr(shared_ptr<StructSymbol> structSym)
{
    llvm::Function* ctor = getCtor(structSym);
    return ctor->getArg(0); // return the "this"
}

