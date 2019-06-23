#pragma once

#include "BuiltinTypeSymbol.h"
#include "FunctionSymbol.h"
#include "StructSymbol.h"
#include "VariableSymbol.h"
#include "Symbol.h"

namespace bluefin {

	class SymbolFactory
	{
	public:
		virtual Symbol* createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities);
		virtual Symbol* createFunctionSymbol(const string& name, Type* type);
		virtual Symbol* createStructSymbol(const string& name);
		virtual Symbol* createVariableSymbol(const string& name, Type* type);
	};

}
