#pragma once

#include <iostream>
#include <istream>
#include <memory>
#include "../generated/bluefin/bluefinParser.h"
#include "../symbolTable/Symbol.h"

#include "antlr4-runtime.h"

using std::string;
using std::shared_ptr;

/* \brief Returns the S-expression corresponding to the AST of the program argument
*/
string getSExpression(const string program);

/* \brief Pass in syntactically incorrect program and expect an exception to be throw
* 
* \warning Expect an exception to be thrown
*/
void improperSyntax(const string program);

namespace SymbolTableTests {

	/* 
	We don't return the parse tree (ParseTree*), which comes from 
	parser.program(), because the lifetime of the ParseTree is governed 
	by the Parser. Once the parser goes out of scope, so too does the 
	ParseTree. This means that where we use the ParseTree*, we must keep
	an active reference to the Parser that created it.
	*/
	antlr4::tree::ParseTree* createParseTree(std::istream&);

	string readFile(const string filePath);

	// eg, function, var, struct, and builtinType
	string getSymbolCategory(shared_ptr<bluefin::Symbol> symbol);
}
