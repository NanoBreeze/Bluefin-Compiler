#pragma once

#include <iostream>
#include "Symbol.h"

namespace bluefin {

	using std::string;

	class VariableSymbol : public Symbol {

	public:
		VariableSymbol(const string& name, Type* type) :
			Symbol(name, type) {}

		inline string getCategoryName() const override { return "var"; }
	};
}
