#pragma once
#include "Type.h"

namespace bluefin {

	class BuiltinType : public Type
	{
	public:
		BuiltinType(const string typeName) : Type{ typeName } //TODO: use enum for typename?
		{ } 

	};
}
