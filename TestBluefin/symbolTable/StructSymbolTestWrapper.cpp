#include "pch.h"
#include "StructSymbolTestWrapper.h"

using namespace bluefin;
using namespace SymbolTableTests;

Symbol* StructSymbolTestWrapper::resolveMember(const string memberName)
{
	Symbol* resolvedSym = StructSymbol::resolveMember(memberName);
	if (resolvedSym) {
		output += createResolveMemberDebugMsg(resolvedSym);
	}
	else {
		output += createUnresolvedMemberDebugMsg(memberName);
	}
	return resolvedSym;
}

string StructSymbolTestWrapper::createResolveMemberDebugMsg(Symbol const* resolvedSym) const
{
	const string structTypeName = type2str();
	const string resolvedSymName = resolvedSym->getName();
	const string symCategory = resolvedSym->getCategoryName();
	const string symType = resolvedSym->getType()->type2str();

	return "resolve - " + structTypeName + "::" + resolvedSymName + " - c_" + symCategory + " - t_" + symType + "\n";

}

string StructSymbolTestWrapper::createUnresolvedMemberDebugMsg(string unresolvedMemberName) const {

	const string structTypeName = type2str();

	return "resolve - " + structTypeName + "::" + unresolvedMemberName + " - " "UNRESOLVED\n";
}
