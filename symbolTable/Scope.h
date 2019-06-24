#pragma once

#include <unordered_map>
#include <memory>
#include <iostream>

#include "Symbol.h"

namespace bluefin {

	using std::unordered_map;
	using std::string;
	using std::unique_ptr;
	using std::shared_ptr;

	class Scope
	{
	public:

		Scope(shared_ptr<Scope> enclosingScope, string name = "") : enclosingScope{ enclosingScope }, name{ name }
		{}
		
		/**
		\brief Puts the <symbol.name, symbol> pair into a map for further use
		*/
		void declare(shared_ptr<Symbol> symbol);

		/**
		\brief Looks up the name/identifier in the current scope and returns the symbol
		*/
		shared_ptr<Symbol> resolve(const string name) const; 

		inline shared_ptr<Scope> getEnclosingScope() const { return enclosingScope; }

		inline string getName() const { return name; }

		inline shared_ptr<Symbol> getMostRecentDeclaredSymbol() const { return mostRecentDeclaredSymbol; }

		// TODO: for testing only. can't get friend function working
		inline unordered_map<string, shared_ptr<Symbol>> getSymbols() const { return symbols; }

	private:
		shared_ptr<Scope> enclosingScope;
		unordered_map<string, shared_ptr<Symbol>> symbols;
		const string name;

		shared_ptr<Symbol> mostRecentDeclaredSymbol; // Why would we need to know the most recently
		// declared symbol? If we want to do something with it (eg, SymbolTable 
		// needs it. if StructSymbol, attach scope)
	};
}
