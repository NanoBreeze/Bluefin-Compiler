#include "pch.h"

#include "SymbolTableTestWrapper.h"
#include <assert.h>
#include <string>
#include "../../symbolTable/FunctionSymbol.h"
#include "../../symbolTable/StructSymbol.h"
#include "../../symbolTable/VariableSymbol.h"
#include "../../symbolTable/Exceptions.h"

using namespace SymbolTableTests;

void SymbolTableTestWrapper::enterScope(const string scopeName) {

	output += createEnterScopeDebugMsg(); 
	symbolTable.enterScope();
}

void SymbolTableTestWrapper::exitScope() {

	output += createExitScopeDebugMsg(); 
	symbolTable.exitScope();
}

void SymbolTableTestWrapper::declare(Symbol* symbol) {

	try {
		symbolTable.declare(symbol);
		output += createDeclareDebugMsg(symbol); 
	}
	catch (ReclarationException e) {
		output += createRedeclarationDebugMsg(symbol);
	}
	catch (...) {
		throw "This kind of exception not expected";
	}
}

Symbol* SymbolTableTestWrapper::resolve(const string name) {

	Symbol* resolvedSym = symbolTable.resolve(name);
	assert(resolvedSym->getName() == name);
	output += createResolveDebugMsg(resolvedSym);

	return resolvedSym;
}

string SymbolTableTestWrapper::createEnterScopeDebugMsg() {

	scopeLevel++;
	const string scopeName = "Level " + std::to_string(scopeLevel);

	return "enterScope - " + scopeName + "\n";
}

string SymbolTableTestWrapper::createExitScopeDebugMsg() {

	const string scopeName = "Level " + std::to_string(scopeLevel);
	scopeLevel--;

	return "exitScope - " + scopeName + "\n";
}

string SymbolTableTestWrapper::createDeclareDebugMsg(Symbol* symbol) const {
	const string symName = symbol->getName();
	const string symCategory = symbol->getCategoryName();

	return "declare - " + symName + " - " + symCategory + "\n";
}

string SymbolTableTestWrapper::createRedeclarationDebugMsg(Symbol* symbol) const {
	const string symName = symbol->getName();

	return "declare - " + symName + " - " + "REDECLARATION\n";
}

string SymbolTableTestWrapper::createResolveDebugMsg(Symbol const* resolvedSym) const {
	const string resolvedSymName = resolvedSym->getName();
	const string symCategory = resolvedSym->getCategoryName();

	return "resolve - " + resolvedSymName + " - " + symCategory + "\n";
}