#pragma once

#include <iostream>

namespace bluefin {

	using std::string;

	class Type
	{
	public:
		Type(const string& typeName) : typeName{ typeName } 
		{}

		string getName() const {
			return typeName;
		}

	private:
		const string typeName;
	};
}

