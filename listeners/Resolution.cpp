#include "Resolution.h"

#include "../symbolTable/Exceptions.h"

using namespace bluefin;
using std::dynamic_pointer_cast;

// Eg) int a = 5; Here, we want to resolve "int". The type can be either a user-defined type or built-in type
// In the past, such resolution was done in Declaration pass, but now we move it into Resolution pass
void Resolution::enterVarDecl(bluefinParser::VarDeclContext* ctx) {
	const string typeName = ctx->type()->getText();
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
		assert(typeSymbol != nullptr);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, typeSymbol);

		if (ctx->type()->getStart()->getTokenIndex() < typeSymbol->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, typeName);
		}
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, typeName);
	}
}

void Resolution::enterParam(bluefinParser::ParamContext* ctx) {
	const string typeName = ctx->type()->getText();
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
		assert(typeSymbol != nullptr);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, typeSymbol);
		
		if (ctx->type()->getStart()->getTokenIndex() < typeSymbol->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, typeName);
		}
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, typeName);
	}
}

void Resolution::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
	const string typeName = ctx->type()->getText();
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName);
		assert(typeSymbol != nullptr);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, typeSymbol);
		
		if (ctx->type()->getStart()->getTokenIndex() < typeSymbol->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, typeName);
		}
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, typeName);
	}
}

void Resolution::enterPrimaryId(bluefinParser::PrimaryIdContext* ctx)
{
	if (scopes.find(ctx) == scopes.end()) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, ctx->ID()->getText());
	}
	else {
		pair<shared_ptr<Symbol>, shared_ptr<Scope>> resolvedSymAndScope = resolve(ctx->ID()->getText(), scopes.at(ctx));
		shared_ptr<Symbol> resolvedSym = resolvedSymAndScope.first;
		shared_ptr<Scope> scope = resolvedSymAndScope.second;


		if (resolvedSym) { // TODO: if not resolved, not good!

			// remove illegal forward referencing. Allow forward referencing only if the resolved symbol is also inside the struct. 
			// Otherwise, compare ctx position with resolved sym's location
			if (!dynamic_pointer_cast<StructSymbol>(scope) && ctx->getStart()->getTokenIndex() < resolvedSym->getTokenIndex()) {
				broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, resolvedSym->getName());

				return;
			}

			if (resolvedSym->getType().isUserDefinedType()) {
				shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(resolvedSym->getType()));
				//assert(structSym != nullptr);
				// ^^^ In testing, we may deliberate test negative cases by supplying invalid structs In such a case, don't crash the test
				if (structSym)
					structSymbolStack.push(structSym);
			}
		}
	}
}

void Resolution::exitMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
	if (!structSymbolStack.empty()) {
		shared_ptr<StructSymbol> s = structSymbolStack.top();
		structSymbolStack.pop();
		string memName = ctx->ID()->getText();

		shared_ptr<Symbol> resMemSym = s->resolveMember(memName);

		// TODO: the semantics of StructSymbolTestWrapper::resolveMember should be similar to SymbolTable's
		// which is to return with a proper symbol or throw an exception. No nullptr. Maybe get rid of 
		// StructSymbolTestWrapper altogether.
		if (resMemSym) { // if not resolved, no need to check its type
			broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resMemSym, s);
			if (resMemSym->getType().isUserDefinedType()) {
				shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(resMemSym->getType()));
				//assert(structSym != nullptr);
				// ^^^ In testing, we may deliberate test negative cases by supplying invalid structs In such a case, don't crash the test
				if (structSym)
					structSymbolStack.push(structSym);
			}
		}
		else {
			broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, memName, s);
		}
	}

	// Empty stack is possible if the struct symbol were not resolved, in enterPrimaryId
	// eg, a.b, if "a" doesn't exist, it wouldn't have been pushed onto the stack
	// so we can't access it
	// TODO: Currently fails silent, make unsilent failure, expect to find a struct to be on stack
}

void Resolution::enterFuncCall(bluefinParser::FuncCallContext* ctx)
{
	pair<shared_ptr<Symbol>, shared_ptr<Scope>> resolvedSymAndScope = resolve(ctx->ID()->getText(), scopes.at(ctx));
	shared_ptr<Symbol> resolvedSym = resolvedSymAndScope.first;
	shared_ptr<Scope> scope = resolvedSymAndScope.second;

	if (!dynamic_pointer_cast<StructSymbol>(scope) && ctx->getStart()->getTokenIndex() < resolvedSym->getTokenIndex()) {
		broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, resolvedSym->getName());
	}
}

// eg, a.b(), 'a' must be a struct type. In non-chained cased, we can easily resolve 'a'. No memberAccess involved
// but if chained, eg, a.b.c(), we need to pass the struct to each other with the structSymbolStack, memberAccess involved
// can we chanin method calls like a.b().c(); I feel that should be valid logically but my impl doesn't handle that.
void Resolution::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
	shared_ptr<StructSymbol> structSym;

	if (structSymbolStack.empty())
	{
		shared_ptr<Symbol> sym = resolve(ctx->expr()->getText(), scopes.at(ctx->expr())).first;
		if (sym->getType().isUserDefinedType()) {
			structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(sym->getType()));
			//assert(structSym != nullptr);
			// ^^^ In testing, we may deliberate test negative cases by supplying invalid structs In such a case, don't crash the test
		}
	}
	else {

		structSym = structSymbolStack.top();
		structSymbolStack.pop();
	}

	// TODO: Define what happens if symbol not resolved or no struct symbol on stack.
	shared_ptr<Symbol> methodSym = structSym->resolveMember(ctx->ID()->getText()); // method

	if (methodSym) {
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, methodSym, structSym);
	}
	else {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, ctx->ID()->getText(), structSym);
	}
}

void Resolution::attachEventObserver(shared_ptr<EventObserver> observer)
{
	eventObservers.push_back(observer);
}

void Resolution::detachEventObserver(shared_ptr<EventObserver> observer)
{
	auto it = std::find(eventObservers.begin(), eventObservers.end(), observer);
	if (it != eventObservers.end()) {
		eventObservers.erase(it); 
	}
}


// TODO: refactor, right now, this is identical to SymbolTableTestWrapper::resolve
pair<shared_ptr<Symbol>, shared_ptr<Scope>> Resolution::resolve(const string name, shared_ptr<Scope> startScope) {

	pair<shared_ptr<Symbol>, shared_ptr<Scope>> resolvedSymAndScope;

	try {
		resolvedSymAndScope = resolveImpl(name, startScope);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resolvedSymAndScope.first);
		assert(resolvedSymAndScope.first->getName() == name);
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, name);
	}

	return resolvedSymAndScope;
}

void Resolution::broadcastEvent(SuccessEvent e, shared_ptr<Symbol> sym, shared_ptr<StructSymbol> structSym)
{
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, sym, structSym);
	}
}

void Resolution::broadcastEvent(ErrorEvent e, string symName, shared_ptr<StructSymbol> structSym) {
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, symName, structSym);
	}
}

pair<shared_ptr<Symbol>, shared_ptr<Scope>> Resolution::resolveImpl(const string name, shared_ptr<Scope> startScope) {
	shared_ptr<Scope> scope = startScope;

	do {
		shared_ptr<Symbol> sym = scope->resolve(name);
		if (sym) { 
			return { sym, scope }; 
		}
	} while (scope = scope->getParentScope());

	throw UnresolvedSymbolException(name);
}


