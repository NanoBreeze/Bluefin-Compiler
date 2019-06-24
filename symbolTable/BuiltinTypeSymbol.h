#pragma once
#include "Type.h"
#include "Symbol.h"

#include <memory>

namespace bluefin {

	class BuiltinTypeSymbol : public Type, public Symbol, public std::enable_shared_from_this<BuiltinTypeSymbol>
	{
	public:

		enum class Possibilities {
			INT, FLOAT, STRING, BOOL, VOID
		};



		BuiltinTypeSymbol(const Possibilities type) : Symbol{ convertEnumToString(type) }
		{ } 

		inline string getCategoryName() const override { return "builtinType"; }
		inline string type2str() const override { return getName(); }

		shared_ptr<Type> getType()  override { return shared_from_this(); }  // must make sure that this BuiltinTypeSymbol's raw ptr is already
			// handled by a shared ptr. This means that every use of BuiltinTypeSymbol is within shared_ptr

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
