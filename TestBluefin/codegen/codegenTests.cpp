#include "pch.h"

#include "antlr4-runtime.h"
#include "../testutils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../listeners/Declaration.h"
#include "../../listeners/Resolution.h"
#include "../../listeners/DecorateExprWithTypes.h"
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

		string resolutionOutput = obs->getOutput();

		DecorateExprWithTypes decorateExprListener(symFact, symTab);
		decorateExprListener.attachEventObserver(obs);
		walker.walk(&decorateExprListener, tree);

		map<ParseTree*, TypeContext> typeContexts = decorateExprListener.getTypeContexts();
		CodeGeneration generator(symTab, typeContexts, programFile);
		walker.walk(&generator, tree);

		string output = generator.dump();
		bool ok = generator.isCodeGenOK();

		string expectedOutput = SymbolTableTests::readFile(pathPrefix + expectedOutputFile);
		std::cout << "OUTPUT:" << std::endl;
		std::cout << output << std::endl;
		std::cout << "EXPECTED:" << std::endl;
		std::cout << expectedOutput << std::endl;
		ASSERT_TRUE(generator.isCodeGenOK());
		ASSERT_EQ(output, expectedOutput);
	}

	TEST(CodeGen, Program_FuncDefFuncCallsRetStatement) {
		validateCodeGen("FuncDefFuncCallsRetStatement.bf", "FuncDefFuncCallsRetStatement_expected.txt");
	}

	TEST(CodeGen, Program_ExprForIntLiterals) {
		validateCodeGen("ExprForIntLiterals.bf", "ExprForIntLiterals_expected.txt");
	}

	TEST(CodeGen, Program_ExprForFloatLiterals) {
		validateCodeGen("ExprForFloatLiterals.bf", "ExprForFloatLiterals_expected.txt");
	}

	TEST(CodeGen, Program_ExprForBool) {
		validateCodeGen("ExprForBool.bf", "ExprForBool_expected.txt");
	}

	TEST(CodeGen, Program_ExprWithTypePromotions) {
		validateCodeGen("ExprWithTypePromotions.bf", "ExprWithTypePromotions_expected.txt");
	}

	TEST(CodeGen, Program_IfStatements) {
		validateCodeGen("IfStatements.bf", "IfStatements_expected.txt");
	}

	TEST(CodeGen, Program_WhileStatements) {
		validateCodeGen("WhileStatements.bf", "WhileStatements_expected.txt");
	}

	TEST(CodeGen, Program_ManyRetStatementsWithIfWhileStatements) {
		validateCodeGen("ManyRetStatementsWithIfWhileStatements.bf", "ManyRetStatementsWithIfWhileStatements_expected.txt");
	}

	TEST(CodeGen, Program_GlobalVarDecl) {
		validateCodeGen("GlobalVarDecl.bf", "GlobalVarDecl_expected.txt");
	}

	TEST(CodeGen, Program_LocalVarDecl) {
		validateCodeGen("LocalVarDecl.bf", "LocalVarDecl_expected.txt");
	}

	TEST(CodeGen, Program_ExprForAssignments) {
		validateCodeGen("ExprForAssignments.bf", "ExprForAssignments_expected.txt");
	}

	TEST(CodeGen, Program_StructDef) {
		validateCodeGen("StructDef.bf", "StructDef_expected.txt");
	}

	TEST(CodeGen, Program_StructExternalAccess) {
		validateCodeGen("StructExternalAccess.bf", "StructExternalAccess_expected.txt");
	}

	TEST(CodeGen, Program_StructDefInheritance) {
		validateCodeGen("StructDefInheritance.bf", "StructDefInheritance_expected.txt");
	}

	TEST(CodeGen, Program_StructInheritanceInternalAccess) {
		validateCodeGen("StructInheritanceInternalAccess.bf", "StructInheritanceInternalAccess_expected.txt");
	}

	TEST(CodeGen, Program_StructInheritanceCtorCalls) {
		validateCodeGen("StructInheritanceCtorCalls.bf", "StructInheritanceCtorCalls_expected.txt");
	}

	TEST(CodeGen, Program_StructInheritanceExternalAccess) {
		validateCodeGen("StructInheritanceExternalAccess.bf", "StructInheritanceExternalAccess_expected.txt");
	}

	TEST(CodeGen, Program_StructDefVtableVptr) {
		validateCodeGen("StructDefVtableVptr.bf", "StructDefVtableVptr_expected.txt");
	}

	TEST(CodeGen, Program_VirtualStructInternalMemberAccess) {
		validateCodeGen("VirtualStructInternalMemberAccess.bf", "VirtualStructInternalMemberAccess_expected.txt");
	}

	TEST(CodeGen, Program_VirtualStructExternalMemberAccess) {
		validateCodeGen("VirtualStructExternalMemberAccess.bf", "VirtualStructExternalMemberAccess_expected.txt");
	}

	TEST(CodeGen, Program_VirtualStructMethodCalls) {
		validateCodeGen("VirtualStructMethodCalls.bf", "VirtualStructMethodCalls_expected.txt");
	}
}
