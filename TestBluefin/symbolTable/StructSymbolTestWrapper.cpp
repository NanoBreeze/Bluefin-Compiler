#include "pch.h"
#include "StructSymbolTestWrapper.h"
#include "utils.h"

#include <memory>

using namespace bluefin;
using namespace SymbolTableTests;
using std::shared_ptr;

shared_ptr<Symbol> StructSymbolTestWrapper::resolveMember(const string memberName)
{
	shared_ptr<Symbol> resolvedSym = StructSymbol::resolveMember(memberName);
	if (resolvedSym) {
		output += createResolveMemberDebugMsg(resolvedSym);
	}
	else {
		output += createUnresolvedMemberDebugMsg(memberName);
	}
	return resolvedSym;
}

string StructSymbolTestWrapper::createResolveMemberDebugMsg(shared_ptr<Symbol> resolvedSym) const
{
	const string structTypeName = getType().type2str();
	const string resolvedSymName = resolvedSym->getName();
	const string symCategory = getSymbolCategory(resolvedSym);;
	const string symType = resolvedSym->getType().type2str();

	return "resolve - " + structTypeName + "::" + resolvedSymName + " - c_" + symCategory + " - t_" + symType + "\n";

}

string StructSymbolTestWrapper::createUnresolvedMemberDebugMsg(string unresolvedMemberName) const {

	const string structTypeName = getType().type2str();

	return "resolve - " + structTypeName + "::" + unresolvedMemberName + " - " "UNRESOLVED\n";
}
