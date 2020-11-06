#include "pch.h"

#include "antlr4-runtime.h"
#include "../testutils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../symbolTable/ErrorCollector.h"
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

		Declaration declarationListener(symTab, symFact);
		walker.walk(&declarationListener, tree);

		Resolution resolutionListener(symTab);
		walker.walk(&resolutionListener, tree);

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		ErrorCollector errCollector = declarationListener.getErrorCollector();
		std::cout << "OUTPUT:" << std::endl;
		std::cout << errCollector.getMsg() << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_EQ(errCollector.getMsg(), expectedOutput);
	}

	TEST(Polymorphism, Program_OverrideMethod) {
		validatePolymorphism("OverrideMethod.bf", "OverrideMethod_expected.txt");
	}
}
