#pragma once

#include "Scope.h"
#include "../TestBluefin/utils.h"

namespace bluefin {

	class SymbolTable
	{
	public:
		SymbolTable() : currScope{ nullptr }
		{}
		
		void enterScope(const string scopeName="");
	
		void exitScope();

		void declare(Symbol* symbol);


		/*
		Find the name in the curr scope, if not, find in its 
		parent scope, and continue bubbling upwards. 
		If not found, return nullptr
		*/
		Symbol* resolve(const string name);

		/* 
		This method is for testing currScope and the scope chain
		Having difficulty putting in a friend class.
		*/
		inline Scope* getCurrScope() const { return currScope; }

	private:
		Scope* currScope;
	};

}