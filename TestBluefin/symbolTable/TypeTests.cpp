#include "pch.h"

#include "antlr4-runtime.h"
#include "../utils.h"
#include <iostream>
#include <fstream>

#include "../../symbolTable/SymbolTable.h"
#include "../../symbolTable/SymbolFactory.h"
#include "../../src/PopulateSymTabListener.h"
#include "../../src/DecorateExprWithTypes.h"

#include "PostOrderPrintType.h"
#include "SymbolTableTestWrapper.h"
#include "SymbolWrapperFactory.h"

namespace SymbolTableTests {

	using std::string;
	using std::ifstream;
	using namespace bluefin;
	using namespace antlr4;


	void validateTypeChecking(const string programFile, const string expectedOutputFile) {
		string pathPrefix = "../TestBluefin/symbolTable/programs/TypeChecking/";

		ifstream file(pathPrefix + programFile);
		tree::ParseTree* tree = createParseTree(file);

		tree::ParseTreeWalker walker;

		string dummy;
		SymbolTableTestWrapper symTab(dummy);
		SymbolWrapperFactory symFact(dummy);

		PopulateSymTabListener populateSymtabListener(symTab, symFact);
		walker.walk(&populateSymtabListener, tree);

		ParseTreeProperty<shared_ptr<Scope>> scopes = populateSymtabListener.getScopeOfPrimaryCtxs();
		DecorateExprWithTypes decorateExprListener(populateSymtabListener.getScopeOfPrimaryCtxs(), symFact);
		walker.walk(&decorateExprListener, tree);

		string symbolTypesStr;
		PostOrderPrintType printTypeListener(symbolTypesStr, decorateExprListener.getExprTypes());
		walker.walk(&printTypeListener, tree);

		string expectedOutput = readFile(pathPrefix + expectedOutputFile);
		//std::cout << "OUTPUT:" << std::endl;
		//std::cout << symbolTypesStr << std::endl;
		//std::cout << "EXPECTED:" << std::endl;
		//std::cout << expectedOutput << std::endl;
		ASSERT_EQ(symbolTypesStr, expectedOutput);
	}

	TEST(Types, Program_ExprTypesForPrimaries) {
		validateTypeChecking("ExprTypesForPrimaries.bf", "ExprTypesForPrimaries_expected.txt");
	}

	TEST(Types, Program_ExprTypesForRelationalOps) {
		validateTypeChecking("ExprTypesForRelationalOps.bf", "ExprTypesForRelationalOps_expected.txt");
	}

	TEST(Types, Program_ExprTypesForEqualityOps) {
		validateTypeChecking("ExprTypesForEqualityOps.bf", "ExprTypesForEqualityOps_expected.txt");
	}

	TEST(Types, Program_ExprTypesForArithmeticBinaryOps) {
		validateTypeChecking("ExprTypesForArithmeticBinaryOps.bf", "ExprTypesForArithmeticBinaryOps_expected.txt");
	}

	TEST(Types, Program_ExprTypesForUnaryOps) {
		validateTypeChecking("ExprTypesForUnaryOp.bf", "ExprTypesForUnaryOp_expected.txt");
	}

	TEST(Types, Program_ExprTypesForLogicalAndOr) {
		validateTypeChecking("ExprTypesForLogicalAndOr.bf", "ExprTypesForLogicalAndOr_expected.txt");
	}

	TEST(Types, Program_ExprTypesForFuncCalls) {
		validateTypeChecking("ExprTypesForFuncCalls.bf", "ExprTypesForFuncCalls_expected.txt");
	}

	TEST(Types, Program_ExprTypesForStructMemberAccess) {
		validateTypeChecking("ExprTypesForStructMemberAccess.bf", "ExprTypesForStructMemberAccess_expected.txt");
	}

	TEST(Types, Program_ExprTypesForAssignments) {
		validateTypeChecking("ExprTypesForAssignments.bf", "ExprTypesForAssignments_expected.txt");
	}




}
