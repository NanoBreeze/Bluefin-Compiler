#pragma once

#include "../../symbolTable/Symbol.h"
#include "../../symbolTable/StructSymbol.h"

namespace SymbolTableTests {

	using namespace bluefin;

	class StructSymbolTestWrapper : public StructSymbol
	{
		 Symbol* resolveMember(const string memberName) override; 
	};
}
