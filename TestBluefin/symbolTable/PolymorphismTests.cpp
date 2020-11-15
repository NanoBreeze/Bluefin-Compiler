#include "pch.h"

#include "antlr4-runtime.h"
#include "../testutils.h"
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


	void validatePolymorphism(const string programFile, const string expectedOutputFile) {
		string pathPrefix = "../TestBluefin/symbolTable/programs/Polymorphism/";

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
		walker.walk(&resolutionListener, tree);

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		string output = obs->getPolymorphismOutput();
		std::cout << "OUTPUT:" << std::endl;
		std::cout << output << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_EQ(output, expectedOutput);
	}

	TEST(Polymorphism, Program_IncorrectVirtualOverrideUse) {
		validatePolymorphism("IncorrectVirtualOverrideUse.bf", "IncorrectVirtualOverrideUse_expected.txt");
	}

	TEST(Polymorphism, Program_MandatoryOverrideKeyword) {
		validatePolymorphism("MandatoryOverrideKeyword.bf", "MandatoryOverrideKeyword_expected.txt");
	}

	TEST(Polymorphism, Program_VirtualMethodNotFound) {
		validatePolymorphism("VirtualMethodNotFound.bf", "VirtualMethodNotFound_expected.txt");
	}

	TEST(Polymorphism, Program_IllegalVirtualMethodOverload) {
		validatePolymorphism("IllegalVirtualMethodOverload.bf", "IllegalVirtualMethodOverload_expected.txt");
	}
}
