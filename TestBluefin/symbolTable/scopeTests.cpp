#include "pch.h"

#include <iostream>
#include "../symbolTable/Exceptions.h"
#include "../symbolTable/Symbol.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/FunctionSymbol.h"
#include "../symbolTable/VariableSymbol.h"
#include "../symbolTable/BuiltinTypeSymbol.h"
#include "../symbolTable/StructSymbol.h"

namespace SymbolTableTests {

	using std::string;
	using namespace bluefin;

	TEST(SymbolTable, DefaultState) {
		SymbolTable symtab;
		Scope* scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "global");
		ASSERT_EQ(scope->getEnclosingScope(), nullptr);

		unordered_map<string, Symbol*> defaultSymbols = scope->getSymbols();
		ASSERT_EQ(defaultSymbols.size(), 5);
		ASSERT_TRUE(*dynamic_cast<BuiltinTypeSymbol*>(defaultSymbols.at("int")) == BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT)); 
		// for some reason ASSERT_EQ isn't working here
		ASSERT_TRUE(*dynamic_cast<BuiltinTypeSymbol*>(defaultSymbols.at("float")) == BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		ASSERT_TRUE(*dynamic_cast<BuiltinTypeSymbol*>(defaultSymbols.at("void")) == BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::VOID));
		ASSERT_TRUE(*dynamic_cast<BuiltinTypeSymbol*>(defaultSymbols.at("bool")) == BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL));
		ASSERT_TRUE(*dynamic_cast<BuiltinTypeSymbol*>(defaultSymbols.at("string")) == BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::STRING));
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
		ASSERT_EQ(scope->getName(), "global");
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
		ASSERT_EQ(scope->getName(), "global"); 

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

		ASSERT_EQ(symtab.getCurrScope()->getName(), "global"); 
		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope(), nullptr);
	}

	TEST(SymbolTable, DeclareInSingleScope) {
		SymbolTable symtab;
		symtab.enterScope("First");

		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		symtab.declare(symA);
		symtab.declare(symB); 

		unordered_map<string, Symbol*> symbols = symtab.getCurrScope()->getSymbols();
		
		ASSERT_EQ(symbols.size(), 2);
		ASSERT_EQ(symbols.at("a"), symA);
		ASSERT_EQ(symbols.at("b"), symB);
	}

	TEST(SymbolTable, DeclareInTwoScopes) {
		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA);
		symtab.declare(symB); 

		symtab.enterScope("Second");
		Symbol* symC = new VariableSymbol("c", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symD = new VariableSymbol("d", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		symtab.declare(symC);
		symtab.declare(symD); 

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
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		Symbol* symA2 = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB2 = new FunctionSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		Symbol* symD = new VariableSymbol("d", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA2);
		symtab.declare(symB2);
		symtab.declare(symD);

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

		ASSERT_EQ(symtab.getCurrScope()->getName(), "global");

		symtab.exitScope();
		ASSERT_EQ(symtab.getCurrScope(), nullptr);
	}

	TEST(SymbolTable, Redefinition) {
		// Redefinition is illegal and expect an exception to be thrown

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symAFunc = new FunctionSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		symtab.declare(symA);

		ASSERT_THROW(symtab.declare(symAFunc), ReclarationException);
	}

	TEST(SymbolTable, ResolutionInSameScope) {

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT));
		symtab.declare(symA);
		symtab.declare(symB);

		Symbol* resA = symtab.resolve("a");
		Symbol* resA2 = symtab.resolve("a");
		Symbol* resB = symtab.resolve("b");

		ASSERT_EQ(resA, symA);
		ASSERT_EQ(resA2, symA);
		ASSERT_EQ(resB, symB);
	}

	TEST(SymbolTable, ResolutionInDifferentScopes) {

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		Symbol* symA2 = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA2);

		Symbol* resA2 = symtab.resolve("a");
		Symbol* resB = symtab.resolve("b");
		ASSERT_EQ(resA2, symA2);
		ASSERT_EQ(resB, symB);

		symtab.exitScope();
		Symbol* resA = symtab.resolve("a");
		ASSERT_EQ(resA, symA);
	}

	TEST(SymbolTable, MissingDeclaration) {

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* symA = new VariableSymbol("a", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		EXPECT_THROW(symtab.resolve("c"), UnresolvedSymbolException);
	}

	TEST(SymbolTable, StructResolveMember) {
		// Test resolution of member
		// And that member doesn't go up in scope

		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* globalB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(globalB);
		Symbol* structA = new StructSymbol("A");
		symtab.declare(structA);

		symtab.enterScope("struct A's definition");
		Symbol* symB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		Symbol* symC = new VariableSymbol("c", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::STRING));
		symtab.declare(symB);
		symtab.declare(symC);
		symtab.exitScope();

		// A a;
		Symbol* resolveA = symtab.resolve("A");
		ASSERT_EQ(resolveA, structA);
		Symbol* declarea = new VariableSymbol("a", structA->getType()); 
		symtab.declare(declarea);

		// a.b; a.c
		StructSymbol* A = static_cast<StructSymbol*>(resolveA);
		ASSERT_EQ(A->resolveMember("b"), symB);
		ASSERT_EQ(A->resolveMember("c"), symC);
	}

	TEST(SymbolTable, StructMissingResolutionMember) {
		SymbolTable symtab;

		symtab.enterScope("First");
		Symbol* globalB = new VariableSymbol("b", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT));
		symtab.declare(globalB);
		Symbol* structA = new StructSymbol("A");
		symtab.declare(structA);

		symtab.enterScope("struct A's definition");
		Symbol* symC = new VariableSymbol("c", BuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::STRING));
		symtab.declare(symC);
		symtab.exitScope();

		// A a;
		Symbol* resolveA = symtab.resolve("A");
		ASSERT_EQ(resolveA, structA);
		Symbol* declarea = new VariableSymbol("a", structA->getType());
		symtab.declare(declarea);

		// a.b; a.c
		StructSymbol* A = static_cast<StructSymbol*>(resolveA);
		ASSERT_EQ(A->resolveMember("b"), nullptr);
		ASSERT_EQ(A->resolveMember("c"), symC);
	}
}

