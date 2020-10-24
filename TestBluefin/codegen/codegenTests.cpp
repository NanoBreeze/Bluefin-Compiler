#include "pch.h"

#include "antlr4-runtime.h"
#include "../utils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../listeners/Declaration.h"
#include "../../listeners/Resolution.h"
#include "../../listeners/CodeGeneration.h"

namespace CodeGenTests {
	using std::string;
	using std::ifstream;
	using antlr4::tree::ParseTree;
	using antlr4::tree::ParseTreeWalker;
	using std::map;
	using namespace bluefin;
	using namespace antlr4;

	void validateCodeGen(const string programFile, const string expectedOutputFile) {
		string pathPrefix = "../../TestBluefin/codegen/programs/";

		ifstream file(pathPrefix + programFile);
		tree::ParseTree* tree = SymbolTableTests::createParseTree(file);

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

		CodeGeneration generator(symTab);
		walker.walk(&generator, tree);
		generator.dump();


		//string expectedOutput = SymbolTableTests::readFile(pathPrefix + expectedOutputFile);
		//string output = obs->getOutput();
		//std::cout << "OUTPUT:" << std::endl;
		//std::cerr << "OUTPUT:" << std::endl;
		//std::cout << output << std::endl;
		//std::cout << "EXPECTED:" << std::endl;
		//std::cout << expectedOutput << std::endl;
		ASSERT_EQ("one", "two");
	}

	TEST(CodeGen, Program_BasicFuncDef) {
		validateCodeGen("basicFuncDef.bf", "ExternalResolution_expected.txt");
	}
}
