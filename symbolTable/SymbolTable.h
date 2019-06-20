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
	
		/*
		Returns the enclosing scope
		*/
		void exitScope();

		/**
		TODO: throw error or return something if multiple 
		redeclarations within same scope (using same name)
		TODO: the name is supposed to be the same as the 
		symbol name. Thus, consider removing first param
		*/
		void declare(const string name, Symbol* symbol);


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