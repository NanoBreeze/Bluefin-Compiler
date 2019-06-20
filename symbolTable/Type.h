#pragma once

#include <iostream>

namespace bluefin {

	using std::string;

	class Type
	{

	public:
		string getName() const {
			return typeName;
		}

	protected:
		Type(const string& typeName) : typeName{ typeName } 
		{}

	private:
		const string typeName;
	};
}

