#pragma once

#include <iostream>
#include "Type.h"

namespace bluefin {

	using std::string;

	/**
	Abstract class
	*/
	class Symbol {

	public:
		inline string getName() const { return name; }
		inline Type* getType() const { return type; }

		bool operator==(Symbol& rhs);

		virtual string getCategoryName() const = 0; //eg, function, struct, builtin, var
		virtual ~Symbol() {}

	protected:
		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, Type* type) : name{ name }, type{ type } 
		{}

	private:
		const string name;
		Type* type;
	};
}