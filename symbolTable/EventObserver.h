#pragma once

#include "Type.h"
#include <memory>
#include <string>

namespace bluefin {

	using std::shared_ptr;
	using std::string;
	class Symbol;
	class StructSymbol;

	enum class ScopeEvent {
		ENTERING_SCOPE,
		EXITING_SCOPE,
		SETTING_CURRENT_SCOPE
	};

	enum class SuccessEvent {
		DECLARED_SYMBOL,
		RESOLVED_SYMBOL
	};

	enum class ErrorEvent { // maybe call this "ResolutionErrorEvent"
		REDECLARED_EXISTING_SYMBOL,
		UNRESOLVED_SYMBOL, 
		ILLEGAL_FORWARD_REFERENCE,
		UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE, // the StructSymbol corresponding to a VarSym's Type cannot be resolved.
		// eg, UndefinedType a; a.b; (the 'a' in 'a.b' will cause it to happen)
		BUILTINTYPE_CANNOT_HAVE_MEMBER, // only StructSymbols are allowed to have members. 
		//(eg, int a; a.b; In exitMemberAccess, the 'a' will cause this to be triggered)
		FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM, // the name of the ctx is resolved. We expected its category to be FuncSym, which is how it was called, but it's not
		// eg, struct First {int a; }; int main() { First fir; fir.a(); }
		FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM // similar to above. eg) struct Second {void a() {}}; ... second.a;
	};

	enum class SimpleTypeErrorEvent {
		IF_STATEMENT_NOT_BOOL,
		WHILE_STATEMENT_NOT_BOOL,
		MISSING_RETURN_STATEMENT,
		RETURN_INVALID_TYPE,
		INCOMPATIBLE_ARG_PARAM_TYPE
	};

	enum class OperatorTypeErrorEvent {
		INCOMPATIBLE_BINARY_OPERATOR_OPERAND, // eg, a + 5, where `a` is a user-defined type
		INCOMPATIBLE_UNARY_OPERATOR_OPERAND, // eg, -a, where `a` is a user-defined type
	};

	enum class FunctionCallTypeErrorEvent {
		ARGS_AND_PARAMS_COUNT_DIFFER // calling a function whose resolved symbol has diff. number of params
	};

	// In the future, if our program gets more complex, we may need to inherit from EventObserver and let 
	// there be different implementations of `onEvent(..)`
	class EventObserver
	{
	public:

		void onEvent(ScopeEvent);
		void onEvent(SuccessEvent, shared_ptr<Symbol>, shared_ptr<StructSymbol> sym = nullptr);
		void onEvent(ErrorEvent, string symName, shared_ptr<StructSymbol> sym = nullptr);
		void onEvent(SimpleTypeErrorEvent, Type lhs, Type rhs = Type{ "" }); // admittedly, the syntax is quite ugly
		void onEvent(OperatorTypeErrorEvent, string op, Type lhs, Type rhs = Type{ "" }); // admittedly, the syntax is quite ugly
		void onEvent(FunctionCallTypeErrorEvent, string funcName, size_t argCount, size_t paramCount, bool isMethod);

		string getOutput() const { return output; }
		string getTypeOutput() const { return typeOutput; }

	private:
		string output;
		string typeOutput; // string only for types
		unsigned scopeLevel; // to be used only by scope-related events. 
		string getSymbolCategory(shared_ptr<Symbol>) const;
	};
}
