#include "SymbolTable.h"
#include "StructSymbol.h"

using namespace bluefin;

void SymbolTable::enterScope(const string scopeName) {
	Scope* scope = new Scope(currScope, scopeName); 

	if (currScope) { // initially, currScope is nullptr, so can't get its most recent member
		if (StructSymbol * s = dynamic_cast<StructSymbol*>(currScope->getMostRecentDeclaredSymbol())) {
			s->attachScope(scope);
		}
	}

	currScope = scope;
}

void SymbolTable::exitScope() {
	Scope* enclosingScope = currScope->getEnclosingScope(); 
	// don't delete scope here. What if it's needed by someone else, eg, StructSymbol
	// would still need to refer to its scope.
	// TODO: Instead, store all scopes somewhere else for deletion...
	// Problem: StructSymbol should delete its own scope
	// To avoid double deletion, we can't just stopre all current scopes and delete them
	// because that would include the structSymbol's scope, which had already been deleted (double deletion)
	// Solution? Just use smart pointers
	//delete currScope;
	currScope = enclosingScope;
}

void SymbolTable::declare(Symbol* symbol) {
	currScope->declare(symbol);
}


Symbol* SymbolTable::resolve(const string name) {

	Scope* scope = currScope;

	do {
		Symbol* sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = scope->getEnclosingScope());
		
	return nullptr;
}
