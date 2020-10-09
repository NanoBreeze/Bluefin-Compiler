#pragma once

#include <utility>
#include <memory>
#include <iostream>
#include "Symbol.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;

	class VariableSymbol : public Symbol {

	public:
		VariableSymbol(const string& name, Type type, size_t tokenIndex) :
			Symbol(name, type, tokenIndex) 
		{}
	};
}
