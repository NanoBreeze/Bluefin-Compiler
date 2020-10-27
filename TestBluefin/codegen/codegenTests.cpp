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
		string pathPrefix = "codegen/programs/";

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

		CodeGeneration generator(symTab, programFile);
		walker.walk(&generator, tree);
		string output = generator.dump();


		string expectedOutput = SymbolTableTests::readFile(pathPrefix + expectedOutputFile);
		std::cout << "OUTPUT:" << std::endl;
		std::cout << output << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_EQ(output, expectedOutput);
	}

	TEST(CodeGen, Program_BasicFuncDef) {
		validateCodeGen("BasicFuncDef.bf", "BasicFuncDef_expected.txt");
	}

	TEST(CodeGen, Program_ExprForIntLiterals) {
		validateCodeGen("ExprForIntLiterals.bf", "ExprForIntLiterals_expected.txt");
	}

	TEST(CodeGen, Program_ExprForBool) {
		validateCodeGen("ExprForBool.bf", "ExprForBool_expected.txt");
	}
}
