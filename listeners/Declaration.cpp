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
	bool isVirtual = (ctx->Virtual() != nullptr); 
	bool isOverride = (ctx->Override() != nullptr); 
	// if either contains 'virtual' or 'override' specifiers, then the method belongs in the vtable
	shared_ptr<Symbol> funcSym = symbolFactory.createFunctionSymbol(funcName, Type{ retTypeName }, ctx->getStart()->getTokenIndex(), isVirtual, isOverride);

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
	- superclass must exist
	- name, return type, and param types must match superclass' (or its superclass') corresponding virtual method
	- can only occur in methods, not in functions
	- Also check if override specifier doesn't exist but is supposed to
	*/

	string funcName = ctx->ID()->getText();
	// A method can't contain 'virtual' and 'override'
	if (ctx->Virtual()) {
		if (!currStructSym) 
			broadcastEvent(PolymorphismErrorEvent::FUNCTION_CANNOT_HAVE_VIRTUAL_SPECIFIER, funcName);
	}
	else if (ctx->Override()) {
		if (!currStructSym)
			broadcastEvent(PolymorphismErrorEvent::FUNCTION_CANNOT_HAVE_OVERRIDE_SPECIFIER, funcName);
		else if (!currStructSym->getSuperClass())
			broadcastEvent(PolymorphismErrorEvent::NO_SUPERCLASS, funcName);
		else {
			// Note, find virtual method in parent chain
			shared_ptr<FunctionSymbol> resolvedFunc = nullptr;
			bool foundVirtMethod = symbolTable.findCorrespondingVirtualMethod(currFunctionSym, currStructSym);
			if (!foundVirtMethod)
				broadcastEvent(PolymorphismErrorEvent::CORRESPONDING_VIRTUAL_METHOD_NOT_FOUND, funcName);
		}
	}
	else { 
		if (currStructSym && currStructSym->getSuperClass()) {
			// the method doesn't contain the 'override' keyword. Check if it's supposed to (corresponding virtual method exists)
			bool foundVirtMethod = symbolTable.findCorrespondingVirtualMethod(currFunctionSym, currStructSym);
			if (foundVirtMethod)
				broadcastEvent(PolymorphismErrorEvent::MISSING_OVERRIDE_SPECIFIER, funcName);
		}
	}

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

void Declaration::enterMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
	symbolTable.saveParseTree(ctx);
}

void Declaration::enterMethodCall(bluefinParser::MethodCallContext* ctx)
{
	symbolTable.saveParseTree(ctx);
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

void Declaration::broadcastEvent(PolymorphismErrorEvent e, string funcName)
{
	for (shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, funcName);
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
