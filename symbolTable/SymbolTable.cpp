#include "SymbolTable.h"

using namespace bluefin;

void SymbolTable::enterScope(const string scopeName) {
	Scope* scope = new Scope(currScope, scopeName); // TODO: how are we ever going to delete this scope
	currScope = scope;
}

void SymbolTable::exitScope() {
	Scope* enclosingScope = currScope->getEnclosingScope();
	delete currScope;
	currScope = enclosingScope;
}

void SymbolTable::declare(const string name, Symbol* symbol) {
	currScope->declare(name, symbol);
}


Symbol* SymbolTable::resolve(const string name) {

	Scope* scope = currScope;

	do {
		Symbol* sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = scope->getEnclosingScope());
		
	return nullptr;
}
