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
	default:
		assert(false);
	}
}

/*
void EventObserver::onEvent(ComplexEvent complexEvent, shared_ptr<Symbol> sym)
{
	const string symName = sym->getName();
	const string symType = sym->getType().type2str();
	const string symCategory = getSymbolCategory(sym);

	switch (complexEvent) {
		case ComplexEvent::DECLARED_SYMBOL:
			output += "declare - " + symName + " - c_" + symCategory + " - t_" + symType + "\n";
			break;
		case ComplexEvent::RESOLVED_SYMBOL:
			output += "resolve - " + symName + " - c_" + symCategory + " - t_" + symType + "\n";
			break;
		case ComplexEvent::REDECLARED_EXISTING_SYMBOL:
			output += "declare - " + symName + " - " + "REDECLARATION\n";
			break;
		case ComplexEvent::UNRESOLVED_SYMBOL:
			output += "resolve - " + symName + " - " "UNRESOLVED\n";
			break;
		case ComplexEvent::ILLEGAL_FORWARD_REFERENCE:
			output += "resolve - " + symName + " - " "ILLEGAL_FORWARD_REFERENCE\n";
			break;
		default:
			assert(false);
	}
}

void EventObserver::onEvent(StructMemberEvent structMemberEvent, shared_ptr<Symbol> memberSym, shared_ptr<StructSymbol> structSym) {
	const string structTypeName = structSym->getType().type2str();
	const string memberSymName = memberSym->getName();
	const string memberSymCategory = getSymbolCategory(memberSym);;
	const string memberSymType = memberSym->getType().type2str();
	switch (structMemberEvent) {
		case StructMemberEvent::RESOLVED_STRUCT_MEMBER_SYMBOL:
			output += "resolve - " + structTypeName + "::" + memberSymName + " - c_" + memberSymCategory + " - t_" + memberSymType + "\n";
			break;
		case StructMemberEvent::UNRESOLVED_STRUCT_MEMBER_SYMBOL:
			output += "resolve - " + structTypeName + "::" + memberSymName + " - UNRESOLVED\n";
			break;
		default:
			assert(false);
	}
}
*/

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
