#include "StructSymbol.h"
#include "VariableSymbol.h"
#include "FunctionSymbol.h"
#include "Symbol.h"
#include "Exceptions.h"
#include <assert.h>
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

size_t StructSymbol::getFieldIndex(string fieldName) const
{
	for (int i = 0; i < fields.size(); i++) {
		if (fieldName == fields[i]->getName()) return i;
	}

	throw StructFieldNoIndexException(fieldName);
	return 999;
}

// The fields are put in one by one. Only keep track of the fields (not methods) since we need those later for codegen
void StructSymbol::declare(shared_ptr<Symbol> symbol)
{
	shared_ptr<VariableSymbol> field = dynamic_pointer_cast<VariableSymbol>(symbol);
	if (field)
		fields.push_back(field);
	else {
		shared_ptr<FunctionSymbol> method = dynamic_pointer_cast<FunctionSymbol>(symbol);
		assert(method != nullptr);
		methods.push_back(method);
	}

	Scope::declare(symbol);
}
