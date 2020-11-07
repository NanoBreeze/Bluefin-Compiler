#include "utils.h"
#include "SymbolTable.h"
#include "StructSymbol.h"

using namespace bluefin;
using std::dynamic_pointer_cast;

namespace bluefin {
	// If the containing scope of the ctx is a StructSymbol, then this funcDef is inside a struct, aka, it's a method
	bool isStructMethod(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(ctx));
		return structSym != nullptr;
	}

	bool isStructMethod(shared_ptr<FunctionSymbol> funcSym, SymbolTable& symbolTable) {
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(funcSym));
		return structSym != nullptr;
	}

	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable)
	{
		return dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(ctx));
	}

	bool isStructField(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(varDeclScope);

		return structSym != nullptr; // if containing scope, when casted to structSymbol, is not null, then it is a structSymbol!
	}

	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);
		return dynamic_pointer_cast<StructSymbol>(varDeclScope);
	}

	vector<shared_ptr<VariableSymbol>> getAllStructFields(shared_ptr<StructSymbol> structSym) {
		// Since some some the structDef's varDecl's contain assignment, we create a ctor for every struct def
		// so that those assignment expressions can go into the ctor function

		vector<shared_ptr<VariableSymbol>> fields;

		shared_ptr<Scope> scope = dynamic_pointer_cast<Scope>(structSym);
		assert(scope != nullptr);

		unordered_map<string, shared_ptr<Symbol>> syms = scope->getSymbols();

		for (auto pair : syms) { // exclude all the methods
			shared_ptr<Symbol> sym = pair.second;
			if (dynamic_pointer_cast<VariableSymbol>(sym))
				fields.push_back(dynamic_pointer_cast<VariableSymbol>(sym));
		}
		return fields;
	}

	vector<shared_ptr<FunctionSymbol>> getAllStructMethods(shared_ptr<StructSymbol> structSym) {
		vector<shared_ptr<FunctionSymbol>> methods;

		shared_ptr<Scope> scope = dynamic_pointer_cast<Scope>(structSym);
		assert(scope != nullptr);

		unordered_map<string, shared_ptr<Symbol>> syms = scope->getSymbols();

		for (auto pair : syms) { // exclude all the methods
			shared_ptr<Symbol> sym = pair.second;
			if (dynamic_pointer_cast<FunctionSymbol>(sym))
				methods.push_back(dynamic_pointer_cast<FunctionSymbol>(sym));
		}
		return methods;
	}
}
