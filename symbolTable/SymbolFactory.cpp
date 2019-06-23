#include "SymbolFactory.h"

using namespace bluefin;
Symbol* SymbolFactory::createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities pos) {
	return new BuiltinTypeSymbol(pos);
}

Symbol* SymbolFactory::createFunctionSymbol(const string& name, Type* type) {
	return new FunctionSymbol(name, type);
}

Symbol* SymbolFactory::createStructSymbol(const string& name) {
	return new StructSymbol(name);
}

Symbol* SymbolFactory::createVariableSymbol(const string& name, Type* type) {
	return new VariableSymbol(name, type);
}
