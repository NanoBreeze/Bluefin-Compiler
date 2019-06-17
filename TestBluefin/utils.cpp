#include "pch.h"
#include <memory>
#include "antlr4-runtime.h"

#include "utils.h"
#include "../generated/bluefin/bluefinLexer.h"
#include "../generated/bluefin/bluefinParser.h"

using namespace antlr4;
using std::make_shared;

string getSExpression(const string program) {
	
	ANTLRInputStream input(program);
	
	bluefin::bluefinLexer lexer(&input);
	CommonTokenStream tokens(&lexer);

	bluefin::bluefinParser parser(&tokens);
	tree::ParseTree* tree = parser.program();

	return tree->toStringTree(&parser);
}

void improperSyntax(const string program) {

	ANTLRInputStream input(program);
	
	bluefin::bluefinLexer lexer(&input);
	CommonTokenStream tokens(&lexer);

	bluefin::bluefinParser parser(&tokens);
	parser.setErrorHandler(make_shared<BailErrorStrategy>()); 
	tree::ParseTree* tree = parser.program(); // expect exception to be thrown
}
