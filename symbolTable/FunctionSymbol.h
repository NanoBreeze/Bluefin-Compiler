#pragma once

#include <iostream>
#include "Symbol.h"

namespace bluefin {

	using std::string;

	class FunctionSymbol : public Symbol {
	public:

		FunctionSymbol(const string& name, const Type type) :
			Symbol(name, type) 
		{}
	};
}
