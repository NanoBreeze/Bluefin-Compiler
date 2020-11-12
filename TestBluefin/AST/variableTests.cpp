#include "pch.h"

#include "../testutils.h"
#include <iostream>
#include "Exceptions.h"

using std::string;
using antlr4::ParseCancellationException;

TEST(Variable, WithoutAssignment) {

	const string prog = "Wow x;";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (varDecl (type Wow) x ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Variable, BuiltinWithoutAssignment) {

	const string prog = "int x;";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (varDecl (type (builtinType int)) x ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Variable, WithAssignment) {

	const string prog = "Wow x = 87;";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (varDecl (type Wow) x = (expr 87) ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Variable, BuiltinWithAssignment) {

	const string prog = "int x = 87;";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (varDecl (type (builtinType int)) x = (expr 87) ;))";
	EXPECT_EQ(sExpr, expected);
}

/****************************************
************* Negative Tests ************
*****************************************/

TEST(Variable, NoType) {
	const string prog = "a = b;";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Variable, MissingRHS) {
	const string prog = "int a = ;";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Variable, MissingSemicolon) {
	const string prog = "int a = 5";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Variable, NameAsKeyword) {
	const string prog = "int void = 1;";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}


