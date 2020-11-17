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

	// Given a functionSym/method, get the containing struct. This is so that when calling the method,
	// we know which ptr type to cast the address. 
	shared_ptr<StructSymbol> getContainingStruct(shared_ptr<FunctionSymbol> funcSym, SymbolTable& symbolTable);

	// Given a func def that's a method (inside a struct), retrieve the struct
	// Returns null if not inside a struct. Somewhat duplicates isStructMethod(..)
	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable);

	// A VarDecl may be at global scope, local scope (local variable), or in a StructSymbol (field)
	bool isStructField(bluefinParser::VarDeclContext* ctx, SymbolTable& symTab);

	// Checks if the VarDecl is at global scope
	bool isGlobalVarDecl(bluefinParser::VarDeclContext* ctx, SymbolTable& symTab);

	// Checks if the VarDecl is at local scope
	bool isLocalVarDecl(bluefinParser::VarDeclContext* ctx, SymbolTable& symTab);

	// Given a VarDecl that's inside a struct (so it's a field), get the containing struct
	// Returns null if not inside a struct. Somewhat duplicates isStructMethod(..)
	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable);

	// Given the structSym, search in its class hierarchy for the closest override method that corresponds to every virtual method
	vector<shared_ptr<FunctionSymbol>> getVTableMethods(shared_ptr<StructSymbol> structSym);

	bool shouldLLVMStructTypeContainExplicitVPtr(const shared_ptr<StructSymbol> structSym);

	size_t getVTableMethodIndex(shared_ptr<StructSymbol> structSym, string methodName);
}