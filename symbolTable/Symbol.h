
#include <iostream>
#include "Type.h"

namespace bluefin {

	using std::string;

	/**
	Abstract class
	*/
	class Symbol {

	public:
		string getName() const { return name; }
		Type getType() const { return type; }

	protected:
		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, const Type type) : name{ name }, type{ type } 
		{}

	private:
		const string name;
		const Type type;
	};
}