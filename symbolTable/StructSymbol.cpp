#include "StructSymbol.h"
#include "Symbol.h"
#include "Exceptions.h"
#include <memory>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;

shared_ptr<Scope> StructSymbol::getParentScope() const {
	if (superClass) {
		return dynamic_pointer_cast<Scope>(superClass);
	}

	return getEnclosingScope(); 
}

size_t StructSymbol::getMemberIndex(string memName) const
{
	for (int i = 0; i < memNames.size(); i++) {
		if (memName == memNames[i]) return i;
	}

	throw StructFieldNoIndexException(memName);
	return 999;
}

// The fields are put in one by one
void StructSymbol::declare(shared_ptr<Symbol> symbol)
{
	memNames.push_back(symbol->getName());
	Scope::declare(symbol);
}
