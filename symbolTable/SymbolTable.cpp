
#include <memory>
#include "SymbolTable.h"
#include "StructSymbol.h"
#include "BuiltinTypeSymbol.h"
#include "./Exceptions.h"


namespace bluefin {

	using std::dynamic_pointer_cast;
	using std::make_shared;

	SymbolTable::SymbolTable() : currScope{ new Scope(nullptr, "global") } {

		// Add all built-in types to the global scope
		declare(BuiltinTypeSymbol::BOOL());
		declare(BuiltinTypeSymbol::STRING());
		declare(BuiltinTypeSymbol::INT());
		declare(BuiltinTypeSymbol::FLOAT());
		declare(BuiltinTypeSymbol::VOID());
	}

	void SymbolTable::enterScope(const string scopeName) {
		shared_ptr<Scope> scope = make_shared<Scope>(currScope, scopeName);
		currScope = scope;
	}

	/*
	Sets the passed in scope as the current scope so all declares and resolves
	come from there. Right now, this is used only by StructScope to simplify its declaration logic.
	*/
	void SymbolTable::setCurrentScope(shared_ptr<Scope> scope)
	{
		assert(scope->getEnclosingScope() == currScope);

		currScope = scope;
	}

	void SymbolTable::exitScope() {
		currScope = currScope->getEnclosingScope();
	}

	void SymbolTable::declare(shared_ptr<Symbol> symbol) {
		currScope->declare(symbol);
	}


	shared_ptr<Symbol> SymbolTable::resolve(const string name) {

		shared_ptr<Scope> scope = currScope;

		do {
			shared_ptr<Symbol> sym = scope->resolve(name);
			if (sym) { return sym; }
		} while (scope = scope->getEnclosingScope());

		throw UnresolvedSymbolException(name);
	}
}
