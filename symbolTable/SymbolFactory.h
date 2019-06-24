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
		virtual shared_ptr<Symbol> createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities);
		virtual unique_ptr<Symbol> createFunctionSymbol(const string& name, shared_ptr<Type> type);
		virtual shared_ptr<Symbol> createStructSymbol(const string& name);
		virtual unique_ptr<Symbol> createVariableSymbol(const string& name, shared_ptr<Type> type);
	};

}
