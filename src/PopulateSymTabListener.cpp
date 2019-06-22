#include "PopulateSymTabListener.h"

#include <iostream>
#include "../symbolTable/Symbol.h"
#include "../symbolTable/VariableSymbol.h"
#include "../symbolTable/StructSymbol.h"
#include "../symbolTable/FunctionSymbol.h"
#include <assert.h>

using namespace bluefin;

void PopulateSymTabListener::enterVarDecl(bluefinParser::VarDeclContext* ctx)  
{
	const string typeName = ctx->type()->getText();
	Symbol* typeSymbol = symbolTable.resolve(typeName);
	// assert(typeSymbol != nullptr);
	// right now, if this fails, we don't want to stop execution

	// However, we will skip the variable declaration if can't resolve its type
	if (typeSymbol) {
		string varName = ctx->ID()->getText();
		Symbol* varSym = new VariableSymbol(varName, typeSymbol->getType());

		symbolTable.declare(varSym);
	}
}

void PopulateSymTabListener::enterFuncDef(bluefinParser::FuncDefContext* ctx)
{
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	Symbol* retTypeSymbol = symbolTable.resolve(retTypeName);
	// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

	if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		Symbol* funcSym = new FunctionSymbol(funcName, retTypeSymbol->getType());

		symbolTable.declare(funcSym);
	}

	symbolTable.enterScope();
}

void PopulateSymTabListener::enterParam(bluefinParser::ParamContext* ctx)
{
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	Symbol* retTypeSymbol = symbolTable.resolve(retTypeName);
	// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

	if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		Symbol* funcSym = new VariableSymbol(funcName, retTypeSymbol->getType());

		symbolTable.declare(funcSym);
	}
}

void PopulateSymTabListener::exitFuncDef(bluefinParser::FuncDefContext* ctx)
{
	symbolTable.exitScope();
}

void PopulateSymTabListener::enterStructDef(bluefinParser::StructDefContext* ctx)
{
	const string structName = ctx->ID()->getText();
	Symbol* structSym = new StructSymbol(structName);
	symbolTable.declare(structSym);

	symbolTable.enterScope(); // since structs don't contain "block" elements
}

void PopulateSymTabListener::exitStructDef(bluefinParser::StructDefContext* ctx)
{
	symbolTable.exitScope();
}

void PopulateSymTabListener::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx)
{
	symbolTable.resolve(ctx->ID()->getText());
}

void PopulateSymTabListener::enterBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.enterScope();
}

void PopulateSymTabListener::exitBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.exitScope();
}
