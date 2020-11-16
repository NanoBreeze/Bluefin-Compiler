#include "pch.h"

#include <iostream>
#include <memory>
#include <utility>
#include "../../symbolTable/Exceptions.h"
#include "../../symbolTable/Symbol.h"
#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/Scope.h"
#include "../../symbolTable/FunctionSymbol.h"
#include "../../symbolTable/VariableSymbol.h"
#include "../../symbolTable/BuiltinTypeSymbol.h"
#include "../../symbolTable/StructSymbol.h"
#include "../../symbolTable/SymbolFactory.h"

namespace SymbolTableTests {

	using std::string;
	using std::shared_ptr;
	using std::dynamic_pointer_cast;
	using std::static_pointer_cast;
	using std::make_shared;

	using namespace bluefin;
	using SFB = SymbolFactory::Builtin;


	TEST(SymbolTable, DefaultState) {
		SymbolFactory symFact;

		SymbolTable symtab;
		shared_ptr<Scope> scope = symtab.getCurrScope();
		ASSERT_EQ(scope->getName(), "global");
		ASSERT_EQ(scope->getEnclosingScope(), nullptr);

		unordered_map<string, shared_ptr<Symbol>> defaultSymbols = scope->getSymbols();
		ASSERT_EQ(defaultSymbols.size(), 5);

		ASSERT_TRUE(*(defaultSymbols.at("int")) == *symFact.createBuiltinTypeSymbol(SFB::INT));
		// for some reason ASSERT_EQ isn't working here
		ASSERT_TRUE(*(defaultSymbols.at("float")) == *symFact.createBuiltinTypeSymbol(SFB::FLOAT));
		ASSERT_TRUE(*(defaultSymbols.at("void")) == *symFact.createBuiltinTypeSymbol(SFB::VOID));
		ASSERT_TRUE(*(defaultSymbols.at("bool")) == *symFact.createBuiltinTypeSymbol(SFB::BOOL));
		ASSERT_TRUE(*(defaultSymbols.at("string")) == *symFact.createBuiltinTypeSymbol(SFB::STRING));
	}

	TEST(SymbolTable, EnterScope) {

		SymbolTable symtab;
		symtab.enterScope("Level 0");
		symtab.enterScope("Level 1");
		symtab.enterScope("Level 2");
		
		shared_ptr<Scope> scope = symtab.getCurrScope();
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

		shared_ptr<Scope> scope = symtab.getCurrScope();
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

		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::FLOAT(), 0);
		symtab.declare(symA);
		symtab.declare(symB); 

		unordered_map<string, shared_ptr<Symbol>> symbols = symtab.getCurrScope()->getSymbols();
		
