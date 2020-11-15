#include "EventObserver.h"
#include "Symbol.h"
#include "StructSymbol.h"
#include "BuiltinTypeSymbol.h"
#include "VariableSymbol.h"
#include "FunctionSymbol.h"
#include <assert.h>
#include <string>
#include <exception>

using namespace bluefin;
using std::to_string;
using std::dynamic_pointer_cast;

void EventObserver::onEvent(ScopeEvent simpleEvent)
{
	switch (simpleEvent) {
		case ScopeEvent::ENTERING_SCOPE:
			scopeLevel++;
			output += "enterScope - Level " + to_string(scopeLevel) + "\n";
			break;
		case ScopeEvent::EXITING_SCOPE:
			output += "exitScope - Level " + to_string(scopeLevel) + "\n";
			scopeLevel--;
			break;
		case ScopeEvent::SETTING_CURRENT_SCOPE:
			scopeLevel++;
			output += "setCurrentScope - Level " + to_string(scopeLevel) + "\n";
			break;
		default:
			assert(false);
	}
}
void EventObserver::onEvent(SuccessEvent successEvent, shared_ptr<Symbol> sym, shared_ptr<StructSymbol> structSym) {

	if (successEvent == SuccessEvent::DECLARED_SYMBOL && structSym != nullptr)
		throw std::logic_error{ "Invalid combination" };

	const string symName = sym->getName();
	const string symType = sym->getType().type2str();
	const string symCategory = getSymbolCategory(sym);

	switch (successEvent) {

	case SuccessEvent::DECLARED_SYMBOL:
		output += "declare - " + symName + " - c_" + symCategory + " - t_" + symType + "\n";
		break;
	case SuccessEvent::RESOLVED_SYMBOL:
		if (structSym) {
			const string structTypeName = structSym->getType().type2str();
			output += "resolve - " + structTypeName + "::" + symName + " - c_" + symCategory + " - t_" + symType + "\n";
		}
		else {
			output += "resolve - " + symName + " - c_" + symCategory + " - t_" + symType + "\n";
		}
		break;
	default:
		assert(false);
	}

}

void EventObserver::onEvent(ErrorEvent errorEvent, string symName, shared_ptr<StructSymbol> structSym) {

	switch (errorEvent) {
	case ErrorEvent::REDECLARED_EXISTING_SYMBOL:
		output += "declare - " + symName + " - " + "REDECLARATION\n";
		break;
	case ErrorEvent::UNRESOLVED_SYMBOL:
		if (structSym) {
			const string structTypeName = structSym->getType().type2str();
			output += "resolve - " + structTypeName + "::" + symName + " - " "UNRESOLVED\n";
		}
		else {
			output += "resolve - " + symName + " - " "UNRESOLVED\n";
		}
		break;
	case ErrorEvent::ILLEGAL_FORWARD_REFERENCE:
		output += "resolve - " + symName + " - " "ILLEGAL_FORWARD_REFERENCE\n";
		break;
	case ErrorEvent::UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE:
		output += "resolve - " + symName + " - " "UNRESOLVED_STRUCTDEF_TO_MATCH_TYPE\n";
		break;
	case ErrorEvent::BUILTINTYPE_CANNOT_HAVE_MEMBER:
		output += "resolve - " + symName + " - " "BUILTINTYPE_CANNOT_HAVE_MEMBER\n";
		break;
	case ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM:
		if (structSym) {
			const string structTypeName = structSym->getType().type2str();
			output += "resolve - " + structTypeName + "::" + symName + " - " "FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM\n";
		}
		else {
			output += "resolve - " + symName + " - " "FOUND_RESOLVED_SYMBOL_BUT_NOT_FUNCSYM\n";
		}
		break;
	case ErrorEvent::FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM:
		if (structSym) {
			const string structTypeName = structSym->getType().type2str();
			output += "resolve - " + structTypeName + "::" + symName + " - " "FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM\n";
		}
		else {
			output += "resolve - " + symName + " - " "FOUND_RESOLVED_SYMBOL_BUT_NOT_VARSYM\n";
		}
		break;
	default:
		assert(false);
	}
}

