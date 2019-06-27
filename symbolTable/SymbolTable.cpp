
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
		declare(make_shared<BuiltinTypeSymbol>(BuiltinTypeSymbol::BuiltinType::BOOL));
		declare(make_shared<BuiltinTypeSymbol>(BuiltinTypeSymbol::BuiltinType::FLOAT));
		declare(make_shared<BuiltinTypeSymbol>(BuiltinTypeSymbol::BuiltinType::INT));
		declare(make_shared<BuiltinTypeSymbol>(BuiltinTypeSymbol::BuiltinType::VOID));
		declare(make_shared<BuiltinTypeSymbol>(BuiltinTypeSymbol::BuiltinType::STRING));
	}

	void SymbolTable::enterScope(const string scopeName) {
		shared_ptr<Scope> scope = make_shared<Scope>(currScope, scopeName);

		if (currScope) { // initially, currScope is nullptr, so can't get its most recent member
			if (shared_ptr<StructSymbol> s = dynamic_pointer_cast<StructSymbol>(currScope->getMostRecentDeclaredSymbol())) {
				s->attachScope(scope);
			}
		}

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
