#pragma once

#include <memory>
#include <unordered_map>
#include "Scope.h"
#include "../TestBluefin/utils.h"

namespace bluefin {

	using std::shared_ptr;
	using std::unique_ptr;;
	using std::make_shared;
	using std::unordered_map;

	class StructSymbol;


	class SymbolTable
	{
	public:
		SymbolTable(); 
		
		virtual void enterScope(const string scopeName="");

		virtual void setCurrentScope(shared_ptr<Scope>);
	
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

		virtual shared_ptr<Symbol> getSymbolMatchingType(Type type);

	private:
		void addUserDefinedType(shared_ptr<StructSymbol>);

		shared_ptr<Scope> currScope; // shared_ptr b/c it can refer to the same scope as a StructSymbol's
		unordered_map<Type, shared_ptr<Symbol>> typeSymbols;
	};

}