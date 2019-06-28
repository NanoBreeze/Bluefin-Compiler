#include "Symbol.h"

using namespace bluefin;

#include <memory>
using std::shared_ptr;
bool Symbol::operator==(Symbol& rhs)
{
	shared_ptr<Type> thisType = this->getType();
	shared_ptr<Type> rhsType = rhs.getType();
	return this->name == rhs.name
		&& this->type == rhs.type;
}
