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
		virtual void declare(shared_ptr<Symbol> symbol);

		/**
		\brief Looks up the name/identifier in the current scope and returns the symbol
		// overriden to allow custom debug messages
		*/
		virtual shared_ptr<Symbol> resolve(const string name) const; 

		inline shared_ptr<Scope> getEnclosingScope() const { return enclosingScope; }

		// Used for inheritance. For structs, check superclass scope before global scope
		virtual inline shared_ptr<Scope> getParentScope() const { return enclosingScope; }

		inline string getName() const { return name; }

		// TODO: for testing only. can't get friend function working
		inline unordered_map<string, shared_ptr<Symbol>> getSymbols() const { return symbols; }

	private:
		shared_ptr<Scope> enclosingScope; // TODO: circular reference. outer scope contains StructSymbol, which contains this scope, which points to its parent, the outer scope
		const string name;
		
		unordered_map<string, shared_ptr<Symbol>> symbols;
	};
}
