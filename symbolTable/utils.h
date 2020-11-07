#include <memory>
#include <exception>
#include <vector>
#include "SymbolTable.h"
#include "StructSymbol.h"
#include "VariableSymbol.h"
#include "FunctionSymbol.h"
#include "BuiltinTypeSymbol.h"

namespace bluefin {

	// A FuncDefContext can refer to an actual function definition, or a function definition
	// inside a struct (aka, a method). This matters a lot for code generation, since a method contains
	// an extra "this" parameter
	bool isStructMethod(bluefinParser::FuncDefContext* ctx, SymbolTable& symTab);

	// A FunctionSymbol can be either a real global function, or it could be inside a struct (thus, a method)
	bool isStructMethod(shared_ptr<FunctionSymbol>, SymbolTable& symTab);

	// Given a func def that's a method (inside a struct), retrieve the struct
	// Returns null if not inside a struct. Somewhat duplicates isStructMethod(..)
	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable);

	// A VarDecl may be at global scope, local scope (local variable), or in a StructSymbol (field)
	bool isStructField(bluefinParser::VarDeclContext* ctx, SymbolTable& symTab);

	// Given a VarDecl that's inside a struct (so it's a field), get the containing struct
	// Returns null if not inside a struct. Somewhat duplicates isStructMethod(..)
	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable);
}