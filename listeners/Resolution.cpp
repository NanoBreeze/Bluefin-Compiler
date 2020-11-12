#include "Resolution.h"

#include "../symbolTable/Exceptions.h"
#include "../symbolTable/FunctionSymbol.h"

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

		// Now, since this is a primary id, we expect the resolved symbol can't be a functionSymbol
		// Otherwise, it's like this example: void f() {} int main() {f;}, which isn't right
		if (dynamic_pointer_cast<FunctionSymbol>(sym)) {
			broadcastEvent(ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM, varName);
			return;
		}
		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, sym);

		shared_ptr<Scope> scopeContainingResolvedSym = symbolTable.getScope(sym);
		if (!dynamic_pointer_cast<StructSymbol>(scopeContainingResolvedSym) && ctx->getStart()->getTokenIndex() < sym->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, sym->getName());
			return;
		}

		symbolTable.updateParseTreeContextWithResolvedSym(ctx, sym);
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, varName);
	}
}

void Resolution::exitMemberAccess(bluefinParser::MemberAccessContext* ctx)
{
	// I want to refactor this method, especially all the try catch statements.
	string memName = ctx->ID()->getText();
	shared_ptr<Symbol> leftSymbol = symbolTable.getResolvedSymbol(ctx->expr());
	if (leftSymbol == nullptr) { // the left symbol can't be resolved (eg, a.b). Then the primaryId would have already broadcasted it can't resolve the symbol.
		// For multichained one, eg, a.b.c, if ctx->expr() refers to 'b', then b couldn't be resolved, then the previous invocation of exitMemberAccess(..)
		//  must have also already broadcasted the unresolved symbol, so nothing to do here.
		// Ideally, we should still broadcast an unresolved event and create a filterer to decide which ones to show the user
		return;
	}

	shared_ptr<StructSymbol> structSym;
	// this can throw an exception. Check if the leftSymbol's type resolves to an actual Struct definition
	// Eg)  UndefinedType blah;
	//		blah.here; (the type of blah's VarSym is "UndefinedType", which doesn't have a matching one.
	try {
		structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(leftSymbol->getType()));
	}
	catch (UnresolvedStructDefException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE, leftSymbol->getName());
		return; 
	}

	if (structSym) { // the type of the leftward symbol (eg, the 'a' in 'a.b') is a struct and its StructSymbol exists. Now check that the id is a member
		try {
			shared_ptr<Symbol> resMemSym = symbolTable.resolveMember(memName, structSym);

			if (dynamic_pointer_cast<FunctionSymbol>(resMemSym)) {
				broadcastEvent(ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM, memName, structSym);
				return;
			}
			broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resMemSym, structSym);
			symbolTable.updateParseTreeContextExternalStructMember(ctx, structSym, resMemSym);
		}
		catch (UnresolvedSymbolException e) {
			broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, memName, structSym);
		}
	}
	else { // the VarSym that corresponds to 'a' in 'a.b' has a valid Type, but it doesn't correspond to a StructSym
		// eg, int a; a.b;
		broadcastEvent(ErrorEvent::BUILTINTYPE_CANNOT_HAVE_MEMBER, structSym->getName());
	}
}

// NOTE: We want to check if we're making the func call inside a struct's method. If so, then it's okay for
// the method to be declared later in the struct than the current pos
// To make life easier for DecorateExprWithTypes, here, we must check that not only is the resolved symbol resolved
void Resolution::enterFuncCall(bluefinParser::FuncCallContext* ctx)
{
	string varName = ctx->ID()->getText();
	shared_ptr<Scope> scopeContainingFuncCall = symbolTable.getScope(ctx);
	try {
		shared_ptr<Symbol> sym = symbolTable.resolve(varName, scopeContainingFuncCall);
		assert(sym->getName() == varName);

		if (dynamic_pointer_cast<FunctionSymbol>(sym) == nullptr) {
			broadcastEvent(ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM, varName);
			return;
		}

		broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, sym);

		shared_ptr<Scope> scopeContainingResolvedSym = symbolTable.getScope(sym);
		if (!dynamic_pointer_cast<StructSymbol>(scopeContainingResolvedSym) && ctx->getStart()->getTokenIndex() < sym->getTokenIndex()) {
			broadcastEvent(ErrorEvent::ILLEGAL_FORWARD_REFERENCE, sym->getName());
			return;
		}
 
		symbolTable.updateParseTreeContextWithResolvedSym(ctx, sym);
	}
	catch (UnresolvedSymbolException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, varName);
	}
}

/* In the past, we used a stack of StructSymbol. Now, we use a map of ParseTree* with corresponding Context values. When something is resolved fine, the Context
values are changed; otherwise, the Context values remain nullptr.
eg, a.b().c, 'a' must resolve to a VarSym. If it doesn't, the primaryId would have already reported the error, and in extMethodCall, we simply exit.
We then get the Type of the VarSym and call symbolTable::getSymbolMatchingType(..) to get the StructSym. If the StructSym doesn't exist, report an error and exit.
We then try to resolve b as a member of the StructSym. If it can't be resolved, report an error. If it can, in this case, it will be a FunctionSymbol. Great, now update the ParseTree* entry with the variable of resolved.
For the '.c' part, exitMemberAccess(..) will be called. It gets the symbol resolved from the ParseTree* that corresponded to b(). It also gets the Type (a FunctionSymbol's 
Type is its return type, in this case, we expect it to correspond to a StructSym), and tries to resolve 'c' from the StructSym. 
*/
void Resolution::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
	// Basically identical to exitMemberAccess
	// I want to refactor this method, especially all the try catch statements.
	string memName = ctx->ID()->getText();
	shared_ptr<Symbol> leftSymbol = symbolTable.getResolvedSymbol(ctx->expr());
	if (leftSymbol == nullptr) { 
		return;
	}

	shared_ptr<StructSymbol> structSym;
	// this can throw an exception
	// Eg)  UndefinedType blah;
	//		blah.here; (the type of blah's VarSym is "UndefinedType", which doesn't have a matching one.
	try {
		structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(leftSymbol->getType()));
	}
	catch (UnresolvedStructDefException e) {
		broadcastEvent(ErrorEvent::UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE, leftSymbol->getName());
		return; 
	}

	if (structSym) { // the type of the leftward symbol (eg, the 'a' in 'a.b') is a struct and its StructSymbol exists. Now check that the id is a member
		try {
			shared_ptr<Symbol> resMemSym = symbolTable.resolveMember(memName, structSym);
			if (dynamic_pointer_cast<FunctionSymbol>(resMemSym) == nullptr) {
				broadcastEvent(ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM, memName, structSym);
				return;
			}
			broadcastEvent(SuccessEvent::RESOLVED_SYMBOL, resMemSym, structSym);
			symbolTable.updateParseTreeContextExternalStructMember(ctx, structSym, resMemSym);
		}
		catch (UnresolvedSymbolException e) {
			broadcastEvent(ErrorEvent::UNRESOLVED_SYMBOL, memName, structSym);
		}
	}
	else { // the VarSym that corresponds to 'a' in 'a.b' has a valid Type, but it doesn't correspond to a StructSym
		// eg, int a; a.b;
		broadcastEvent(ErrorEvent::BUILTINTYPE_CANNOT_HAVE_MEMBER, structSym->getName());
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
	for (shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, sym, structSym);
	}
}

void Resolution::broadcastEvent(ErrorEvent e, string symName, shared_ptr<StructSymbol> structSym) {
	for (shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, symName, structSym);
	}
}
