#pragma once
#include "Type.h"
#include "Symbol.h"

namespace bluefin {

	class BuiltinTypeSymbol : public Type, public Symbol
	{
	public:

		enum class Possibilities {
			INT, FLOAT, STRING, BOOL, VOID
		};

		BuiltinTypeSymbol(const Possibilities type) : Symbol{ convertEnumToString(type), *this } //TODO: use enum for typename?
		{ } 

		inline string getCategoryName() const override { return "builtinType"; }

	private:
		string convertEnumToString(Possibilities possibility) const {
			switch (possibility) {
			case Possibilities::INT:
				return "int";
			case Possibilities::FLOAT:
				return "float";
			case Possibilities::STRING:
				return "string";
			case Possibilities::BOOL:
				return "bool";
			case Possibilities::VOID:
				return "void";
			default:
				throw "oh no! No other conversions possible";
			}
		}
	};
}