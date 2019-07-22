#include "Declaration.h"
#include <iostream>

using namespace bluefin;
using std::string;
using std::dynamic_pointer_cast;


void Declaration::enterVarDecl(bluefinParser::VarDeclContext* ctx) {
	const string typeName = ctx->type()->getText();
	shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
	// assert(typeSymbol != nullptr);
	// right now, if this fails, we don't want to stop execution

	// However, we will skip the variable declaration if can't resolve its type
	if (typeSymbol) {
		string varName = ctx->ID()->getText();
		shared_ptr<Symbol> varSym =  symbolFactory.createVariableSymbol(varName, typeSymbol->getType(), ctx->getStart()->getTokenIndex());

		symbolTable.declare(varSym);
		scopes.emplace(ctx, symbolTable.getCurrScope());
	}
}

void Declaration::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	shared_ptr<Symbol> retTypeSymbol = symbolTable.resolve(retTypeName);
	// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

	if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		shared_ptr<Symbol> funcSym = symbolFactory.createFunctionSymbol(funcName, retTypeSymbol->getType(), ctx->getStart()->getTokenIndex());

		symbolTable.declare(funcSym);
		functionSymbol = dynamic_pointer_cast<FunctionSymbol>(funcSym);
		scopes.emplace(ctx, symbolTable.getCurrScope());
	}

	symbolTable.enterScope("function " + ctx->ID()->getText());
}

void Declaration::exitFuncDef(bluefinParser::FuncDefContext* ctx)
{
	symbolTable.exitScope();
	functionSymbol = nullptr;
}

void Declaration::enterParam(bluefinParser::ParamContext* ctx) {
		// almost identical to enterVarDecl(..)
		const string retTypeName = ctx->type()->getText();
		shared_ptr<Symbol> retTypeSymbol = symbolTable.resolve(retTypeName);
		// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

		if (retTypeSymbol) {
			string funcName = ctx->ID()->getText();
			shared_ptr<Symbol> paramSym = symbolFactory.createVariableSymbol(funcName, retTypeSymbol->getType(), ctx->getStart()->getTokenIndex());

			symbolTable.declare(paramSym);
			functionSymbol->attachParam(paramSym);
		}
}

void Declaration::enterStructDef(bluefinParser::StructDefContext* ctx) {
	const string structName = ctx->ID()->getText();

	shared_ptr<StructSymbol> superClassSym;

	if (ctx->superClass()) {
		superClassSym = dynamic_pointer_cast<StructSymbol>(symbolTable.resolve(ctx->superClass()->ID()->getText()));
	}

	shared_ptr<Symbol> structSym = symbolFactory.createStructSymbol(structName, symbolTable.getCurrScope(), superClassSym);

	symbolTable.declare(structSym);
	symbolTable.setCurrentScope(dynamic_pointer_cast<Scope>(structSym));
}

void Declaration::exitStructDef(bluefinParser::StructDefContext* ctx)
{
	symbolTable.exitScope();
}

void Declaration::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	scopes.emplace(ctx, symbolTable.getCurrScope());
}

/*
void Declaration::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {
	shared_ptr<StructSymbol> s = std::static_pointer_cast<StructSymbol>(scopes.at(ctx->expr())); // ctx->expr() is either a primaryId or another MA, which already had their scopes stored
	shared_ptr<Symbol> resMemSym = s->resolveMember(ctx->ID()->getText());

	if (shared_ptr<StructSymbol> memberStructType = dynamic_pointer_cast<StructSymbol>(resMemSym->getType())) {
		scopes.emplace(ctx, memberStructType);
	}
}
*/


void Declaration::enterFuncCall(bluefinParser::FuncCallContext* ctx)
{
	scopes.emplace(ctx, symbolTable.getCurrScope()); // will be used in later pass to resolve this ctx name again
}

void Declaration::enterBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.enterScope();
}

void Declaration::exitBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.exitScope();
}
