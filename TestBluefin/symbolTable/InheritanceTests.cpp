#include "pch.h"

#include "antlr4-runtime.h"
#include "../utils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../listeners/Declaration.h"
#include "../../listeners/Resolution.h"

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

		SymbolTable symTab;
		SymbolFactory symFact;

		shared_ptr<EventObserver> obs = make_shared<EventObserver>();

		Declaration declarationListener(symTab, symFact);
		declarationListener.attachEventObserver(obs);
		walker.walk(&declarationListener, tree);

		Resolution resolutionListener(symTab);
		resolutionListener.attachEventObserver(obs);
		walker.walk(&resolutionListener, tree);

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		string output = obs->getOutput();
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
