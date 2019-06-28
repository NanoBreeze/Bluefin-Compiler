#pragma once

#include <iostream>

namespace bluefin {

	using std::string;

	/*
	Only BuiltinTypeSymbol and StructSymbol inherit from this.
	They already have names, eg, "int", "float", "First", 
	This "Type" is mostly used as a tag. However, it does 
	contain a toString method, which returns the same as the Symbol's name,
	in order to print the type. Used mostly for type checking testing

	There are 5 default types: int, float, string, bool void
	Any other types would be user defined (via structs)
	*/
	class Type
	{
	public:

		virtual string type2str() const = 0; 

		inline bool operator==(const Type& rhs) {
			return this->type2str() == rhs.type2str();
		}

		virtual ~Type() {}

	protected:
		Type() {}
	};
}

