#pragma once

#include <memory>
#include <iostream>
#include "Symbol.h"
#include "Scope.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;

	/** TODO: forward reference
	Might be circular reference to have a sp to enclosing scope. Consider using wp
	*/
	class StructSymbol : public Symbol, public Type, public Scope
	{

	public:
		StructSymbol(const string& name, shared_ptr<Scope> enclosingScope) : 
			Symbol(name, this), Scope {enclosingScope, name}
		{}

		/*
		TODO: Technically, this is not needed
		but if we use resolve, then structSymbolTestWrapper's resolve
		message will also be rpinted when symbol table searches
		So this is used to temporarily not mess up other tests
		*/
		virtual shared_ptr<Symbol> resolveMember(const string memberName) {// virtual to allow testing, so test can write
			return this->resolve(memberName);
		}

		inline string type2str() const override { return Symbol::getName(); }

	};

}
