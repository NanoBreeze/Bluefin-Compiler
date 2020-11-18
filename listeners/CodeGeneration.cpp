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
	Type varDeclType = sym->getType();
    LLVMType* llvmType = getLLVMType(varDeclType);

    // This if stmt checks whether we're operating on a global VarDecl, local VarDecl, or struct def member
    if (isGlobalVarDecl(ctx, symbolTable)) {
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

		resolvedSymAndAddrs.emplace(sym, v);

        if (ctx->expr() || varDeclType.isUserDefinedType()) { // if there's an expression, it either can be constant folded or requires calling 
            // other funcs or primaryIds, which would generate code. We don't know which right now, so we create
            // new internal function for it. The expr could also be a primaryInt/primaryFloat/etc, which we could find out here,
            // but for consistency, we choose to deal with that case same way as above.
            // If the variable is a structDef, also create the internal function and call the struct's ctor
            
            Function* internalF = createGlobalVarDeclFunction(id);
            functionsThatInitializeGlobalVarDecl.emplace(v, internalF);
            BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", internalF);
            Builder->SetInsertPoint(BB);
        }
    }
    else if (isLocalVarDecl(ctx, symbolTable)) { // local varDecl 
        // local variables are allocated on the stack
        Value* allocaVal = Builder->CreateAlloca(llvmType, nullptr, "alloctmp_" + id); // NOTE, we're storing the AllocaInst here
        values.emplace(ctx, allocaVal);
		resolvedSymAndAddrs.emplace(sym, allocaVal); 

        // if the type is a struct, then call the struct's its corresponding ctor
        if (varDeclType.isUserDefinedType()) {
            
            shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(varDeclType));
            Function* ctor = structHelper.getCtor(structSym);
            Builder->CreateCall(ctor, allocaVal);
        }
    }
    else {
        // struct member field
    }
}

void CodeGeneration::exitVarDecl(bluefinParser::VarDeclContext* ctx)
{
    string id = ctx->ID()->getText();
    shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);

    if (isGlobalVarDecl(ctx, symbolTable)) {

        shared_ptr<Symbol> sym = symbolTable.getSymbol(ctx);
        Type symType = sym->getType();
        Value* globalAddr = resolvedSymAndAddrs.at(sym);
        assert(isa<GlobalVariable>(globalAddr));

        if (symType.isUserDefinedType()) { // simply a bare global var. eg) Normal norm;
            assert(!ctx->expr()); // don't support VarDecl with assignment of struct type. eg) Normal norm = normmm; is not allowed
			Function* internalF = functionsThatInitializeGlobalVarDecl.at(dyn_cast<GlobalVariable>(globalAddr));
            shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(symType));
            Function* ctor = structHelper.getCtor(structSym);
            Builder->CreateCall(ctor, globalAddr);
            Builder->CreateRetVoid();
            internalFunctionsForVarDeclExpr.push_back(internalF);
        }
        else if (ctx->expr()) {
            Value* val = values.at(ctx->expr());
			Function* internalF = functionsThatInitializeGlobalVarDecl.at(dyn_cast<GlobalVariable>(globalAddr));
            if (isa<Constant>(val)) {
				// if the variable declaration is for a global and it contains an expr, then we will need
				// to update the global's value (since it was originally set to 0)
                Constant* c = dyn_cast<Constant>(val);
                TheModule->getNamedGlobal(id)->setInitializer(c);
                // Remove the internal function since the expr() is a compile-time constant and we can update vardecl 
                internalF->eraseFromParent();

            }
            else { // non-compile-time assignment. Eg) int d = foo();
                // At this point, we finished evaluating the expr and our insertion point is still inside the internal function
                // So store the expr's evaluated value to the global var, and then close it off with a ret void
                Builder->CreateStore(val, globalAddr);
                Builder->CreateRetVoid();

                internalFunctionsForVarDeclExpr.push_back(internalF);
            }
        }
    }
    else if (isLocalVarDecl(ctx, symbolTable) && ctx->expr()) {
        // local var with an expr. Store the value of the expr into the local var
        Value* exprVal = values.at(ctx->expr());
        Value* allocaInst = values.at(ctx);

        Builder->CreateStore(exprVal, allocaInst);
    }
    else if (isStructField(ctx, symbolTable)) {
        // if we're in here, that means the current BB insertion point should be in the ctor.
        // if user-defined type, then call ctor
		Type varDeclType = symbolTable.getSymbol(ctx)->getType();
		shared_ptr<StructSymbol> structSym = getContainingStruct(ctx, symbolTable);
		Value* thisPtr = structHelper.getCtorThisPtr(structSym);

        if (varDeclType.isUserDefinedType()) {
            shared_ptr<StructSymbol> structOfVarDeclType = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(varDeclType));

            Function* varDeclCtor = structHelper.getCtor(structOfVarDeclType);
            GetElementPtrInst* memPtr = createStructElementAddr(structSym, id, thisPtr);
            Builder->CreateCall(varDeclCtor, memPtr);
        }
        if (ctx->expr()) {
            assert(!varDeclType.isUserDefinedType()); // for now, we don't allow struct assignment inside vardecl. eg) Foo foo = foobar;
			GetElementPtrInst* memPtr = createStructElementAddr(structSym, id, thisPtr);
            Value* exprVal = values.at(ctx->expr());
            Builder->CreateStore(exprVal, memPtr);
        }
    }
}

