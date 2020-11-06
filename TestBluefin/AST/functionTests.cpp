#include "pch.h"

#include "../testutils.h"
#include <iostream>
#include <exception>

#include "Exceptions.h"

using std::string;
using antlr4::ParseCancellationException;


TEST(Function, NoParamBuiltinReturn) {

	const string prog = "void hi() {}";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (funcDef (type (builtinType void)) hi ( ) { }))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Function, NoParamNotBuiltinReturn) {

	const string prog = "Hi hi() {}";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (funcDef (type Hi) hi ( ) { }))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Function, SingleBuiltinParam) {

	const string prog = "Hi hi(int a) {}";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (funcDef (type Hi) hi ( (paramList (param (type (builtinType int)) a)) ) { }))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Function, DoubleParam) {

	const string prog = "Hi hi(int a, float b) {}";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (funcDef (type Hi) hi ( (paramList (param (type (builtinType int)) a) , (param (type (builtinType float)) b)) ) { }))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Function, WithDefinition) {

	const string prog = "Hi hi() { A x; }";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (funcDef (type Hi) hi ( ) { (stmt (varDecl (type A) x ;)) }))";
	EXPECT_EQ(sExpr, expected);
}

/****************************************
************* Negative Tests ************
*****************************************/

TEST(Function, DeclarationOnly) {
	const string prog = "Hi hi();";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Function, ParamWithoutType) {
	const string prog = "Hi hi(x) {}";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Function, TwoParamsWithoutComma) {
	const string prog = "Hi hi(int x int y) {}";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Function, InvalidKeywordAsReturnType) {
	const string prog = "true hi() {}";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Function, NumberAsReturnType) {
	const string prog = "42 hi() {}";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