		ASSERT_EQ(symbols.size(), 2);
		ASSERT_EQ(symbols.at("a"), symA);
		ASSERT_EQ(symbols.at("b"), symB);
	}

	TEST(SymbolTable, DeclareInTwoScopes) {
		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::FLOAT(), 0);
		symtab.declare(symA);
		symtab.declare(symB); 

		symtab.enterScope("Second");
		shared_ptr<Symbol> symC = make_shared<VariableSymbol>("c", Type::INT(), 0);
		shared_ptr<Symbol> symD = make_shared<VariableSymbol>("d", Type::FLOAT(), 0);
		symtab.declare(symC);
		symtab.declare(symD); 

		shared_ptr<Scope> secondScope = symtab.getCurrScope();
		ASSERT_EQ(secondScope->getName(), "Second");
		unordered_map<string, shared_ptr<Symbol>> secondScopeSymbols = secondScope->getSymbols();
		ASSERT_EQ(secondScopeSymbols.size(), 2);
		ASSERT_EQ(secondScopeSymbols.at("c"), symC);
		ASSERT_EQ(secondScopeSymbols.at("d"), symD);
		symtab.exitScope();

		shared_ptr<Scope> firstScope = symtab.getCurrScope();
		ASSERT_EQ(firstScope->getName(), "First");
		unordered_map<string, shared_ptr<Symbol>> firstScopeSymbols = firstScope->getSymbols();
		ASSERT_EQ(firstScopeSymbols.size(), 2);
		ASSERT_EQ(firstScopeSymbols.at("a"), symA);
		ASSERT_EQ(firstScopeSymbols.at("b"), symB);
	}

	TEST(SymbolTable, VariableFunctionHiding) {
		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::FLOAT(), 0);
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		shared_ptr<Symbol> symA2 = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB2 = make_shared<VariableSymbol>("b", Type::FLOAT(), 0);
		shared_ptr<Symbol> symD = make_shared<VariableSymbol>("d", Type::INT(), 0);
		symtab.declare(symA2);
		symtab.declare(symB2);
		symtab.declare(symD);

		shared_ptr<Scope> secondScope = symtab.getCurrScope();
		ASSERT_EQ(secondScope->getName(), "Second");
		unordered_map<string, shared_ptr<Symbol>> secondScopeSymbols = secondScope->getSymbols();
		ASSERT_EQ(secondScopeSymbols.size(), 3);
		ASSERT_EQ(secondScopeSymbols.at("a"), symA2);
		ASSERT_EQ(secondScopeSymbols.at("b"), symB2);
		ASSERT_EQ(secondScopeSymbols.at("d"), symD);
		symtab.exitScope();

		shared_ptr<Scope> firstScope = symtab.getCurrScope();
		ASSERT_EQ(firstScope->getName(), "First");
		unordered_map<string, shared_ptr<Symbol>> firstScopeSymbols = firstScope->getSymbols();
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
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symAFunc = make_shared<FunctionSymbol>("a", Type::FLOAT(), 0, false, false);
		symtab.declare(symA);

		ASSERT_THROW(symtab.declare(symAFunc), RedeclarationException);
	}

	TEST(SymbolTable, ResolutionInSameScope) {

		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::FLOAT(), 0);
		symtab.declare(symA);
		symtab.declare(symB);

		shared_ptr<Symbol> resA = symtab.resolve("a", symtab.getCurrScope());
		shared_ptr<Symbol> resA2 = symtab.resolve("a", symtab.getCurrScope());
		shared_ptr<Symbol> resB = symtab.resolve("b", symtab.getCurrScope());

		ASSERT_EQ(resA, symA);
		ASSERT_EQ(resA2, symA);
		ASSERT_EQ(resB, symB);
	}

	TEST(SymbolTable, ResolutionInDifferentScopes) {

		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::STRING(), 0);
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		shared_ptr<Symbol> symA2 = make_shared<VariableSymbol>("a", Type::INT(), 0);
		symtab.declare(symA2);

		shared_ptr<Symbol> resA2 = symtab.resolve("a", symtab.getCurrScope());
		shared_ptr<Symbol> resB = symtab.resolve("b", symtab.getCurrScope());
		ASSERT_EQ(resA2, symA2);
		ASSERT_EQ(resB, symB);

		symtab.exitScope();
		shared_ptr<Symbol> resA = symtab.resolve("a", symtab.getCurrScope());
		ASSERT_EQ(resA, symA);
	}

	TEST(SymbolTable, MissingDeclaration) {

		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> symA = make_shared<VariableSymbol>("a", Type::INT(), 0);
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::INT(), 0);
		symtab.declare(symA);
		symtab.declare(symB);

		symtab.enterScope("Second");
		EXPECT_THROW(symtab.resolve("c", symtab.getCurrScope()), UnresolvedSymbolException);
	}

	TEST(SymbolTable, StructResolveMember) {
		// Test resolution of member
		// And that member doesn't go up in scope

		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> globalB = make_shared<VariableSymbol>("b", Type::INT(), 0);
		symtab.declare(globalB);
		shared_ptr<Symbol> structA = make_shared<StructSymbol>("A", symtab.getCurrScope(), 0); 
		symtab.declare(structA);

		symtab.setCurrentScope(dynamic_pointer_cast<Scope>(structA));
		shared_ptr<Symbol> symB = make_shared<VariableSymbol>("b", Type::INT(), 0);
		shared_ptr<Symbol> symC = make_shared<VariableSymbol>("c", Type::STRING(), 0);
		symtab.declare(symB);
		symtab.declare(symC);
		symtab.exitScope();

		// A a;
		shared_ptr<Symbol> resolveA = symtab.resolve("A", symtab.getCurrScope());
		ASSERT_EQ(resolveA, structA);
		shared_ptr<Symbol> declarea = make_shared<VariableSymbol>("a", structA->getType(), 0);
		symtab.declare(declarea);

		// a.b; a.c
		shared_ptr<StructSymbol> A = static_pointer_cast<StructSymbol>(resolveA);
		ASSERT_EQ(A->resolve("b"), symB);
		ASSERT_EQ(A->resolve("c"), symC);
	}

	TEST(SymbolTable, StructMissingResolutionMember) {
		SymbolTable symtab;

		symtab.enterScope("First");
		shared_ptr<Symbol> globalB = make_shared<VariableSymbol>("b", Type::INT(), 0);
		symtab.declare(globalB);
		shared_ptr<Symbol> structA = make_shared<StructSymbol>("A", symtab.getCurrScope(), 0);
		symtab.declare(structA);

		symtab.setCurrentScope(dynamic_pointer_cast<Scope>(structA));
		shared_ptr<Symbol> symC = make_shared<VariableSymbol>("c", Type::INT(), 0);
		symtab.declare(symC);
		symtab.exitScope();

		// A a;
		shared_ptr<Symbol> resolveA = symtab.resolve("A", symtab.getCurrScope());
		ASSERT_EQ(resolveA, structA);
		shared_ptr<Symbol> declarea = make_shared<VariableSymbol>("a", structA->getType(), 0);
		symtab.declare(declarea);

		// a.b; a.c
		shared_ptr<StructSymbol> A = static_pointer_cast<StructSymbol>(resolveA);
		ASSERT_EQ(A->resolve("b"), nullptr);
		ASSERT_EQ(A->resolve("c"), symC);
	}
	
}

