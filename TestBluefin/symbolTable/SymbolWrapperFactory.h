#pragma once

#include <memory>
#include <string>
#include "../../symbolTable/SymbolFactory.h"

namespace SymbolTableTests {

	using namespace bluefin;
	using std::shared_ptr;
	using std::unique_ptr;

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

		 shared_ptr<Symbol> createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities) override;
		 unique_ptr<Symbol> createFunctionSymbol(const string& name, shared_ptr<Type> type) override;
		 shared_ptr<Symbol> createStructSymbol(const string& name) override;
		 unique_ptr<Symbol> createVariableSymbol(const string& name, shared_ptr<Type> type) override;

	private:
		string& output;
	};
}