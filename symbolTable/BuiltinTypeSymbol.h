#pragma once
#include "Type.h"
#include "Symbol.h"

#include <memory>

namespace bluefin {

	using std::shared_ptr;

	class BuiltinTypeSymbol : public Type, public Symbol
	{
	public:
		static shared_ptr<BuiltinTypeSymbol> INT() {
			static shared_ptr<BuiltinTypeSymbol> builtinInt(new BuiltinTypeSymbol("int"));
			return builtinInt;
		}

		static shared_ptr<BuiltinTypeSymbol> FLOAT() {
			static shared_ptr<BuiltinTypeSymbol> builtinFloat(new BuiltinTypeSymbol("float"));
			return builtinFloat;
		}

		static shared_ptr<BuiltinTypeSymbol> STRING() {
			static shared_ptr<BuiltinTypeSymbol> builtinStr(new BuiltinTypeSymbol("string"));
			return builtinStr;
		}
		static shared_ptr<BuiltinTypeSymbol> BOOL() {
			static shared_ptr<BuiltinTypeSymbol> builtinBool(new BuiltinTypeSymbol("bool"));
			return builtinBool;
		}
		static shared_ptr<BuiltinTypeSymbol> VOID() {
			static shared_ptr<BuiltinTypeSymbol> builtinVoid(new BuiltinTypeSymbol("void"));
			return builtinVoid;
		}


		inline string type2str() const override { return getName(); }

	private:
		BuiltinTypeSymbol(string type) : Symbol{ type, this } {}
	};
}
