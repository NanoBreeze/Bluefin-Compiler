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

		Scope(Scope* enclosingScope, string name = "") : enclosingScope{ enclosingScope }, name{ name }
		{}
		
		/**
		\brief Puts the <name, symbol> pair into a map for further use
		*/
		void declare(const string name, Symbol* symbol);

		/**
		\brief Looks up the name/identifier in the current scope and returns the symbol
		*/
		Symbol* resolve(const string name) const; 

		Scope* getEnclosingScope() const;

		inline string getName() const { return name; }

		// TODO: for testing only. can't get friend function working
		inline unordered_map<string, Symbol*> getSymbols() const { return symbols; }

	private:
		Scope* enclosingScope;
		unordered_map<string, Symbol*> symbols;
		const string name;
	};
}