void EventObserver::onEvent(SimpleTypeErrorEvent typeEventError, Type lhs, Type rhs) {
	switch (typeEventError) {
	case SimpleTypeErrorEvent::IF_STATEMENT_NOT_BOOL:
		typeOutput += "If statement. Expected: 'bool', Found: '" + lhs.type2str() + "'\n";
		break;
	case SimpleTypeErrorEvent::WHILE_STATEMENT_NOT_BOOL:
		typeOutput += "While statement. Expected: 'bool', Found: '" + lhs.type2str() + "'\n";
		break;
	case SimpleTypeErrorEvent::MISSING_RETURN_STATEMENT:
		typeOutput += "Return statement missing. Expected: '" + lhs.type2str() + "'\n";
		break;
	case SimpleTypeErrorEvent::RETURN_INVALID_TYPE:
		typeOutput += "Return type invalid. Expected: '" + lhs.type2str() + "', Found: '" + rhs.type2str() + "'\n";
		break;
	case SimpleTypeErrorEvent::INCOMPATIBLE_ARG_PARAM_TYPE:
		typeOutput += "Function call argument type invalid. Expected: '" + lhs.type2str() + "', Found: '" + rhs.type2str() + "'\n";
		break;
	default:
		assert(false);
	}
}

void EventObserver::onEvent(OperatorTypeErrorEvent operatorTypeErrorEvent, string op, Type lhs, Type rhs) {
	switch (operatorTypeErrorEvent) {
	case OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND:
		typeOutput += "Operator '" + op + "' cannot be applied to '" + lhs.type2str() + "' and '" + rhs.type2str() + "'\n";
		break;
	case OperatorTypeErrorEvent::INCOMPATIBLE_UNARY_OPERATOR_OPERAND:
		typeOutput += "Operator '" + op + "' cannot be applied to '" + lhs.type2str() + "'\n";
		break;
	default:
		assert(false);
	}
}

void EventObserver::onEvent(FunctionCallTypeErrorEvent funcCallTypeErrorEvent, string funcName, size_t argCount, size_t paramCount, bool isMethod) {
	switch (funcCallTypeErrorEvent) {
	case FunctionCallTypeErrorEvent::ARGS_AND_PARAMS_COUNT_DIFFER:
		if (isMethod)
			typeOutput += "Method call " + funcName + "(..) has " + to_string(argCount) + " arguments but definition expects " + to_string(paramCount) + "\n";
		else 
			typeOutput += "Function call " + funcName + "(..) has " + to_string(argCount) + " arguments but definition expects " + to_string(paramCount) + "\n";
		break;
	default:
		assert(false);
	}
}

void EventObserver::onEvent(PolymorphismErrorEvent e, string funcName) {
	switch (e) {
	case PolymorphismErrorEvent::FUNCTION_CANNOT_HAVE_OVERRIDE_SPECIFIER:
		polymorphismOutput += "'override' specifier can be applied only to methods. '" + funcName + "' is a function\n";
		break;
	case PolymorphismErrorEvent::FUNCTION_CANNOT_HAVE_VIRTUAL_SPECIFIER:
		polymorphismOutput += "'virtual' specifier can be applied only to methods. '" + funcName + "' is a function\n";
		break;
	case PolymorphismErrorEvent::CORRESPONDING_VIRTUAL_METHOD_NOT_FOUND:
		polymorphismOutput += "No corresponding virtual method matches method '" + funcName + "'\n";
		break;
	case PolymorphismErrorEvent::MISSING_OVERRIDE_SPECIFIER:
		polymorphismOutput += "Missing 'override' specifier for method '" + funcName + "'\n";
		break;
	case PolymorphismErrorEvent::NO_SUPERCLASS:
		polymorphismOutput += "Cannot apply 'override' qualifier on method '" + funcName + "' since its class has no parent class\n";
		break;
	case PolymorphismErrorEvent::ILLEGAL_OVERLOAD_METHOD:
		polymorphismOutput += "Derived class' method '" + funcName + "' has same name as parent class' method but doesn't override\n";
		break;
	default:
		assert(false);
	}
}

string EventObserver::getSymbolCategory(shared_ptr<Symbol> symbol) const
{
	if (dynamic_pointer_cast<BuiltinTypeSymbol>(symbol)) {
		return "builtinType";
	}
	else if (dynamic_pointer_cast<VariableSymbol>(symbol)) {
		return "var";
	}
	else if (dynamic_pointer_cast<FunctionSymbol>(symbol)) {
		return "function";
	}
	else { //struct
		return "struct";
	}
}

