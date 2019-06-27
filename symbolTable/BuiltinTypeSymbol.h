#pragma once
#include "Type.h"
#include "Symbol.h"

#include <memory>

namespace bluefin {

	class BuiltinTypeSymbol : public Type, public Symbol
	{
	public:

		enum class BuiltinType {
			INT, FLOAT, STRING, BOOL, VOID
		};



		BuiltinTypeSymbol(const BuiltinType type) :
			Type{ convertBuiltinEnumToTypeEnum(type) }, 
			Symbol { convertEnumToString(type), this }
		{ } 

		inline string getCategoryName() const override { return "builtinType"; }
		inline string type2str() const override { return getName(); }

	private:

		string convertEnumToString(BuiltinType possibility) const {
			switch (possibility) {
			case BuiltinType::INT:
				return "int";
			case BuiltinType::FLOAT:
				return "float";
			case BuiltinType::STRING:
				return "string";
			case BuiltinType::BOOL:
				return "bool";
			case BuiltinType::VOID:
				return "void";
			default:
				throw "oh no! No other conversions possible";
			}
		}
		
		Type::Possibility convertBuiltinEnumToTypeEnum(BuiltinType builtinType) const {
			switch (builtinType) {
			case BuiltinType::INT:
				return Type::Possibility::INT;
			case BuiltinType::FLOAT:
				return Type::Possibility::FLOAT;
			case BuiltinType::STRING:
				return Type::Possibility::STRING;
			case BuiltinType::BOOL:
				return Type::Possibility::BOOL;
			case BuiltinType::VOID:
				return Type::Possibility::VOID;
			default:
				throw "oh no! No other conversions possible";
			}
		}
	};
}
