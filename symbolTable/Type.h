#pragma once

#include <iostream>

namespace bluefin {

	using std::string;

	/*
	Only BuiltinTypeSymbol and StructSymbol inherit from this.
	They already have names, eg, "int", "float", "First", 
	This "Type" is mostly used as a tag. However, it does 
	contain a toString method, which returns the same as the Symbol's name,
	in order to print the type. Used mostly for testing
	*/
	class Type
	{
	public:
		
		// use PVF rather than take in typename in ctor since it will be the same as the 
		// Symbol's name, so symbol can just return it easily. Also, more suitable
		// for expressing type is just a "tag"
		virtual string type2str() const = 0; 
		virtual ~Type() {}

	protected:
		Type() {}
	};
}