void CodeGeneration::enterFuncDef(bluefinParser::FuncDefContext* ctx) {

    bool isMethod = isStructMethod(ctx, symbolTable);
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getSymbol(ctx));
    assert(funcSym != nullptr);

    // If the funcDef is a method, then its declaration must have already been generated by enterStructDef
    // so simply set the insertion point and update currentMethodThis to this method's "this"
    if (isMethod) {
        Function* F = methodToLLVMFunctions.at(funcSym);
        Builder->SetInsertPoint(&(F->getEntryBlock()));
        currentMethodThis = F->getArg(0);
    }
    else {

        Function* F = createFunction(funcSym);
        BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", F);
        Builder->SetInsertPoint(BB);

        // since the args can be mutated, create space on the stack that will contain their (possibly mutated) value
        // Future references to the args will refer to the allocated space
        // Add the values to resolvedSymsAndValues, and set the param names for LLVM's code generation (to avoid awkward names like %0, %1)
        vector<shared_ptr<Symbol>> params = funcSym->getParams();
        for (int i = 0; i < params.size(); i++) {
            Type t = params[i]->getType();
            AllocaInst* allocaInst = Builder->CreateAlloca(getLLVMType(t), nullptr, "alloctmp_" + params[i]->getName());
            Builder->CreateStore(F->getArg(i), allocaInst);
            resolvedSymAndAddrs.emplace(params[i], allocaInst);
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

/* We create the struct, then create its methods and start constructing its ctor, 
All fields/aka, VarDecl with expr() will generate code inside the ctor to update their values.
When entering a funcDef, the insertion point will be set to its corresponding method/function, 
and upon exiting, it will be set back to the ctor.
In exitStructDef, we close off the ctor.
*/
void CodeGeneration::enterStructDef(bluefinParser::StructDefContext* ctx)
{
    // Since some some the structDef's varDecl's contain assignment, we create a ctor for every struct def
    // so that those assignment expressions can go into the ctor function
    shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbol(ctx));
    StructType* structType = createStruct(structSym);
    bluefinToLLVMTypes.emplace(structSym->getType(), structType); 

	// Create the Function/method here so that later, internal VarDecl (fields) can forward reference them
    vector<shared_ptr<FunctionSymbol>> methods = structSym->getMethods();
    for (auto funcSym : methods) {
        Function* F = createFunction(funcSym, structType);
		BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", F);
		Builder->SetInsertPoint(BB);

		vector<shared_ptr<Symbol>> params = funcSym->getParams();
		for (int i = 0; i < params.size(); i++) { // remember, no need to allocate for "this" pointer, which is the first arg in function, but not in param
            Type t = params[i]->getType();
			AllocaInst* allocaInst = Builder->CreateAlloca(getLLVMType(t), nullptr, "alloctmp_" + params[i]->getName());
			Builder->CreateStore(F->getArg(i+1), allocaInst);
			resolvedSymAndAddrs.emplace(params[i], allocaInst);
		}

        methodToLLVMFunctions.emplace(funcSym, F);
    }

    Function* ctor = createCtor(structType);
    BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", ctor);
    Builder->SetInsertPoint(BB);

    structHelper.addStructDef(structSym, ctor);
    currentMethodThis = ctor->getArg(0); // referring to ctor
    
    // If the struct has a parent type, call its parent's ctor Function. We will need to bitcast our existing "this" ptr to 
    // the parent's type
    if (shared_ptr<StructSymbol> parentSym = structSym->getSuperClass()) {
        Function* parentCtor = structHelper.getCtor(parentSym);
		LLVMType* t = structHelper.getCtorThisPtr(parentSym)->getType();
        // We bitcast the "this" ptr in case we're calling a parent's method. If not, bitcasting will simply return the original "this" ptr, unchanged 
        Value* currentMethodThisCasted = Builder->CreateBitOrPointerCast(currentMethodThis, t, "memCast");
        Builder->CreateCall(parentCtor, currentMethodThisCasted);
    }

    // Now create the vtable if any methods were declared virtual, and assign the vptr correctly.
    // Make sure the vptr is set after the superclass ctor call
    vector<shared_ptr<FunctionSymbol>> vtableMethods = getVTableMethods(structSym);
    if (vtableMethods.size() > 0) {
		vector<Constant*> virtualMethodPtrs;
		for (auto funcSym : vtableMethods) {
			if (funcSym->isVirtual() || funcSym->isOverride()) {
				Function* F = methodToLLVMFunctions.at(funcSym);
				PointerType* u8PtrType = LLVMType::getInt8PtrTy(*TheContext);
				Constant* virtualMethodPtr = ConstantExpr::getBitCast(F, u8PtrType);
				virtualMethodPtrs.push_back(virtualMethodPtr);
			}
		}

        assert(virtualMethodPtrs.size() > 0);
        ArrayType* arrType = ArrayType::get(LLVMType::getInt8PtrTy(*TheContext), virtualMethodPtrs.size());
        Constant* constantArr = ConstantArray::get(arrType, virtualMethodPtrs);

        GlobalVariable* vtable = new GlobalVariable(*TheModule, constantArr->getType(), false,
            GlobalVariable::LinkageTypes::ExternalLinkage, constantArr, "_vtable_" + structSym->getName());

        // Now store vptr
        // Personally, I think i8Ptr works just as well, but looking at C++'s LLVM IR, they use i32 for the vptr, whereas
        // actually function pointers in the vtable are casted to i8*
		LLVMType* i32 = FunctionType::get(LLVMType::getInt32Ty(*TheContext), true);
		Value* vptrCastedPtr = Builder->CreateBitOrPointerCast(currentMethodThis, i32->getPointerTo()->getPointerTo()->getPointerTo(), "memCast");
        Constant* vtableCasted = ConstantExpr::getBitCast(vtable, i32->getPointerTo()->getPointerTo());
        Builder->CreateStore(vtableCasted, vptrCastedPtr);
    }
}

void CodeGeneration::exitStructDef(bluefinParser::StructDefContext* ctx)
{
    Builder->CreateRetVoid(); // close off the ctor
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

	// the (mutable) primary Id must be either an internal struct member, local stack or is a global. For latter two, we load it into memory
    if (isResolvedSymStructField) {

        // if the resolved symbol is a struct field, then right now, this primary id 
        // would be generated either into either a ctor (inside a vardecl's expr()) or a method

        shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(resolvedSym));
        Value* thisPtr = currentMethodThis;
        LLVMType* t = getLLVMType(structSym->getType());

        // this is pretty clever. The resolved symbol may actually be in a parent/grandparent's structDef.
        // We're currently in the derived structDef. How do we access the parent/grandparent? One way is to
        // bitcast the current address to the relevant StructType since the first member would correspond to the 
        // parent anyways
        thisPtr = Builder->CreateBitOrPointerCast(thisPtr, t->getPointerTo(), "memCast");

        GetElementPtrInst* memPtr = createStructElementAddr(structSym, varName, thisPtr);
        elementPtrs.emplace(ctx, memPtr);

		val = Builder->CreateLoad(memPtr, "loadtmp_" + varName);
	}
    else {
        val = resolvedSymAndAddrs.at(resolvedSym);
        if (val != nullptr && isa<AllocaInst, GlobalVariable>(val) == false)
            assert(false);
        val = Builder->CreateLoad(val, "loadtmp_" + varName);
    }

    val = createCastIfNecessary(ctx, val);

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
    expr = createCastIfNecessary(ctx, expr);
    values.emplace(ctx, expr);
}

