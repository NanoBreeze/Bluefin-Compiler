#pragma once

#include <unordered_map>
#include <iostream>

#include "Symbol.h"

namespace bluefin {

	using std::unordered_map;
	using std::string;

	class Scope
	{
	public:

		Scope(Scope* enclosingScope, string name = "") :
			enclosingScope{ enclosingScope }, mostRecentDeclaredSymbol{ nullptr }, name{ name }
		{}
		
		/**
		\brief Puts the <symbol.name, symbol> pair into a map for further use
		*/
		void declare(Symbol* symbol);

		/**
		\brief Looks up the name/identifier in the current scope and returns the symbol
		*/
		Symbol* resolve(const string name) const; 

		inline Scope* getEnclosingScope() const { return enclosingScope; }

		inline string getName() const { return name; }

		inline Symbol* getMostRecentDeclaredSymbol() const { return mostRecentDeclaredSymbol; }

		// TODO: for testing only. can't get friend function working
		inline unordered_map<string, Symbol*> getSymbols() const { return symbols; }

	private:
		Scope* enclosingScope;
		unordered_map<string, Symbol*> symbols;
		const string name;

		Symbol* mostRecentDeclaredSymbol; // Why would we need to know the most recently
		// declared symbol? If we want to do something with it (eg, SymbolTable 
		// needs it. if StructSymbol, attach scope)
	};
}
