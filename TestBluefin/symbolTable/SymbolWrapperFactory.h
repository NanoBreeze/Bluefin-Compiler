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

		 unique_ptr<Symbol> createStructSymbol(const string& name, shared_ptr<Scope> enclosingScope, size_t tokenIndex=0, shared_ptr<StructSymbol> superClass=nullptr) override;

	private:
		string& output;
	};
}
