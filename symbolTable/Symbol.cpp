#include "Symbol.h"

using namespace bluefin;

#include <memory>
using std::shared_ptr;
bool Symbol::operator==(Symbol& rhs)
{
	Type thisType = this->getType();
	Type rhsType = rhs.getType();
	return this->name == rhs.name
		&& this->type == rhs.type;
}
