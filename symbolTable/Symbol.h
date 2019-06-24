#pragma once

#include <memory>
#include <iostream>
#include "Type.h"

namespace bluefin {

	using std::string;
	using std::shared_ptr;

	/**
	Abstract class
	*/
	class Symbol {

	public:
		inline string getName() const { return name; }
		 virtual shared_ptr<Type> getType()  { 
			return type; 
		}

		bool operator==(Symbol& rhs);

		virtual string getCategoryName() const = 0; //eg, function, struct, builtin, var
		virtual ~Symbol() {}

	protected:
		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, shared_ptr<Type> type) : name{ name }, type{type } 
		{}

		
		// Both BuiltinTypeSymbol and StructSymbol also inherit from Type
		// passing a self reference to the ctor isn't a good idea
		//  with adding smart pointer b/c it would cause double deletion 
		// when user's sp goes out of scope. Single this is handled by two
		// groups of shared pointers. Theywould use this ctor
		Symbol(const string& name) : name{ name }, type{ type }
		{}

	private:
		const string name;
		shared_ptr<Type> type; // a variableSymbol or other symbols may require this type for their ctors
	};
}