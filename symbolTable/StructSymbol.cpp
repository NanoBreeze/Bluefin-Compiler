#include "StructSymbol.h"

using namespace bluefin;

void StructSymbol::attachScope(Scope* scope)
{
	this->scope = scope;
}

Symbol* StructSymbol::resolveMember(const string memberName)
{
	return scope->resolve(memberName);
}
