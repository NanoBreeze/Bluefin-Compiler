#pragma once

#include <iostream>
#include "Symbol.h"

namespace bluefin {

	using std::string;

	class VariableSymbol : public Symbol {
	public:

		VariableSymbol(const string& name, const Type type) :
			Symbol(name, type) {}
	};
}
