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
		UNRESOLVED_SYMBOL, // if said symbol is a struct member, use UNRESOLVED_STRUCT_MEMBER_SYMBOL instead
		ILLEGAL_FORWARD_REFERENCE
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
