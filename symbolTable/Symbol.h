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
		inline Type getType() const  { return type; }
		inline size_t getTokenIndex() const { return tokenIndex; }
		virtual ~Symbol() {}

	protected:
		// name is the name of the id, eg, a, hello, wow
		Symbol(const string& name, Type type, size_t tokenIndex=0) : 
			name{ name }, type{ type }, tokenIndex{ tokenIndex }
		{}
		
	private:
		const string name;
		Type type; // a variableSymbol or other symbols may require this type for their ctors
		size_t tokenIndex;
	};
}