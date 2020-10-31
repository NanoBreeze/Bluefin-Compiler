#include "CodeGeneration.h"
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

void CodeGeneration::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
    shared_ptr<FunctionSymbol> funcSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getSymbol(ctx));
    assert(funcSym != nullptr);

    Type retType = funcSym->getType();
    assert(retType.isUserDefinedType() == false && retType != Type::STRING()); // for now, only allow some types to be generated

    vector<LLVMType*> paramTypes;
    vector<shared_ptr<Symbol>> params = funcSym->getParams();
    for (const auto param : params) {
        Type paramType = param->getType();
		assert(paramType.isUserDefinedType() == false && paramType != Type::STRING() && paramType != Type::VOID()); // for now, only allow some types to be generated
        if (paramType == Type::BOOL())
            paramTypes.push_back(LLVMType::getInt1Ty(*TheContext));
        else if (paramType == Type::FLOAT())
            paramTypes.push_back(LLVMType::getFloatTy(*TheContext));
        else if (paramType == Type::INT())
            paramTypes.push_back(LLVMType::getInt32Ty(*TheContext));
    }

    FunctionType* FT = nullptr;
    if (retType == Type::BOOL()) {
        FT = FunctionType::get(LLVMType::getInt1Ty(*TheContext), paramTypes, false);
    }
    else if (retType == Type::FLOAT()) {
        FT = FunctionType::get(LLVMType::getFloatTy(*TheContext), paramTypes, false);
    }
    else if (retType == Type::INT()) {
        FT = FunctionType::get(LLVMType::getInt32Ty(*TheContext), paramTypes, false);
    }
    else if (retType == Type::VOID()) {
        FT = FunctionType::get(LLVMType::getVoidTy(*TheContext), paramTypes, false);
    }

    Function* F = Function::Create(FT, Function::ExternalLinkage, funcSym->getName(), TheModule.get());

    // Add the values to resolvedSymsAndValues, and set the param names for LLVM's code generation (to avoid awkward names like %0, %1)
    for (int i = 0; i < params.size(); i++) {
        resolvedSymAndValues.emplace(params[i], F->getArg(i));
        F->getArg(i)->setName(params[i]->getName());
    }

    BasicBlock* BB = BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(BB);

    //Builder->CreateRetVoid();

    //bool isErr = verifyFunction(*F);
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
    * This means we must look not only at the existing type, but also that specific TypeContext's promotion type and cast it
    */
	string varName = ctx->ID()->getText();
	shared_ptr<Symbol> resolvedSym = symbolTable.getResolvedSymbol(ctx);
    Value* val = resolvedSymAndValues.at(resolvedSym);
    if (typeContexts.at(ctx).getPromotionType() == Type::FLOAT())
		val = Builder->CreateCast(llvm::Instruction::CastOps::SIToFP, val, LLVMType::getFloatTy(*TheContext), "casttmp");

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

void CodeGeneration::exitFuncCall(bluefinParser::FuncCallContext* ctx)
{
    string id = ctx->ID()->getText();
    Function* func = TheModule->getFunction(id);
    assert(func != nullptr); // Since we do code generation after all the other passes, we can be pretty sure the defintion 
    // had arleady appeared and FuncDef put it into the module already

    vector<Value*> args;
    for (int i = 0; i < func->arg_size(); i++) {
        Value* val = values.at(ctx->argList()->expr(i));
        args.push_back(val);
    }

    string twine = (func->getReturnType() == LLVMType::getVoidTy(*TheContext)) ? "" : "calltmp"; // if the function ret type is void
    // the function call must not have a name (other the return value doesn't exist). Otherwise, Builder->CreateCall(..) will trigger an assertion

    Value* funcRetVal = Builder->CreateCall(func, args, twine);
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
    string str;
    //raw_ostream stream;
    raw_fd_ostream& stream = llvm::outs();

    bool isBroken = verifyModule(*TheModule, &stream);
    stream.flush();
    return !isBroken;
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
