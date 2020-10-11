#pragma once

#include <memory>
#include <iostream>
#include "Symbol.h"
#include "Scope.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;

	/** 
	Might be circular reference to have a sp to enclosing scope. Consider using wp
	*/
	class StructSymbol : public Symbol, public Scope
	{

	public:
		StructSymbol(const string& name, shared_ptr<Scope> enclosingScope, size_t tokenIndex, shared_ptr<StructSymbol> superClass=nullptr) : 
			Symbol(name, Type{ name }, tokenIndex), Scope{ enclosingScope, name }, superClass{ superClass }
		{}


		/*
		TODO: Technically, this is not needed
		but if we use resolve, then structSymbolTestWrapper's resolve
		message will also be rpinted when symbol table searches
		So this is used to temporarily not mess up other tests
		*/
		virtual shared_ptr<Symbol> resolveMember(const string memberName) {// virtual to allow testing, so test can write
			shared_ptr<Symbol> member = resolve(memberName);
			if (!member && superClass) {
				return superClass->resolveMember(memberName);
			}
			return member;
		}

		shared_ptr<Scope> getParentScope() const override;

		inline shared_ptr<StructSymbol> getSuperClass() const { return superClass; }

	private:
		shared_ptr<StructSymbol> superClass;

	};

}