void CodeGeneration::exitAddExpr(bluefinParser::AddExprContext* ctx)
{
    ParseTree* leftParseTree = ctx->expr(0);
    ParseTree* rightParseTree = ctx->expr(1);
    assert(typeContexts.at(leftParseTree).getPromotionType() == typeContexts.at(rightParseTree).getPromotionType());
    assert(typeContexts.at(ctx).getEvalType() == typeContexts.at(rightParseTree).getPromotionType());
    Type exprEvalType = typeContexts.at(ctx).getEvalType();

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

    expr = createCastIfNecessary(ctx, expr);
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
// TODO: Visiting the primaryId on the lhs will automatically generate a load for it
// even though it is totally useless and won't be used
void CodeGeneration::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx)
{
    Value* address = nullptr;

    bluefinParser::ExprContext* leftCtx = ctx->expr(0);
    if (elementPtrs.count(leftCtx) == 1)
        address = elementPtrs.at(leftCtx);
    else {
        // left sym is a primaryId (the root, eg, the 'a' in 'a.b.c')
        // Set address to the globalVar or local address of the root
		shared_ptr<Symbol> leftSym = symbolTable.getResolvedSymbol(ctx->expr(0));
		address = resolvedSymAndAddrs.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(address) == false)
			assert(false);
    }

	Value* exprVal = values.at(ctx->expr(1));
	Builder->CreateStore(exprVal, address);

	// Since we allow chaining assignments together, this Context* node may be the rhs child of another 
	// SimpleAssignExprContext* node or a VarDeclContext* node. As a result, associate this node with the
	// value of its rhs expr and do cast the rhs if needed
	//. eg) floatVal = intVal = 8;, where 'intVal = 8' should be cast to a float
    exprVal = createCastIfNecessary(ctx, exprVal);
	values.emplace(ctx, exprVal);
}

