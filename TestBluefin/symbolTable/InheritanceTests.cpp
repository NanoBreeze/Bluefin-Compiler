#include "pch.h"

#include "antlr4-runtime.h"
#include "../utils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../listeners/Declaration.h"
#include "../../listeners/Resolution.h"
#include "SymbolTableTestWrapper.h"
#include "SymbolWrapperFactory.h"

namespace SymbolTableTests {

	using std::string;
	using std::ifstream;
	using namespace bluefin;
	using namespace antlr4;


	void validateInheritance(const string programFile, const string expectedOutputFile) {
		string pathPrefix = "../TestBluefin/symbolTable/programs/Inheritance/";

		ifstream file(pathPrefix + programFile);
		tree::ParseTree* tree = createParseTree(file);

		tree::ParseTreeWalker walker;
		string output = "";

		SymbolTableTestWrapper symTab(output); 
		SymbolWrapperFactory symFact(output);

		Declaration declarationListener(symTab, symFact);
		walker.walk(&declarationListener, tree);

		Resolution resolutionListener(declarationListener.getScopes(), output, symTab);
		walker.walk(&resolutionListener, tree);

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		std::cout << "OUTPUT:" << std::endl;
		std::cout << output << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_EQ(output, expectedOutput);
	}

	TEST(Inheritance, Program_ExternalResolution) {
		validateInheritance("ExternalResolution.bf", "ExternalResolution_expected.txt");
	}
	TEST(Inheritance, Program_InternalResolution) {
		validateInheritance("InternalResolution.bf", "InternalResolution_expected.txt");
	}
	TEST(Inheritance, Program_NameHiding) {
		validateInheritance("NameHiding.bf", "NameHiding_expected.txt");
	}
	/*
	TEST(Inheritance, Program_SuperclassMembers) {
		validateInheritance("SuperclassMembers.bf", "SuperclassMembers_expected.txt");
	}
	*/
}
