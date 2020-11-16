#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "Symbol.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;
	using std::vector;

	/* TODO: should this contain its own arguments?
	 So I imagine a call to SymbolTable::enterScope will occur on method decalaration.
	 This means that function param will be in the same scope as the local variables.
	 TODO: need to remove declaration scope from Standard
	*/
	class FunctionSymbol : public Symbol {
	public:

		FunctionSymbol(const string& name, Type type, bool isVirtual, bool isOverride, size_t tokenIndex) :
			Symbol(name, type, tokenIndex), isFunctionVirtual{ isVirtual }, isFunctionOverride {isOverride}
		{}

		void attachParam(shared_ptr<Symbol>); 

		inline vector<shared_ptr<Symbol>> getParams() const { return params; }

		inline bool isVirtual() const { return isFunctionVirtual; }

		inline bool isOverride() const { return isFunctionOverride; }

	private:
		vector<shared_ptr<Symbol>> params;
		bool isFunctionVirtual;
		bool isFunctionOverride;
	};
}
