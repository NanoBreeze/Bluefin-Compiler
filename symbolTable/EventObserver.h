#pragma once

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

	enum class ErrorEvent {
		REDECLARED_EXISTING_SYMBOL,
		UNRESOLVED_SYMBOL, 
		ILLEGAL_FORWARD_REFERENCE,
		UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE, // the StructSymbol corresponding to a VarSym's Type cannot be resolved.
		// eg, UndefinedType a; a.b; (the 'a' in 'a.b' will cause it to happen)
		BUILTINTYPE_CANNOT_HAVE_MEMBER, // only StructSymbols are allowed to have members. 
		//(eg, int a; a.b; In exitMemberAccess, the 'a' will cause this to be triggered)
	};

	// In the future, if our program gets more complex, we may need to inherit from EventObserver and let 
	// there be different implementations of `onEvent(..)`
	class EventObserver
	{
	public:

		void onEvent(ScopeEvent);
		void onEvent(SuccessEvent, shared_ptr<Symbol>, shared_ptr<StructSymbol> sym = nullptr);
		void onEvent(ErrorEvent, string symName, shared_ptr<StructSymbol> sym = nullptr);

		string getOutput() const { return output; }

	private:
		string output;
		unsigned scopeLevel; // to be used only by scope-related events. 
		string getSymbolCategory(shared_ptr<Symbol>) const;
	};
}
