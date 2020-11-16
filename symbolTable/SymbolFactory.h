#pragma once

#include <memory>
#include "BuiltinTypeSymbol.h"
#include "FunctionSymbol.h"
#include "StructSymbol.h"
#include "VariableSymbol.h"
#include "Symbol.h"

namespace bluefin {

	using std::unique_ptr;
	using std::shared_ptr;

	class SymbolFactory
	{
	public:
		enum class Builtin {
			INT, FLOAT, STRING, BOOL, VOID 
		};

		virtual shared_ptr<Symbol> createBuiltinTypeSymbol(Builtin builtin);
		virtual unique_ptr<Symbol> createFunctionSymbol(const string& name, Type type, size_t tokenIndex, bool isVirtual=false, bool isOverride=false);
		virtual unique_ptr<Symbol> createStructSymbol(const string& name, shared_ptr<Scope> enclosingScope, size_t tokenIndex, shared_ptr<StructSymbol> superClass=nullptr);
		virtual unique_ptr<Symbol> createVariableSymbol(const string& name, Type type, size_t tokenIndex);
	};

}
