#include "Scope.h"

using namespace bluefin;

void Scope::declare(const string name, Symbol* symbol)
{
	symbols[name] = symbol;
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
