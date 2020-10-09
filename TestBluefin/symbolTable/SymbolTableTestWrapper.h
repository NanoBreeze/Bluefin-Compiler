#pragma once

#include <string>
#include <memory>
#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/Symbol.h"

namespace SymbolTableTests {

	using namespace bluefin;
	using std::string;
	using std::shared_ptr;

	/**
	This class is used to test that the four functions (enterScope, exitScope, declare, resolve)
	are called at the correct places when a walker traverses the AST. Each function
	calls the respective SymbolTable::functionName and writes an output message.
	At the end of the walking, we compare the debug message with an expected result
	to verify that the walker is correctly calling the functions at the appropriate places

	The debug msgs for the following functions are:
	enterScope: "enterScope - Level X", where X is scopeLevel, which increments and decrements appropriately
	exitScope: "exitScope - Level X"
	declare: "declare - varName - symCategory - symType", where varName is the variable's identifier, eg, "x", "y"
		and symCategory can be one of "builtinType", "function", "struct", "var"
		symType is the type of the declared variable. eg, "int", "bool", "First". 
		eg, "declare - x - function - int"
	resolve: "resolve - varName - resolvedSymCategory - resolvedSymType (and type of this var)", where varName is the variable and resolvedSymCategory 
		is the category of the finally resolved variable. resolvedSymType is the type of the resolved symbol, and hence the type of this var
		For builtin-types, eg, "int", the type2str is...drumroll please...also int
	*/
	class SymbolTableTestWrapper : public SymbolTable
	{
	public:

		// the output is the debug message to be written. It is shared with the
		// symbol wrappers so they all write to it.
		SymbolTableTestWrapper(string& output) : output{ output }, scopeLevel{ 0 }
		{}

		void enterScope(const string scopeName = "") override;

		void setCurrentScope(shared_ptr<Scope> newCurrScope) override;

		void exitScope() override;

		void declare(shared_ptr<Symbol> symbol) override;

		shared_ptr<Symbol> resolve(const string name) override;

		shared_ptr<Symbol> getSymbolMatchingType(Type type) override;

	private:
		string& output;
		unsigned scopeLevel;

		string createEnterScopeDebugMsg();
		string createSetCurrentScopeDebugMsg();
		string createExitScopeDebugMsg();
		string createDeclareDebugMsg(shared_ptr<Symbol>) const;
		string createRedeclarationDebugMsg(shared_ptr<Symbol>) const;
		string createResolveDebugMsg(shared_ptr<Symbol> resolvedSym) const;
		string createUnresolvedDebugMsg(string resolvedSym) const;
	};
}
