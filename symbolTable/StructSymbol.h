#pragma once

#include <memory>
#include <iostream>
#include "Symbol.h"
#include "Scope.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;

	/** TODO: Awkward design 
	How do we actually make a scope? From SymbolTable::enterScope(..). It literally just creates a scope that points to another once. A struct must contain a reference to a scope (aka, composition). Since a Scope is created in enterScope, we want to associate a struct with it (we presume the struct has already been declared into the symbol table. This requires:
	- After creating a scope, check if it should be associated with a struct. To do so, check that the most recently entered symbol is a struct. If so, associate
DOWNSIDE: 
	- Symbol table needs to keep track of state 
Creating a structsymbol and attaching its scope are now two different phases, but they should happen together, because it doesn't make sense to create a struct symbol (definition) without the scope. Can't guarantee this class invariant
	
	TODO: too many dangling pointers around. eg, scope. Also, who manages deletion of symbols, the scope or the creator?
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
