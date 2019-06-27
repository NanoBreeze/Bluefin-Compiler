#include "PopulateSymTabListener.h"

#include "antlr4-runtime.h"
#include <iostream>
#include <memory>
#include "../symbolTable/Symbol.h"
#include "../symbolTable/VariableSymbol.h"
#include "../symbolTable/StructSymbol.h"
#include "../symbolTable/FunctionSymbol.h"
#include <assert.h>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;

void PopulateSymTabListener::enterVarDecl(bluefinParser::VarDeclContext* ctx)  
{
	const string typeName = ctx->type()->getText();
	shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
	// assert(typeSymbol != nullptr);
	// right now, if this fails, we don't want to stop execution

	// However, we will skip the variable declaration if can't resolve its type
	if (typeSymbol) {
		string varName = ctx->ID()->getText();
		shared_ptr<Symbol> varSym =  symbolFactory.createVariableSymbol(varName, typeSymbol->getType());

		symbolTable.declare(varSym);
		scopes.emplace(ctx, symbolTable.getCurrScope());
	}
}

void PopulateSymTabListener::enterFuncDef(bluefinParser::FuncDefContext* ctx)
{
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	shared_ptr<Symbol> retTypeSymbol = symbolTable.resolve(retTypeName);
	// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

	if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		shared_ptr<Symbol> funcSym = symbolFactory.createFunctionSymbol(funcName, retTypeSymbol->getType());

		symbolTable.declare(funcSym);
	}

	symbolTable.enterScope("function " + ctx->ID()->getText());
}

void PopulateSymTabListener::enterParam(bluefinParser::ParamContext* ctx)
{
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	shared_ptr<Symbol> retTypeSymbol = symbolTable.resolve(retTypeName);
	// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

	if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		shared_ptr<Symbol> funcSym = symbolFactory.createVariableSymbol(funcName, retTypeSymbol->getType());

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
	shared_ptr<Symbol> structSym = symbolFactory.createStructSymbol(structName);
	symbolTable.declare(structSym);

	symbolTable.enterScope(); // since structs don't contain "block" elements
}

void PopulateSymTabListener::exitStructDef(bluefinParser::StructDefContext* ctx)
{
	symbolTable.exitScope();
}

void PopulateSymTabListener::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx)
{
	shared_ptr<Symbol> resolvedSym = symbolTable.resolve(ctx->ID()->getText());

	if (resolvedSym) { // if not resolved, then this is not resolved. Uh oh!
		scopes.emplace(ctx, symbolTable.getCurrScope()); // will be used in later pass to resolve this ctx name again
		if (shared_ptr<StructSymbol> s = dynamic_pointer_cast<StructSymbol>(resolvedSym->getType())) {
			structSymbolStack.push(s); // this Id has the type of a structSymbol. eg, A a; a.b; "a" in "a.b" is the primaryId
		}
	}
}

void PopulateSymTabListener::exitMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
	if (!structSymbolStack.empty()) { 
		shared_ptr<StructSymbol> s = structSymbolStack.top();
		structSymbolStack.pop();

		shared_ptr<Symbol> resMemSym = s->resolveMember(ctx->ID()->getText());
		if (resMemSym) { // if not resolved, no need to check its type
			if (shared_ptr<StructSymbol> resStruct = dynamic_pointer_cast<StructSymbol>(resMemSym->getType())) {
				structSymbolStack.push(resStruct); // if the resolved member is a struct, it may be used later
		}

		}
	}

	// Empty stack is possible if the struct symbol were not resolved, in enterPrimaryId
	// eg, a.b, if "a" doesn't exist, it wouldn't have been pushed onto the stack
	// so we can't access it
	// TODO: Currently fails silent, make unsilent failure, expect to find a struct to be on stack
}


void PopulateSymTabListener::enterBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.enterScope();
}

void PopulateSymTabListener::exitBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.exitScope();
}

