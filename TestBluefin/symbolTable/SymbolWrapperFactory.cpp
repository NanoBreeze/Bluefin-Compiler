#include "pch.h"
#include "SymbolWrapperFactory.h"
#include "StructSymbolTestWrapper.h"

using namespace SymbolTableTests;
using namespace bluefin;

Symbol* SymbolWrapperFactory::createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities pos) {
	return new BuiltinTypeSymbol(pos);
}

Symbol* SymbolWrapperFactory::createFunctionSymbol(const string& name, Type* type) {
	return new FunctionSymbol(name, type);
}

Symbol* SymbolWrapperFactory::createStructSymbol(const string& name) {
	return new StructSymbolTestWrapper(name, output);
}

Symbol* SymbolWrapperFactory::createVariableSymbol(const string& name, Type* type) {
	return new VariableSymbol(name, type);
}
