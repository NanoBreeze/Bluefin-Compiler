#pragma once

#include <iostream>

using std::string;

/* \brief Returns the S-expression corresponding to the AST of the program argument
*/
string getSExpression(const string program);

/* \brief Pass in syntactically incorrect program and expect an exception to be throw
* 
* \warning Expect an exception to be thrown
*/
void improperSyntax(const string program);
