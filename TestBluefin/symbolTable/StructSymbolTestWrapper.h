#pragma once

#include "../../symbolTable/Symbol.h"
#include "../../symbolTable/StructSymbol.h"

namespace SymbolTableTests {

	using namespace bluefin;

	class StructSymbolTestWrapper : public StructSymbol
	{
	public:
		StructSymbolTestWrapper(const string& name, string& output) :
			StructSymbol(name), output{ output }
		{}

		/*
		Prints out:
		resolve - StructTypeName::member - c_var - t_tmemberType
		or
		resolve - StructTypeName::member - UNRESOLVED
		*/
		 Symbol* resolveMember(const string memberName) override; 

	private:
		string& output;
		string createResolveMemberDebugMsg(Symbol const* resolvedSym) const;
		string createUnresolvedMemberDebugMsg(string unresolvedMemberName) const;
	};
}
