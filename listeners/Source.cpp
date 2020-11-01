#include <iostream>

#include "antlr4-runtime.h"

#include "../generated/bluefin/bluefinLexer.h"
#include "../generated/bluefin/bluefinParser.h"

using std::cout;
using std::endl;

using namespace antlr4;

int a = 5;
float b = a = 4;

int main() {
	float a = 5;
	int c = 6;
	c = a = 3;
	cout << "Hello" << endl;

	ANTLRInputStream input2("int hello() {int b = 7; }");
	bluefin::bluefinLexer lexer2(&input2);
	CommonTokenStream tokens2(&lexer2);

	bluefin::bluefinParser parser2(&tokens2);
	tree::ParseTree* tree2 = parser2.program();

	std::wstring s2 = antlrcpp::s2ws(tree2->toStringTree(&parser2)) + L"\n";


	std::wcout << "Parse Tree 2: " << s2 << std::endl; 

	
	return 0;
}
