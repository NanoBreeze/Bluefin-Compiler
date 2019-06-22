#pragma once

#include <string>
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/Symbol.h"

namespace SymbolTableTests {

	using namespace bluefin;
	using std::string;

	/**
	This class is used to test that the four functions (enterScope, exitScope, declare, resolve)
	are called at the correct places when a walker traverses the AST. Each function
	calls the respective SymbolTable::functionName and writes an output message.
	At the end of the walking, we compare the debug message with an expected result
	to verify that the walker is correctly calling the functions at the appropriate places

	The debug msgs for the following functions are:
	enterScope: "enterScope - Level X", where X is scopeLevel, which increments and decrements appropriately
	exitScope: "exitScope - Level X"
	declare: "declare - varName - symCategory", where varName is the variable's identifier, eg, "x", "y"
		and symCategory can be one of "builtinType", "function", "struct", "var"
		eg, "declare - x - function"
	resolve: "resolve - varName - resolvedSymCategory", where varName is the variable and resolvedSymCategory 
		is the category of the finally resolved variable.
	
	*/
	class SymbolTableTestWrapper : public bluefin::SymbolTable
	{
	public:

		SymbolTableTestWrapper() : output{ "" }, scopeLevel{ 0 }
		{}

		void enterScope(const string scopeName = "") override;

		void exitScope() override;

		void declare(Symbol* symbol) override;

		Symbol* resolve(const string name) override;

		inline string getOutput() const { return output; }

	private:
		SymbolTable symbolTable;
		string output;
		unsigned scopeLevel;

		string createEnterScopeDebugMsg();
		string createExitScopeDebugMsg();
		string createDeclareDebugMsg(Symbol*) const;
		string createRedeclarationDebugMsg(Symbol*) const;
		string createResolveDebugMsg(Symbol const* resolvedSym) const;
		string createUnresolvedDebugMsg(string resolvedSym) const;
	};
}
