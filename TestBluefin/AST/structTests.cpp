#include "pch.h"

#include "../utils.h"
#include <iostream>
#include "Exceptions.h"

using std::string;
using antlr4::ParseCancellationException;

TEST(Struct, EmptyDefintion) {

	const string prog = "struct x {};";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, SingleMember) {

	const string prog = "struct x { A m; };";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (varDecl (type A) m ;) } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, TwoMembers) {

	const string prog = "struct x { A m; int b; };";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (varDecl (type A) m ;) (varDecl (type (builtinType int)) b ;) } ;))";
	EXPECT_EQ(sExpr, expected);
}

/****************************************
************* Negative Tests ************
*****************************************/

TEST(Struct, DeclarationOnly) {
	const string prog = "struct A;";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, NestedFunctions) {
	const string prog = "struct A { void hi() {} };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, NestedStructDefinition) {
	const string prog = "struct A { struct B { }; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, NestedBlock) {
	const string prog = "struct A { { } };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