void CodeGeneration::exitFuncCall(bluefinParser::FuncCallContext* ctx)
{
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getResolvedSymbol(ctx));
    assert(funcSym != nullptr);

    string id = ctx->ID()->getText();
    Function* F = nullptr;
    Value* methodPtr = nullptr;
    vector<Value*> args;

    // The resolved FunctionSymbol that corresponds to this funcCall is either a global function or a struct's method
    // If it's a struct's method, that means we must currently be inside the ctor or a method itself 
    // In either case, we must remember to pass in the "this" variable
    if (isStructMethod(funcSym, symbolTable)) {
        F = methodToLLVMFunctions.at(funcSym);
        shared_ptr<StructSymbol> structSymContainer = getContainingStruct(funcSym, symbolTable);

        /* Sample code from C++ . Notice that we need to cast Base* to a triple pointer, to laod the double pointer (vptr) and then offset from that
            %5 = load %class.Base*, %class.Base** %4, align 8
		    %6 = bitcast %class.Base* %5 to i32 (%class.Base*)***
			%7 = load i32 (%class.Base*)**, i32 (%class.Base*)*** %6, align 8
			%8 = getelementptr inbounds i32 (%class.Base*)*, i32 (%class.Base*)** %7, i64 0
			%9 = load i32 (%class.Base*)*, i32 (%class.Base*)** %8, align 8
			%10 = call i32 %9(%class.Base* %5)
        */
        if (funcSym->isVirtual() || funcSym->isOverride()) { // if it contains the 'virtual' or 'override' keyword, then use vptr
            LLVMType* funcTypePtr = F->getType();
			Value* triplePtr = Builder->CreateBitOrPointerCast(currentMethodThis, funcTypePtr->getPointerTo()->getPointerTo(), "memCast");
            Value* vptrLoaded = Builder->CreateLoad(triplePtr, "vptrLoaded");

            int memberIndex = getVTableMethodIndex(structSymContainer, funcSym->getName());
			Value* index = Builder->getInt64(memberIndex);
			Value* methodPtrPtr = Builder->CreateGEP(funcTypePtr, vptrLoaded, index, "vtableMethodPtr");

            methodPtr = Builder->CreateLoad(methodPtrPtr, "vtableMethod");
            assert(isa<FunctionType>(funcTypePtr->getPointerElementType()));
        }

        LLVMType* t = getLLVMType(structSymContainer->getType());
        assert(t->getPointerTo() == F->getArg(0)->getType());
            
        // We bitcast the "this" ptr in case we're calling a parent's method. If not, bitcasting will 
        // simply return the original "this" ptr, unchanged. Alternatively, we could just bitcast it 
        // to F->getArg(0)->getType(), but finding the actual struct type is safer, in case we made a mistake
        Value* currentMethodThisCasted = Builder->CreateBitOrPointerCast(currentMethodThis, t->getPointerTo(), "memCast");
        args.push_back(currentMethodThisCasted);
    }
    else {
        F = TheModule->getFunction(id);
    }

    // NOTE: argList() is null is no children, so we use funcSym->getParams().size() instead
    for (int i = 0; i < funcSym->getParams().size(); i++) {
        Value* val = values.at(ctx->argList()->expr(i));
        args.push_back(val);
    }

    string twine = (F->getReturnType() == LLVMType::getVoidTy(*TheContext)) ? "" : "calltmp"; // if the function ret type is void
    // the function call must not have a name (other the return value doesn't exist). Otherwise, Builder->CreateCall(..) will trigger an assertion

    // methodPtr is used to handle vtable calls
	Value* funcRetVal = methodPtr ? Builder->CreateCall(dyn_cast<FunctionType>(F->getType()->getPointerElementType()), methodPtr, args, twine)
								  : Builder->CreateCall(F, args, twine);

    // We want to check whether the return value of the function call needs to be promoted
    funcRetVal = createCastIfNecessary(ctx, funcRetVal);
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

    // if the left val corresponds to an elementPtr (either we're in a chained external member access or the root
    // which is always a primaryId, refers to an internal member (so we're inside a field varDecl. Then use that 
    // as the address
   if (elementPtrs.count(ctx->expr()) == 1)
        address = elementPtrs.at(ctx->expr());
    else {
        // left sym is a primaryId (the root, eg, the 'a' in 'a.b.c')
        // Set address to the globalVar or local address of the root
		address = resolvedSymAndAddrs.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(address) == false) 
			assert(false);
    }

    // This is the StructSym corresponding to the left side. eg) `DerDer derder; derder.x;` Note that the 'x' may actually
    // be inside a base class. As a result, we may need to convert the address of derder to Base 
    // to correctly retrieve the addr of 'x'
    shared_ptr<StructSymbol> structSymDer = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(leftSym->getType()));
    assert(structSymDer != nullptr);

    string memName = ctx->ID()->getText();
	shared_ptr<Symbol> resMemSym = symbolTable.resolveMember(memName, structSymDer);

    // The StructSymbol directly containing the resolved member (could be a base or a derived structSym)
    shared_ptr<StructSymbol> structSymContainer = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(resMemSym));
    LLVMType* t = getLLVMType(structSymContainer->getType());
    Value* addressCasted = Builder->CreateBitOrPointerCast(address, t->getPointerTo(), "memCast"); // if not a derived type, this will generate no new IR, simply returns address

	GetElementPtrInst* memPtr = createStructElementAddr(structSymContainer, memName, addressCasted);
    elementPtrs.emplace(ctx, memPtr);

    // TODO: Refactor. To allow this member access to be used, we load it. However, this means for each chained member access
    // we load intermediate members, whose values would never actually be used, which is unnecessary redundancy
    // eg) for "a.b.c";, load will be created for "b" and "c" even though the load for "b" isn't needed (actually, primaryId "a' 
    // will also trigger a load
	Value* val = Builder->CreateLoad(memPtr, "loadtmp_" + memName);
    val = createCastIfNecessary(ctx, val);

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
		leftAddr = resolvedSymAndAddrs.at(leftSym);
		if (isa<AllocaInst, GlobalVariable>(leftAddr) == false)
			assert(false);
    }

	// The left address may refer to a derived type but the method actually comes from a base type. As a result, we will have to 
    // cast the left address to whatever pointer type the method's first arg is
	shared_ptr<StructSymbol> structSymContainer = getContainingStruct(funcSym, symbolTable);
	LLVMType* t = getLLVMType(structSymContainer->getType());
	assert(t->getPointerTo() == F->getArg(0)->getType());
	Value* leftAddrCasted = Builder->CreateBitOrPointerCast(leftAddr, t->getPointerTo(), "memCast");

    Value* methodPtr = nullptr;
    if (funcSym->isVirtual() || funcSym->isOverride()) { // if it contains the 'virtual' or 'override' keyword, then use vptr
        LLVMType* funcTypePtr = F->getType();
        Value* triplePtr = Builder->CreateBitOrPointerCast(leftAddrCasted, funcTypePtr->getPointerTo()->getPointerTo(), "memCast");
        Value* vptrLoaded = Builder->CreateLoad(triplePtr, "vptrLoaded");

        int memberIndex = getVTableMethodIndex(structSymContainer, funcSym->getName());
        Value* index = Builder->getInt64(memberIndex);
        Value* methodPtrPtr = Builder->CreateGEP(funcTypePtr, vptrLoaded, index, "vtableMethodPtr");

        methodPtr = Builder->CreateLoad(methodPtrPtr, "vtableMethod");
        assert(isa<FunctionType>(funcTypePtr->getPointerElementType()));
    }

    vector<Value*> args;
    args.push_back(leftAddrCasted);
	for (int i = 0; i < funcSym->getParams().size(); i++) {
		Value* val = values.at(ctx->argList()->expr(i));
		args.push_back(val);
	}

    string twine = (F->getReturnType() == LLVMType::getVoidTy(*TheContext)) ? "" : "calltmp"; // if the function ret type is void

	Value* funcRetVal = methodPtr ? Builder->CreateCall(dyn_cast<FunctionType>(F->getType()->getPointerElementType()), methodPtr, args, twine)
								  : Builder->CreateCall(F, args, twine);

    funcRetVal = createCastIfNecessary(ctx, funcRetVal);

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


