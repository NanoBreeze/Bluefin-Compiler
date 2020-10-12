#include "Scope.h"
#include "./Exceptions.h"

using namespace bluefin;

void Scope::declare(shared_ptr<Symbol> symbol)
{
	// if declaration had already occured in this scope, throw an exception
	if (symbols.count(symbol->getName())) {
		throw RedeclarationException(symbol->getName());
	}

	symbols.emplace(symbol->getName(), symbol);
}

shared_ptr<Symbol> Scope::resolve(const string name) const
{
	auto it = symbols.find(name);
	if (it == symbols.end()) {
		return nullptr;
	}
	return it->second;
}

