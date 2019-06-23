#include "pch.h"
#include "StructSymbolTestWrapper.h"

using namespace bluefin;
using namespace SymbolTableTests;

Symbol* StructSymbolTestWrapper::resolveMember(const string memberName)
{
	Symbol* resolvedSym = StructSymbol::resolveMember(memberName);
	return nullptr;
}
