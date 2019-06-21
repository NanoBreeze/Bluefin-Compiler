#include "Symbol.h"

using namespace bluefin;

bool Symbol::operator==(Symbol& rhs)
{
	return this->name == rhs.name;
		// TODO: how to compare type? this->type == rhs.type;
}
