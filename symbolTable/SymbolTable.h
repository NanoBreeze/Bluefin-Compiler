#pragma once

#include "Scope.h"

namespace bluefin {

	class SymbolTable
	{
		SymbolTable() : currScope{ nullptr }
		{}
		
		void enterScope();
	
		/*
		Returns the enclosing scope
		*/
		Scope* exitScope();

		/**
		TODO: throw error or return something if multiple 
		redeclarations within same scope (using same name)
		*/
		void declare(const string name, Symbol* symbol);

		/*
		Find the name in the curr scope, if not, find in its 
		parent scope, and continue bubbling upwards. 
		If not found, return nullptr
		*/
		Symbol* resolve(const string name);
	private:
		Scope* currScope;
	};

}