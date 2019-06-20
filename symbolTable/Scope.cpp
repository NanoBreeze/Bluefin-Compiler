#include "Scope.h"
#include "./Exceptions.h"

using namespace bluefin;

void Scope::declare(const string name, Symbol* symbol)
{
	// if definition already exists, throw an exception
	if (symbols.count(name)) {
		throw RedefinitionException(name);
	}

	symbols.emplace(name, symbol);
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
