#include "pch.h"

#include "antlr4-runtime.h"
#include "utils.h"
#include <iostream>
#include <fstream>

#include "../symbolTable/SymbolTable.h"
#include "../src/PopulateSymTabListener.h"
#include "SymbolTableTestWrapper.h"

namespace SymbolTableTests {

	using std::string;
	using std::ifstream;
	using namespace bluefin;
	using namespace antlr4;

	string pathPrefix = "../TestBluefin/symbolTable/programs/";

	void validateProgram(const string programFile, const string expectedOutputFile) {

		ifstream file(pathPrefix + programFile);
		tree::ParseTree* tree = createParseTree(file);

		tree::ParseTreeWalker walker;
		SymbolTableTestWrapper symTab; 
		PopulateSymTabListener listener(symTab);

		walker.walk(&listener, tree);

		string output = symTab.getOutput();

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		std::cout << "OUTPUT:" << std::endl;
		std::cout << output << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_EQ(output, expectedOutput);

	}
	TEST(SymbolTable, Program_BuiltinDeclaration) {
		validateProgram("BuiltinDeclaration.bf", "BuiltinDeclaration_expected.txt");
	}

	TEST(SymbolTable, Program_DeclarationInNestedScopes) {
		validateProgram("DeclarationInNestedScopes.bf", "DeclarationInNestedScopes_expected.txt");
	}
	TEST(SymbolTable, Program_FunctionDeclaration) {
		validateProgram("FunctionDeclaration.bf", "FunctionDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_MixedDeclaration) {
		//MixedDeclaration
		validateProgram("MixedDeclaration.bf", "MixedDeclaration_expected.txt");
	}

	TEST(SymbolTable, Program_SameDeclarationInSameScope) {
		validateProgram("SameDeclarationInSameScope.bf", "SameDeclarationInSameScope_expected.txt");
	}
	TEST(SymbolTable, Program_StructDeclaration) {
		validateProgram("StructDeclaration.bf", "StructDeclaration_expected.txt");
	}
}
