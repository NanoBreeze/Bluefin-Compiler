#include "SymbolFactory.h"
#include <utility>
#include <memory>

using std::move;
using std::unique_ptr;
using std::shared_ptr;

using namespace bluefin;

// we return a shared_ptr instead of a unique_ptr b/c BuiltinTypeSymbol::getType() uses shared_from_this, so 
// returning a shared_ptr guarantees it will work
unique_ptr<Symbol> SymbolFactory::createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType pos) {
	return unique_ptr<Symbol>(new BuiltinTypeSymbol(pos));
}

unique_ptr<Symbol> SymbolFactory::createFunctionSymbol(const string& name, shared_ptr<Type> type) {
	return unique_ptr<Symbol>(new FunctionSymbol(name, type));
}

unique_ptr<Symbol> SymbolFactory::createStructSymbol(const string& name) {
	return unique_ptr<Symbol>(new StructSymbol(name));
}

unique_ptr<Symbol> SymbolFactory::createVariableSymbol(const string& name, shared_ptr<Type> type) {
	return unique_ptr<Symbol>(new VariableSymbol(name, type));
}
