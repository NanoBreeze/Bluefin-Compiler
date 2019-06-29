#pragma once

#include "../../symbolTable/Symbol.h"
#include "../../symbolTable/StructSymbol.h"

namespace SymbolTableTests {

	using namespace bluefin;

	class StructSymbolTestWrapper : public StructSymbol
	{
	public:
		StructSymbolTestWrapper(const string& name, string& output, shared_ptr<Scope> enclosingScope) :
			StructSymbol(name, enclosingScope), output{ output }
		{}

		/*
		Prints out:
		resolve - StructTypeName::member - c_var - t_tmemberType
		or
		resolve - StructTypeName::member - UNRESOLVED
		*/
		 shared_ptr<Symbol> resolveMember(const string memberName)  override; 

	private:
		string& output;
		string createResolveMemberDebugMsg(shared_ptr<Symbol> resolvedSym) const;
		string createUnresolvedMemberDebugMsg(string unresolvedMemberName) const;
	};
}
