#include "Declaration.h"
#include "../symbolTable/Exceptions.h"
#include <iostream>
#include <vector>


using namespace bluefin;
using std::string;
using std::dynamic_pointer_cast;
using std::vector;

void Declaration::enterVarDecl(bluefinParser::VarDeclContext* ctx) {
	const string typeName = ctx->type()->getText();
	string varName = ctx->ID()->getText();
	shared_ptr<Symbol> varSym = symbolFactory.createVariableSymbol(varName, Type{ typeName }, ctx->getStart()->getTokenIndex());
	
	try {
		symbolTable.declare(varSym, ctx);
		broadcastEvent(SuccessEvent::DECLARED_SYMBOL, varSym);
	}
	catch (RedeclarationException e) {
		broadcastEvent(ErrorEvent::REDECLARED_EXISTING_SYMBOL, varName);
	}
}

void Declaration::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
	// almost identical to enterVarDecl(..)
	const string retTypeName = ctx->type()->getText();
	string funcName = ctx->ID()->getText();
	shared_ptr<Symbol> funcSym = symbolFactory.createFunctionSymbol(funcName, Type{ retTypeName }, ctx->getStart()->getTokenIndex());

	try {
		symbolTable.declare(funcSym, ctx);
		broadcastEvent(SuccessEvent::DECLARED_SYMBOL, funcSym);
	}
	catch (RedeclarationException e) {
		broadcastEvent(ErrorEvent::REDECLARED_EXISTING_SYMBOL, funcName);
	}
	currFunctionSym = dynamic_pointer_cast<FunctionSymbol>(funcSym);

	symbolTable.enterScope("function " + ctx->ID()->getText());
	broadcastEvent(ScopeEvent::ENTERING_SCOPE);
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

		shared_ptr<Symbol> resolved = superClass->resolve(ctx->ID()->getText());
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
			string memberToSearchFor = ctx->ID()->getText();
			shared_ptr<Symbol> resolved = currStructSym->getSuperClass()->resolve(memberToSearchFor);
			if (resolved) {
				broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resolved, currStructSym->getSuperClass());

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
			else {
				broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, memberToSearchFor, currStructSym->getSuperClass());
			}

		}
	}

	finish:
	symbolTable.exitScope();
	broadcastEvent(ScopeEvent::EXITING_SCOPE);
	currFunctionSym = nullptr;
}

void Declaration::enterParam(bluefinParser::ParamContext* ctx) {
	// almost identical to enterVarDecl(..)
	const string typeName = ctx->type()->getText();
	string varName = ctx->ID()->getText();
	shared_ptr<Symbol> paramSym = symbolFactory.createVariableSymbol(varName, Type{ typeName }, ctx->getStart()->getTokenIndex());

	try {
		symbolTable.declare(paramSym, ctx);
		broadcastEvent(SuccessEvent::DECLARED_SYMBOL, paramSym);
	}
	catch (RedeclarationException e) {
		broadcastEvent(ErrorEvent::REDECLARED_EXISTING_SYMBOL, varName);
	}
	currFunctionSym->attachParam(paramSym);
}

void Declaration::enterStructDef(bluefinParser::StructDefContext* ctx) {
	const string structName = ctx->ID()->getText();
	shared_ptr<StructSymbol> superClassSym;

	if (ctx->superClass()) {
		superClassSym = dynamic_pointer_cast<StructSymbol>(symbolTable.resolve(ctx->superClass()->ID()->getText(), symbolTable.getCurrScope()));
		if (superClassSym) {
			broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, superClassSym);
		}
	}

	shared_ptr<Symbol> structSym = symbolFactory.createStructSymbol(structName, symbolTable.getCurrScope(), ctx->getStart()->getTokenIndex(), superClassSym);
	try {
		symbolTable.declare(structSym, ctx);
	broadcastEvent(SuccessEvent::DECLARED_SYMBOL, structSym);
	}
	catch (RedeclarationException e) {
		broadcastEvent(ErrorEvent::REDECLARED_EXISTING_SYMBOL, structName);
	}

	symbolTable.setCurrentScope(dynamic_pointer_cast<Scope>(structSym));
	broadcastEvent(ScopeEvent::SETTING_CURRENT_SCOPE);
	currStructSym = dynamic_pointer_cast<StructSymbol>(structSym);
}

void Declaration::exitStructDef(bluefinParser::StructDefContext* ctx)
{
	symbolTable.exitScope();
	broadcastEvent(ScopeEvent::EXITING_SCOPE);
	currStructSym = nullptr;
}

void Declaration::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	symbolTable.saveParseTreeWithCurrentScope(ctx);
}

void Declaration::enterFuncCall(bluefinParser::FuncCallContext* ctx)
{
	symbolTable.saveParseTreeWithCurrentScope(ctx);
}

void Declaration::enterBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.enterScope();
	broadcastEvent(ScopeEvent::ENTERING_SCOPE);
}

void Declaration::exitBlock(bluefinParser::BlockContext* ctx)
{
	symbolTable.exitScope();
	broadcastEvent(ScopeEvent::EXITING_SCOPE);
}


void Declaration::broadcastEvent(ScopeEvent e)
{
	for (shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e);
	}
}

void Declaration::broadcastEvent(SuccessEvent e, shared_ptr<Symbol> sym, shared_ptr<StructSymbol> structSym)
{
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, sym, structSym);
	}
}

void Declaration::broadcastEvent(ErrorEvent e, string symName, shared_ptr<StructSymbol> structSym) {
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, symName, structSym);
	}
}

void Declaration::attachEventObserver(shared_ptr<EventObserver> observer)
{
	eventObservers.push_back(observer);
}

void Declaration::detachEventObserver(shared_ptr<EventObserver> observer)
{
	auto it = std::find(eventObservers.begin(), eventObservers.end(), observer);
	if (it != eventObservers.end()) {
		eventObservers.erase(it); 
	}
}
