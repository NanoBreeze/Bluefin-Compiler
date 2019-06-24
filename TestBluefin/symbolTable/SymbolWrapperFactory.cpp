#include "pch.h"
#include "SymbolWrapperFactory.h"
#include "StructSymbolTestWrapper.h"

#include <memory>

using namespace SymbolTableTests;
using namespace bluefin;
using std::shared_ptr;
using std::unique_ptr;

shared_ptr<Symbol> SymbolWrapperFactory::createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities pos) {
	return shared_ptr<Symbol>(new BuiltinTypeSymbol(pos));
}

unique_ptr<Symbol> SymbolWrapperFactory::createFunctionSymbol(const string& name, shared_ptr<Type> type) {
	return unique_ptr<Symbol>(new FunctionSymbol(name, type));
}

shared_ptr<Symbol> SymbolWrapperFactory::createStructSymbol(const string& name) {
	return shared_ptr<Symbol>(new StructSymbolTestWrapper(name, output));
}

unique_ptr<Symbol> SymbolWrapperFactory::createVariableSymbol(const string& name, shared_ptr<Type> type) {
	return unique_ptr<Symbol>(new VariableSymbol(name, type));
}
