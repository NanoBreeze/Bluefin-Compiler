#include "pch.h"

#include "../testutils.h"
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

TEST(Struct, SingleField) {

	const string prog = "struct x { A m; };";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (varDecl (type A) m ;) } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, TwoFields) {

	const string prog = "struct x { A m; int b = 9; };";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (varDecl (type A) m ;) (varDecl (type (builtinType int)) b = (expr 9) ;) } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, Methods) {

	const string prog = R"(
	struct x { 
		void f() {} 
		void g(int a) {} 
	};
	)";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (funcDef (type (builtinType void)) f ( ) { }) (funcDef (type (builtinType void)) g ( (paramList (param (type (builtinType int)) a)) ) { }) } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, FieldsAndMethods) {

	const string prog = R"(
	struct x { 
		bool b = true;
		void f() {} 
		A a;
		void g(int a) {} 
	};
	)";
	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x { (varDecl (type (builtinType bool)) b = (expr true) ;) (funcDef (type (builtinType void)) f ( ) { }) (varDecl (type A) a ;) (funcDef (type (builtinType void)) g ( (paramList (param (type (builtinType int)) a)) ) { }) } ;))";
	EXPECT_EQ(sExpr, expected);
}

TEST(Struct, ExtendsFromParentStruct) {

	const string prog = "struct x extends y {};";

	const string sExpr = getSExpression(prog);

	const string expected = "(program (structDef struct x (superClass extends y) { } ;))";
	EXPECT_EQ(sExpr, expected);
}

/****************************************
************* Negative Tests ************
*****************************************/

TEST(Struct, DeclarationOnly) {
	const string prog = "struct A;";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, IfStatement) {
	const string prog = "struct A { if (true) {}; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, WhileStatement) {
	const string prog = "struct A { while (true) {}; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, ReturnStatement) {
	const string prog = "struct A { return a; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, BreakStatement) {
	const string prog = "struct A { break; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, ContinueStatement) {
	const string prog = "struct A { continue; };";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

TEST(Struct, ExprStatement) {
	const string prog = "struct A { f; };";
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

TEST(Struct, DoubleParents) {
	const string prog = "struct A extends B, C {};";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}

