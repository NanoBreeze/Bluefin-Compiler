#include "Scope.h"
#include "./Exceptions.h"

using namespace bluefin;

void Scope::declare(Symbol* symbol)
{
	// if definition already exists, throw an exception
	if (symbols.count(symbol->getName())) {
		throw RedefinitionException(symbol->getName());
	}

	symbols.emplace(symbol->getName(), symbol);
}

Symbol* Scope::resolve(string name) const
{
	auto it = symbols.find(name);
	if (it == symbols.end()) {
		return nullptr;
	}
	return it->second;
}

Scope* Scope::getEnclosingScope() const
{
	return enclosingScope;
}
