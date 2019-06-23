#pragma once

#include <string>
#include "../../symbolTable/SymbolFactory.h"

namespace SymbolTableTests {

	using namespace bluefin;

	/*
	Used to create the test wrappers around symbols. This facilitates testing them, 
	such as StructSymbol::resolveMember. 

	It accepts a string reference and passes it to the wrappers. The reason is that
	the string can be written to from every symbol, for testing purposes.
	*/
	class SymbolWrapperFactory : public SymbolFactory
	{
	public:
		SymbolWrapperFactory(string& output) : output{ output }
		{}

		 Symbol* createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities) override;
		 Symbol* createFunctionSymbol(const string& name, Type* type) override;
		 Symbol* createStructSymbol(const string& name) override;
		 Symbol* createVariableSymbol(const string& name, Type* type) override;

	private:
		string& output;
	};
}
