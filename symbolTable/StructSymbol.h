#pragma once

#include <memory>
#include <iostream>
#include "Symbol.h"
#include "Scope.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;
	using std::enable_shared_from_this;

	/** TODO: Awkward design 
	How do we actually make a scope? From SymbolTable::enterScope(..). It literally just creates a scope that points to another once. A struct must contain a reference to a scope (aka, composition). Since a Scope is created in enterScope, we want to associate a struct with it (we presume the struct has already been declared into the symbol table. This requires:
	- After creating a scope, check if it should be associated with a struct. To do so, check that the most recently entered symbol is a struct. If so, associate
DOWNSIDE: 
	- Symbol table needs to keep track of state 
Creating a structsymbol and attaching its scope are now two different phases, but they should happen together, because it doesn't make sense to create a struct symbol (definition) without the scope. Can't guarantee this class invariant
	
	TODO: too many dangling pointers around. eg, scope. Also, who manages deletion of symbols, the scope or the creator?
	*/
	class StructSymbol : public Symbol, public Type, public enable_shared_from_this<StructSymbol> {

	public:
		StructSymbol(const string& name) : Symbol(name)
		{}

		/*
		This function should always be called b/c it doesn't make sense for a struct definition
		not to contain a scope. Otherwise
		*/
		void attachScope(shared_ptr<Scope>);

		virtual shared_ptr<Symbol> resolveMember(const string memberName); // virtual to allow testing, so test can write

		inline string getCategoryName() const override { return "struct"; }
		inline string type2str() const override { return getName(); }
		shared_ptr<Type> getType()  override { return shared_from_this(); }  // must make sure that this BuiltinTypeSymbol's raw ptr is already

	private:
		shared_ptr<Scope> scope;
	};

}
