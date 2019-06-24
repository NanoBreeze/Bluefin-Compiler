#include "Symbol.h"

using namespace bluefin;

#include <memory>
using std::shared_ptr;
bool Symbol::operator==(Symbol& rhs)
{
	shared_ptr<Type> thisType = this->getType();
	shared_ptr<Type> rhsType = rhs.getType();
	return this->name == rhs.name
		&& this->getType()->type2str() == rhs.getType()->type2str();
	// don't use private member type b/c it could be nullptr for builtinType
	// and structType, which override getType() to return themselves.
	// b/c smart point prevent passing themselves during construction.
}
