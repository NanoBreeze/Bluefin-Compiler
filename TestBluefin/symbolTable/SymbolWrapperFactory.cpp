#include "pch.h"
#include "SymbolWrapperFactory.h"
#include "StructSymbolTestWrapper.h"

#include <memory>

using namespace SymbolTableTests;
using namespace bluefin;
using std::shared_ptr;
using std::unique_ptr;

unique_ptr<Symbol> SymbolWrapperFactory::createStructSymbol(const string& name, shared_ptr<Scope> enclosingScope, shared_ptr<StructSymbol> superClass) {
	return unique_ptr<Symbol>(new StructSymbolTestWrapper(name, output, enclosingScope, superClass));
}
