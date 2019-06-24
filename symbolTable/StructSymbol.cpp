#include "StructSymbol.h"

using namespace bluefin;

void StructSymbol::attachScope(shared_ptr<Scope> scope)
{
	this->scope = scope;
}

shared_ptr<Symbol> StructSymbol::resolveMember(const string memberName)
{
	return scope->resolve(memberName);
}
