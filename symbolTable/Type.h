#pragma once

#include <iostream>

namespace bluefin {

	using std::string;
	using std::hash;

	/*
	In the past, BuiltinTypeSymbol and StructSymbol inherited from this.
	Now, no class should inherit this. It should be composed. 
	Types are internally stored as strings, such as "int", "float", "First", 
	Used mostly for type checking testing

	There are 5 default types: int, float, string, bool, void
	Any other types would be user defined (via structs)
	*/

	class Type
	{
	public:

		static Type INT() { return Type{ "int" }; }
		static Type BOOL() { return Type{ "bool" }; }
		static Type FLOAT() { return Type{ "float" }; }
		static Type STRING() { return Type{ "string" }; }

		string type2str() const {
			return name;
		}

		bool isUserDefinedType() const {
			bool isBuiltinType = (name == "int" || name == "bool" || name == "float" || name == "string" || name == "void");
			return !isBuiltinType;
		}

		explicit Type(string name) : name{ name } {}
		~Type() {}

	private:
		string name;

	};


	inline bool operator ==(const Type& lhs, const Type& rhs) {
		return lhs.type2str() == rhs.type2str();
	}

	inline bool operator !=(const Type& lhs, const Type& rhs) {
		return lhs.type2str() != rhs.type2str();
	}

	inline bool operator <(const Type& lhs, const Type& rhs) {
		return lhs.type2str() < rhs.type2str();
	}
}

namespace std {
	using bluefin::Type;

	template <>
	struct std::hash<Type> {
		size_t operator()(const Type& type) const {
			return hash<string>()(type.type2str());
		}
	};
};