Value* CodeGeneration::createCastIfNecessary(bluefinParser::ExprContext* ctx, llvm::Value* val) const
{
    Type exprEvalType = typeContexts.at(ctx).getEvalType();
    Type exprPromoType = typeContexts.at(ctx).getPromotionType();

    if (exprPromoType == Type::FLOAT() && exprEvalType == Type::INT()) { 
        val = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, val, LLVMType::getFloatTy(*TheContext), "casttmp");
    }
    return val;
}

// Creates a "Function" for the struct's method, which differs from a normal function because 
// it adds name mangling and a "this" ptr
Function* CodeGeneration::createFunction(shared_ptr<FunctionSymbol> funcSym, StructType* structType) const
{
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
    Function* F = Function::Create(FT, Function::ExternalLinkage, "_" + structType->getName() + "_" + funcSym->getName(), TheModule.get());

    // set the name of the first arg to 'this' and provide names to others
    F->getArg(0)->setName("this");
    for (int i = 0; i < params.size(); i++) {
        F->getArg(i + 1)->setName(params[i]->getName());
    }

    return F;
}

Function* CodeGeneration::createFunction(shared_ptr<FunctionSymbol> funcSym) const
{
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
    Function* F = Function::Create(FT, Function::ExternalLinkage, funcSym->getName(), TheModule.get());;

	for (int i = 0; i < params.size(); i++) {
		F->getArg(i)->setName(params[i]->getName());
	}

    return F;
}

