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

	string pathPrefix = "../TestBluefin/symbolTable/programs/";

	void validateProgram(const string programFile, const string expectedOutputFile) {

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

	TEST(SymbolTable, Program_BuiltinDeclaration) {
		validateProgram("BuiltinDeclaration.bf", "BuiltinDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_DeclarationInNestedScopes) {
		validateProgram("DeclarationInNestedScopes.bf", "DeclarationInNestedScopes_expected.txt");
	}
	TEST(SymbolTable, Program_FunctionDeclaration) {
		validateProgram("FunctionDeclaration.bf", "FunctionDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_InvalidAndUnresolvedDeclaration) {
		validateProgram("InvalidAndUnresolvedDeclaration.bf", "InvalidAndUnresolvedDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_MixedDeclaration) {
		//MixedDeclaration
		validateProgram("MixedDeclaration.bf", "MixedDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_SameDeclarationInSameScope) {
		validateProgram("SameDeclarationsInSameScope.bf", "SameDeclarationsInSameScope_expected.txt");
	}
	TEST(SymbolTable, Program_StructDeclaration) {
		validateProgram("StructDeclaration.bf", "StructDeclaration_expected.txt");
	}
	TEST(SymbolTable, Program_ResolvePrimaryIdInExpr) {
		validateProgram("ResolvePrimaryIdInExpr.bf", "ResolvePrimaryIdInExpr_expected.txt");
	}

	TEST(SymbolTable, Program_ResolveStructMembers) {
		validateProgram("ResolveStructMembers.bf","ResolveStructMembers_expected.txt");
	}

	TEST(SymbolTable, Program_ResolveStructMembersForwardReference) {
		validateProgram("ResolveStructMembersForwardReference.bf","ResolveStructMembersForwardReference_expected.txt");
	}

	TEST(SymbolTable, Program_InvalidForwardReferences) {
		validateProgram("InvalidForwardReferences.bf", "InvalidForwardReferences_expected.txt");
	}
}
