#include "pch.h"

#include "antlr4-runtime.h"
#include "../utils.h"
#include <iostream>
#include <fstream>
#include <map>

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
	using antlr4::tree::ParseTree;
	using antlr4::tree::ParseTreeWalker;
	using std::map;
	using namespace bluefin;
	using namespace antlr4;


	void validateTypeChecking(const string programFile, const string expectedOutputFile) {
		string pathPrefix = "../TestBluefin/symbolTable/programs/TypeChecking/";

		ifstream file(pathPrefix + programFile);
		ParseTree* tree = createParseTree(file);

		ParseTreeWalker walker;

		string dummy;
		SymbolTableTestWrapper symTab(dummy);
		SymbolWrapperFactory symFact(dummy);

		PopulateSymTabListener populateSymtabListener(symTab, symFact);
		walker.walk(&populateSymtabListener, tree);

		map<ParseTree*, shared_ptr<Scope>> scopes = populateSymtabListener.getScopeOfPrimaryAndFuncDefCtxs();
		DecorateExprWithTypes decorateExprListener(scopes, symFact);
		walker.walk(&decorateExprListener, tree);

		string symbolTypesStr;
		PostOrderPrintType printTypeListener(symbolTypesStr, decorateExprListener.getTypeContexts());
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

	TEST(Types, Program_ExprTypesForStructMembers) {
		validateTypeChecking("ExprTypesForStructMembers.bf", "ExprTypesForStructMembers_expected.txt");
	}

	TEST(Types, Program_ExprTypesForAssignments) {
		validateTypeChecking("ExprTypesForAssignments.bf", "ExprTypesForAssignments_expected.txt");
	}

	TEST(Types, Program_PromotionTypeForVarDecl) {
		validateTypeChecking("PromotionTypeForVarDecl.bf", "PromotionTypeForVarDecl_expected.txt");
	}

	TEST(Types, Program_PromotionTypeForFuncCallArgs) {
		validateTypeChecking("PromotionTypeForFuncCallArgs.bf", "PromotionTypeForFuncCallArgs_expected.txt");
	}

	TEST(Types, Program_ExprTypesForReturnStmt) {
		validateTypeChecking("ExprTypesForReturnStmt.bf", "ExprTypesForReturnStmt_expected.txt");
	}

}
