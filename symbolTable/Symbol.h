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
		bool operator==(Symbol& rhs);

		inline string getName() const { return name; }
		inline shared_ptr<Type> getType() const  { return type; }
		inline size_t getTokenIndex() const { return tokenIndex; }
		virtual ~Symbol() {}

	protected:
		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, shared_ptr<Type> type, size_t tokenIndex) : 
			name{ name }, type{ type }, tokenIndex{ tokenIndex }
		{}
		
		// Both BuiltinTypeSymbol and StructSymbol also inherit from Type
		// passing a self reference to the ctor isn't a good idea
		//  with adding smart pointer b/c it would cause double deletion 
		// when user's sp goes out of scope. Single this is handled by two
		// groups of shared pointers. They would use this ctor, so create custom deleter
		// that doesn't delete this. 
		// NOTE: Implementation Hack
		Symbol(const string& name, Type* t) : 
			name{ name }, type{ t, [](Type*) {} }, tokenIndex{ 0 }
		{}

	private:
		const string name;
		shared_ptr<Type> type; // a variableSymbol or other symbols may require this type for their ctors
		size_t tokenIndex;
	};
}