#include "pch.h"

#include <iostream>
#include <../symbolTable/Exceptions.h>
#include "../symbolTable/Symbol.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/FunctionSymbol.h"
#include "../symbolTable/VariableSymbol.h"
#include "../symbolTable/BuiltinType.h"

namespace SymbolTableTests {

	using std::string;
	using namespace bluefin;

	TEST(SymbolTable, DefaultState) {
		SymbolTable symtab;
		Scope* scope = symtab.getCurrScope();
		ASSERT_EQ(scope, nullptr);
	}

	TEST(SymbolTable, EnterScope) {

		SymbolTable symtab;
		symtab.enterScope("Level 0");
		symtab.enterScope("Level 1");
		symtab.enterScope("Level 2");
		
		Scope* scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "Level 2");
		scope = scope->getEnclosingScope();
		ASSERT_EQ(scope->getName(), "Level 1");
		scope = scope->getEnclosingScope();
		ASSERT_EQ(scope->getName(), "Level 0");
		scope = scope->getEnclosingScope();
		ASSERT_EQ(scope, nullptr); 
	}

	TEST(SymbolTable, ExitScope) {
		SymbolTable symtab;
		symtab.enterScope("Level 0");
		symtab.enterScope("Level 1");
		symtab.enterScope("Level 2");

		Scope* scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "Level 2");

		symtab.exitScope();
		scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "Level 1");

		symtab.exitScope();
		scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "Level 0");

		symtab.exitScope();
		scope = symtab.getCurrScope();
		ASSERT_EQ(scope, nullptr); 
	}

	TEST(SymbolTable, EnterAndExitScope) {
		SymbolTable symtab;
		symtab.enterScope("First");
		symtab.enterScope("Second");

		ASSERT_EQ(symtab.getCurrScope()->getName(), "Second");

		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope()->getName(), "First");

		symtab.enterScope("Third");
		symtab.enterScope("Fourth");
		symtab.enterScope("Fifth");

		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope()->getName(), "Fourth");
		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope()->getName(), "Third");
		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope()->getName(), "First");
		symtab.exitScope();

		ASSERT_EQ(symtab.getCurrScope(), nullptr); 
	}

	TEST(SymbolTable, DeclareInSingleScope) {
		SymbolTable symtab;
		symtab.enterScope("First");

		Symbol* symA = new VariableSymbol("a", BuiltinType("int"));
		Symbol* symB = new VariableSymbol("b", BuiltinType("float"));
		symtab.declare("a", symA);
		symtab.declare("b", symB); 

		unordered_map<string, Symbol*> symbols = symtab.getCurrScope()->getSymbols();
		
		ASSERT_EQ(symbols.size(), 2);
		ASSERT_EQ(symbols.at("a"), symA);
		ASSERT_EQ(symbols.at("b"), symB);
	}

	TEST(SymbolTable, DeclareInTwoScopes) {
		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinType("int"));
		Symbol* symB = new VariableSymbol("b", BuiltinType("float"));
		symtab.declare("a", symA);
		symtab.declare("b", symB); 

		symtab.enterScope("Second");
		Symbol* symC = new VariableSymbol("c", BuiltinType("int"));
		Symbol* symD = new VariableSymbol("d", BuiltinType("float"));
		symtab.declare("c", symC);
		symtab.declare("d", symD); 

		Scope* secondScope = symtab.getCurrScope();
		ASSERT_EQ(secondScope->getName(), "Second");
		unordered_map<string, Symbol*> secondScopeSymbols = secondScope->getSymbols();
		ASSERT_EQ(secondScopeSymbols.size(), 2);
		ASSERT_EQ(secondScopeSymbols.at("c"), symC);
		ASSERT_EQ(secondScopeSymbols.at("d"), symD);
		symtab.exitScope();

		Scope* firstScope = symtab.getCurrScope();
		ASSERT_EQ(firstScope->getName(), "First");
		unordered_map<string, Symbol*> firstScopeSymbols = firstScope->getSymbols();
		ASSERT_EQ(firstScopeSymbols.size(), 2);
		ASSERT_EQ(firstScopeSymbols.at("a"), symA);
		ASSERT_EQ(firstScopeSymbols.at("b"), symB);
	}

	TEST(SymbolTable, VariableFunctionHiding) {
		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinType("int"));
		Symbol* symB = new VariableSymbol("b", BuiltinType("float"));
		symtab.declare("a", symA);
		symtab.declare("b", symB);

		symtab.enterScope("Second");
		Symbol* symA2 = new VariableSymbol("a", BuiltinType("float"));
		Symbol* symB2 = new FunctionSymbol("b", BuiltinType("float"));
		Symbol* symD = new VariableSymbol("d", BuiltinType("float"));
		symtab.declare("a", symA2);
		symtab.declare("b", symB2);
		symtab.declare("d", symD);

		Scope* secondScope = symtab.getCurrScope();
		ASSERT_EQ(secondScope->getName(), "Second");
		unordered_map<string, Symbol*> secondScopeSymbols = secondScope->getSymbols();
		ASSERT_EQ(secondScopeSymbols.size(), 3);
		ASSERT_EQ(secondScopeSymbols.at("a"), symA2);
		ASSERT_EQ(secondScopeSymbols.at("b"), symB2);
		ASSERT_EQ(secondScopeSymbols.at("d"), symD);
		symtab.exitScope();

		Scope* firstScope = symtab.getCurrScope();
		ASSERT_EQ(firstScope->getName(), "First");
		unordered_map<string, Symbol*> firstScopeSymbols = firstScope->getSymbols();
		ASSERT_EQ(firstScopeSymbols.size(), 2);
		ASSERT_EQ(firstScopeSymbols.at("a"), symA);
		ASSERT_EQ(firstScopeSymbols.at("b"), symB);
		symtab.exitScope();

		ASSERT_EQ(symtab.getCurrScope(), nullptr);
	}

	TEST(SymbolTable, Redefinition) {
		// Redefinition is illegal and expect an exception to be thrown

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinType("int"));
		Symbol* symAFunc = new FunctionSymbol("a", BuiltinType("float"));
		symtab.declare("a", symA);

		ASSERT_THROW(symtab.declare("a", symAFunc), RedefinitionException);
	}

}