Function* CodeGeneration::createCtor(StructType* structType) const
{
    vector<LLVMType*> ctorParam; // since this is the ctor, the type should be only
    ctorParam.push_back(structType->getPointerTo());

    FunctionType* FT = FunctionType::get(getLLVMType(Type::VOID()), ctorParam, false);
    Function* ctor = Function::Create(FT, Function::ExternalLinkage, "_ctor_" + structType->getName(), TheModule.get());
    ctor->getArg(0)->setName("this");
    return ctor;
}

Function* CodeGeneration::createGlobalVarDeclFunction(string id) const
{
    string funcName = "internalGlobalVarDecl_" + id;
    vector<LLVMType*> noParams;
    FunctionType* FT = FunctionType::get(LLVMType::getVoidTy(*TheContext), noParams, false);
    Function* internalF = Function::Create(FT, Function::InternalLinkage, funcName, TheModule.get());

    return internalF;
}

// If the struct extends a base type, we treat the base type as if it were the first member in the StructSym
StructType* CodeGeneration::createStruct(shared_ptr<StructSymbol> structSym) const
{
    vector<LLVMType*> members;
    if (shouldLLVMStructTypeContainExplicitVPtr(structSym)) {
		LLVMType* fPtr = FunctionType::get(LLVMType::getInt32Ty(*TheContext), true);
		members.push_back(fPtr->getPointerTo()->getPointerTo());
    }

    if (shared_ptr<StructSymbol> parentSym = structSym->getSuperClass()) {
        members.push_back(getLLVMType(parentSym->getType()));
    }
    for (auto varSym : structSym->getFields()) {
        members.push_back(getLLVMType(varSym->getType()));
    }

    StructType* structType = StructType::create(*TheContext, members, structSym->getName());
    return structType;
}

GetElementPtrInst* CodeGeneration::createStructElementAddr(shared_ptr<StructSymbol> structSym, string fieldName, Value* structAddr)
{
    LLVMType* structType = getLLVMType(structSym->getType());

    size_t memberIndex = structSym->getFieldIndex(fieldName);

    if (shouldLLVMStructTypeContainExplicitVPtr(structSym))
        memberIndex++;

    // If this struct extends a parent struct, then in the generated IR code for this struct, the first member is the parent type,
    // which means we must increment the memberIndex by 1
    if (structSym->getSuperClass())
        memberIndex++;

    Value* zero = Builder->getInt32(0);
    Value* index = Builder->getInt32(memberIndex);
    Value* indices[2] = { zero, index };

    Value* memAddr = Builder->CreateGEP(structType, structAddr, indices, "memPtr_" + fieldName);
    assert(isa<GetElementPtrInst>(memAddr));
    GetElementPtrInst* castedMemAddr = dyn_cast<GetElementPtrInst>(memAddr);
    return castedMemAddr;
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

