#pragma once

#include <memory>
#include "Scope.h"
#include "../TestBluefin/utils.h"

namespace bluefin {

	using std::shared_ptr;
	using std::unique_ptr;;
	using std::make_shared;

	class SymbolTable
	{
	public:
		SymbolTable(); 
		
		virtual void enterScope(const string scopeName="");
	
		virtual void exitScope();

		virtual void declare(shared_ptr<Symbol> symbol);


		/*
		Find the name in the curr scope, if not, find in its 
		parent scope, and continue bubbling upwards. 
		If not found, return nullptr
		*/
		virtual shared_ptr<Symbol> resolve(const string name);

		/* 
		Why might somebody want the current scope?
		1. Testing
		2. To map the current context with the current scope, passing the symbols for future listener passes
		*/
		inline shared_ptr<Scope> getCurrScope() const { return currScope; }

	private:
		shared_ptr<Scope> currScope; // shared_ptr b/c it can refer to the same scope as a StructSymbol's
	};

}