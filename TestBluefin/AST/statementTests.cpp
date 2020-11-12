#include "pch.h"

#include "../testutils.h"
#include <iostream>
#include "Exceptions.h"

using std::string;
using antlr4::ParseCancellationException;

TEST(Statement, IfNoElse) {

	const string prog = R"(
	int main() { 
		if (57) {} 
	}
	)";
	
	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtIf if ( (expr 57) ) (block { })))";
	const string footer = " }))";
	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, IfWithElse) {

	const string prog = R"(
	int main() { 
		if (57) {}
		else {} 
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtIf if ( (expr 57) ) (block { }) else (block { })))";
	const string footer = " }))";
	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, While) {

	const string prog = R"(
	int main() { 
		while (57) {}
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtWhile while ( (expr 57) ) (block { })))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, ReturnExpression) {

	const string prog = R"(
	int main() { 
		return 57;
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtReturn return (expr 57) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, ReturnNoExpr) {

	const string prog = R"(
	int main() { 
		return; 
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtReturn return ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, Break) {

	const string prog = R"(
	int main() { 
		while (57) {
			break;
		}
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtWhile while ( (expr 57) ) (block { (stmt (stmtBreak break ;)) })))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, Continue) {

	const string prog = R"(
	int main() { 
		while (57) {
			continue;
		}
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtWhile while ( (expr 57) ) (block { (stmt (stmtContinue continue ;)) })))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, BlockEmpty) {

	const string prog = R"(
	int main() { 
		{}
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (block { }))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, BlockWithStatement) {

	const string prog = R"(
	int main() { 
		{
			A m;
		}
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (block { (stmt (varDecl (type A) m ;)) }))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, StatementExpr) {

	const string prog = R"(
	int main() { 
		99;
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr 99) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, ExpressionPrecedence) {

	const string prog = R"(
	int main() { 
		-1 * 2 / 3 + 4 -5 + (6 - 7) >= 3 == 9 || true && false;
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr (expr (expr (expr (expr (expr (expr (expr (expr - (expr 1)) * (expr 2)) / (expr 3)) + (expr 4)) - (expr 5)) + (expr ( (expr (expr 6) - (expr 7)) ))) >= (expr 3)) == (expr 9)) || (expr (expr true) && (expr false))) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, ExpressionRightAssociativity) {
	// Tests right associativity of =

	const string prog = R"(
	int main() { 
		a = b = c;		
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr (expr a) = (expr (expr b) = (expr c))) ;))";
	const string footer = " }))";


	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, FunctionCallWithNoArg) {
	// Tests right associativity of =

	const string prog = R"(
	int main() { 
		f();
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr f ( )) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, FunctionCallWithOneArg) {
	// Tests right associativity of =

	const string prog = R"(
	int main() { 
		f(a+b);
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr f ( (argList (expr (expr a) + (expr b))) )) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

TEST(Statement, FunctionCallWithTwoArgs) {
	// Tests right associativity of =

	const string prog = R"(
	int main() { 
		f(5, g());
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr f ( (argList (expr 5) , (expr g ( ))) )) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}
TEST(Statement, StructMemberAccess) {
	// Tests right associativity of =

	const string prog = R"(
	int main() { 
		a.b.c;
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (stmtExpr (expr (expr (expr a) . b) . c) ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}



TEST(Statement, VariableDecl) {

	const string prog = R"(
	int main() { 
		int a;
	}
	)";

	const string sExpr = getSExpression(prog);

	const string header = "(program (funcDef (type (builtinType int)) main ( ) { ";
	const string expected = "(stmt (varDecl (type (builtinType int)) a ;))";
	const string footer = " }))";

	EXPECT_EQ(sExpr, header + expected + footer);
}

/****************************************
************* Negative Tests ************
*****************************************/

TEST(Statement, MissingSemicolon) {
	const string prog = "int main() { int j }";
	EXPECT_THROW(improperSyntax(prog), ParseCancellationException);
}
