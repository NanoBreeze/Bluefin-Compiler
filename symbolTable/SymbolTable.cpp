#include "SymbolTable.h"

using namespace bluefin;

void SymbolTable::enterScope() {
	Scope* scope = new Scope(currScope); // TODO: how are we ever going to delete this scope
	currScope = scope;
}

Scope* SymbolTable::exitScope() {
	return currScope->getEnclosingScope();
}

void SymbolTable::declare(const string name, Symbol* symbol) {
	currScope->declare(name, symbol);
}


Symbol* SymbolTable::resolve(const string name) {

	Scope* scope = currScope;

	do {
		Symbol* sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = currScope->getEnclosingScope());
		
	return nullptr;
}
