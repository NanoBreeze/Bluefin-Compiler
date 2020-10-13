#include "Resolution.h"

#include "../symbolTable/Exceptions.h"

using namespace bluefin;
using std::dynamic_pointer_cast;

// Eg) int a = 5; Here, we want to resolve "int". The type can be either a user-defined type or built-in type
// In the past, such resolution was done in Declaration pass, but now we move it into Resolution pass
// Note: the symbol corresponding typeName is always at global scope
void Resolution::enterVarDecl(bluefinParser::VarDeclContext* ctx) {
	const string typeName = ctx->type()->getText(); 
	shared_ptr<Scope> scope = symbolTable.getScope(ctx); 
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName, scope);
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
	shared_ptr<Scope> scope = symbolTable.getScope(ctx); 
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName, scope);
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
	shared_ptr<Scope> scope = symbolTable.getScope(ctx); 
	try {
		shared_ptr<Symbol> typeSymbol = symbolTable.resolve(typeName, scope);
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
	string varName = ctx->ID()->getText();
	shared_ptr<Scope> scopeContainingId = symbolTable.getScope(ctx);
	try {
		shared_ptr<Symbol> sym = symbolTable.resolve(varName, scopeContainingId);
		// Alternatively, instead of scopeContainingId->getEnclosingScope(), we can check if the scopeContianingId is a method (currently, no easy way of doing that w/o looking at struct)
		assert(sym->getName() == varName);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, sym);

		shared_ptr<Scope> scopeContainingResolvedSym = symbolTable.getScope(sym);
		if (!dynamic_pointer_cast<StructSymbol>(scopeContainingResolvedSym) && ctx->getStart()->getTokenIndex() < sym->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, sym->getName());
			return;
		}

		if (sym->getType().isUserDefinedType()) {
			shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(sym->getType()));
			//assert(structSym != nullptr);
			// ^^^ In testing, we may deliberate test negative cases by supplying invalid structs In such a case, don't crash the test
			// eg) a.b, if "a" doesn't exist, we don't push it onto the stack
			if (structSym)
				structSymbolStack.push(structSym);
		}
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, varName);
	}
}

void Resolution::exitMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
	if (!structSymbolStack.empty()) {
		shared_ptr<StructSymbol> s = structSymbolStack.top();
		structSymbolStack.pop();
		string memName = ctx->ID()->getText();

		try {
			shared_ptr<Symbol> resMemSym = symbolTable.resolveMember(memName, s); //.s->resolve(memName);

			broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resMemSym, s);
			if (resMemSym->getType().isUserDefinedType()) {
				shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(resMemSym->getType()));
				//assert(structSym != nullptr);
				// ^^^ In testing, we may deliberate test negative cases by supplying invalid structs In such a case, don't crash the test
				if (structSym)
					structSymbolStack.push(structSym);
			}
		}
		catch (UnresolvedSymbolException e) {
			broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, memName, s);
		}
	}

	// The stack may be empty if the StructSymbol weren't resolved, in which case it wouldn't be put onto the stack
	// eg) a.b, if "a" doesn't exist, it wouldn't have been pushed onto the stack // so we can't access it
	// TODO: Should there be a broadcast here?
}

// NOTE: We want to check if we're making the func call inside a struct's method. If so, then it's okay for
// the method to be declared later in the struct than the current pos
void Resolution::enterFuncCall(bluefinParser::FuncCallContext* ctx)
{
	string varName = ctx->ID()->getText();
	shared_ptr<Scope> scopeContainingFuncCall = symbolTable.getScope(ctx);
	try {
		shared_ptr<Symbol> sym = symbolTable.resolve(varName, scopeContainingFuncCall);
		assert(sym->getName() == varName);
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, sym);

		shared_ptr<Scope> scopeContainingResolvedSym = symbolTable.getScope(sym);
		if (!dynamic_pointer_cast<StructSymbol>(scopeContainingResolvedSym) && ctx->getStart()->getTokenIndex() < sym->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, sym->getName());
		}
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, varName);
	}
}

// eg, a.b(), 'a' must be a struct type. In non-chained cased, we can easily resolve 'a'. No memberAccess involved
// but if chained, eg, a.b.c(), we need to pass the struct to each other with the structSymbolStack, memberAccess involved
// can we chanin method calls like a.b().c(); I feel that should be valid logically but my impl doesn't handle that.
void Resolution::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
	shared_ptr<StructSymbol> structSym;
	string methodName = ctx->ID()->getText();

	if (structSymbolStack.empty())
	{
		shared_ptr<Scope> s = symbolTable.getScope(ctx);
		try {
			shared_ptr<Symbol> sym = symbolTable.resolve(methodName, s);
			structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(sym->getType()));
		}
		catch (UnresolvedSymbolException e) {}
	}
	else {

		structSym = structSymbolStack.top();
		structSymbolStack.pop();
	}

	// TODO: Define what happens if symbol not resolved or no struct symbol on stack.
	try {
		shared_ptr<Symbol> methodSym = symbolTable.resolveMember(methodName, structSym); //structSym->resolve(methodName); // method
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, methodSym, structSym);
	}
	catch (UnresolvedSymbolException e) {
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
