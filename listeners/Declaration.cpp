#include "Declaration.h"
#include <iostream>
#include <vector>

using namespace bluefin;
using std::string;
using std::dynamic_pointer_cast;
using std::vector;

void Declaration::enterVarDecl(bluefinParser::VarDeclContext* ctx) {
	const string typeName = ctx->type()->getText();
	//shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
	// assert(typeSymbol != nullptr);
	// right now, if this fails, we don't want to stop execution

	// However, we will skip the variable declaration if can't resolve its type
	//if (typeSymbol) {
		string varName = ctx->ID()->getText();
		shared_ptr<Symbol> varSym = symbolFactory.createVariableSymbol(varName, Type{ typeName }, ctx->getStart()->getTokenIndex());

		symbolTable.declare(varSym);
		scopes.emplace(ctx, symbolTable.getCurrScope());
	//}
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
		currFunctionSym = dynamic_pointer_cast<FunctionSymbol>(funcSym);
		scopes.emplace(ctx, symbolTable.getCurrScope());
	}

	symbolTable.enterScope("function " + ctx->ID()->getText());
}

void Declaration::exitFuncDef(bluefinParser::FuncDefContext* ctx)
{
	/* Check override specifier
	- name, return type, and param types must match with superclasses
	- can only occur in methods, not in functions
	- Also check if override specifier doesn't exist but is supposed to
	*/

	if (ctx->Override()) {
		if (!currStructSym) {
			errCollector.err(ErrorCollector::OVERRIDE_NO_FUNCTION);
			goto finish;
		}

		shared_ptr<StructSymbol> superClass = currStructSym->getSuperClass();
		if (!superClass) {
			errCollector.err(ErrorCollector::OVERRIDE_MISSING_SUPERCLASS);
			goto finish;
		}

		shared_ptr<Symbol> resolved = superClass->resolveMember(ctx->ID()->getText());
		if (!resolved)  {
			errCollector.err(ErrorCollector::OVERRIDE_UNRESOLVED_NAME);
			goto finish;
		}

		shared_ptr<FunctionSymbol> resolvedFunc = dynamic_pointer_cast<FunctionSymbol>(resolved);
		if (!resolvedFunc) {
			errCollector.err(ErrorCollector::OVERRIDE_RESOLVED_NAME_BUT_NOT_METHOD);
			goto finish;
		}

		if (resolvedFunc->getType() != currFunctionSym->getType()) {
			errCollector.err(ErrorCollector::OVERRIDE_UNMATCHED_RETURN_TYPE);
			goto finish;
		}
		
		if (resolvedFunc->getParams().size() != currFunctionSym->getParams().size()) {
			errCollector.err(ErrorCollector::OVERRIDE_UNMATCHED_PARAMETER_TYPES);
			goto finish;
		}

		vector<shared_ptr<Symbol>> resolvedParams = resolvedFunc->getParams();
		vector<shared_ptr<Symbol>> currParams = currFunctionSym->getParams();
		assert(resolvedParams.size() == currParams.size());

		for (int i = 0; i < resolvedParams.size(); i++) {
			if (resolvedParams[i]->getType() != currParams[i]->getType()) {
				errCollector.err(ErrorCollector::OVERRIDE_UNMATCHED_PARAMETER_TYPES);
				goto finish;
			}
		}

		errCollector.success(ErrorCollector::OVERRIDE_SUCCESSFUL);

	}
	// this gets ugly
	else {
		if (currStructSym && currStructSym->getSuperClass()) {
			shared_ptr<Symbol> resolved = currStructSym->getSuperClass()->resolveMember(ctx->ID()->getText());
			shared_ptr<FunctionSymbol> resolvedFunc = dynamic_pointer_cast<FunctionSymbol>(resolved);
			if (resolvedFunc &&
				resolvedFunc->getType() == currFunctionSym->getType()) {

				vector<shared_ptr<Symbol>> resolvedParams = resolvedFunc->getParams();
				vector<shared_ptr<Symbol>> currParams = currFunctionSym->getParams();

				if (resolvedParams.size() == currParams.size()) {
					bool sameParamTypes = true;
					for (int i = 0; i < resolvedParams.size(); i++) {
						if (resolvedParams[i]->getType() != currParams[i]->getType()) {
							sameParamTypes = false;
						}
					}
					if (sameParamTypes) {
						errCollector.err(ErrorCollector::OVERRIDE_SPECIFIER_NEEDED);
					}
				}
			}

		}
	}

	finish:
	symbolTable.exitScope();
	currFunctionSym = nullptr;
}

void Declaration::enterParam(bluefinParser::ParamContext* ctx) {
		// almost identical to enterVarDecl(..)
		const string retTypeName = ctx->type()->getText();
		//shared_ptr<Symbol> retTypeSymbol = symbolTable.resolve(retTypeName);
		// assert(retTypeSymbol != nullptr); if this fails, we don't want to stop execution just right now

		//if (retTypeSymbol) {
		string funcName = ctx->ID()->getText();
		shared_ptr<Symbol> paramSym = symbolFactory.createVariableSymbol(funcName, Type{ retTypeName }, ctx->getStart()->getTokenIndex());

		symbolTable.declare(paramSym);
		currFunctionSym->attachParam(paramSym);
		//}
}

void Declaration::enterStructDef(bluefinParser::StructDefContext* ctx) {
	const string structName = ctx->ID()->getText();

	shared_ptr<StructSymbol> superClassSym;

	if (ctx->superClass()) {
		superClassSym = dynamic_pointer_cast<StructSymbol>(symbolTable.resolve(ctx->superClass()->ID()->getText()));
	}

	shared_ptr<Symbol> structSym = symbolFactory.createStructSymbol(structName, symbolTable.getCurrScope(), ctx->getStart()->getTokenIndex(), superClassSym);

	symbolTable.declare(structSym);
	symbolTable.setCurrentScope(dynamic_pointer_cast<Scope>(structSym));
	currStructSym = dynamic_pointer_cast<StructSymbol>(structSym);
}

void Declaration::exitStructDef(bluefinParser::StructDefContext* ctx)
{
	symbolTable.exitScope();
	currStructSym = nullptr;
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
