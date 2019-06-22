#include "pch.h"
#include <memory>
#include "antlr4-runtime.h"

#include "utils.h"
#include "../generated/bluefin/bluefinLexer.h"
#include "../generated/bluefin/bluefinParser.h"

using namespace antlr4;
using namespace std;
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

namespace SymbolTableTests {

	using namespace bluefin;

	// Memory leaks! But that's okay b/c these are used just for tests
	tree::ParseTree* createParseTree(std::istream& is) {

		ANTLRInputStream* input = new ANTLRInputStream(is);

		bluefinLexer* lexer = new bluefinLexer(input);
		CommonTokenStream* tokens = new CommonTokenStream(lexer);

		bluefinParser* parser = new bluefinParser(tokens);
		parser->setErrorHandler(make_shared<BailErrorStrategy>()); 
		return parser->program();
	}

	string readFile(const string filePath)
	{
		std::ifstream file(filePath);
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
}
