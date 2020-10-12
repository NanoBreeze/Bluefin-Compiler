#include "StructSymbol.h"
#include "Symbol.h"
#include <memory>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;

shared_ptr<Symbol> StructSymbol::resolve(const string memberName) const
{
	shared_ptr<Symbol> member = Scope::resolve(memberName);
	if (!member && superClass) {
		return superClass->resolve(memberName);
	}
	return member;
}

shared_ptr<Scope> StructSymbol::getParentScope() const {
	if (superClass) {
		return dynamic_pointer_cast<Scope>(superClass);
	}

	return getEnclosingScope(); 
}
