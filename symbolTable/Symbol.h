
#include <iostream>
#include "Type.h"

namespace bluefin {

	using std::string;

	class Symbol {
	public:

		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, const Type type) : name{ name }, type{ type } 
		{}

		string getName() const { return name; }
		Type getType() const { return type; }

	private:
		const string name;
		const Type type;
	};
}