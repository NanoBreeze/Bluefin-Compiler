#include "pch.h"

#include "SymbolTableTestWrapper.h"
#include <assert.h>
#include <string>
#include <memory>
#include "../../symbolTable/FunctionSymbol.h"
#include "../../symbolTable/StructSymbol.h"
#include "../../symbolTable/VariableSymbol.h"
#include "../../symbolTable/Exceptions.h"

using namespace SymbolTableTests;
using std::shared_ptr;

void SymbolTableTestWrapper::enterScope(const string scopeName) {

	output += createEnterScopeDebugMsg(); 
	SymbolTable::enterScope(scopeName);
}

void SymbolTableTestWrapper::exitScope() {

	output += createExitScopeDebugMsg(); 
	SymbolTable::exitScope();
}

void SymbolTableTestWrapper::declare(shared_ptr<Symbol> symbol) {

	try {
		SymbolTable::declare(symbol);
		output += createDeclareDebugMsg(symbol); 
	}
	catch (ReclarationException e) {
		output += createRedeclarationDebugMsg(symbol);
	}
}

shared_ptr<Symbol> SymbolTableTestWrapper::resolve(const string name) {

	shared_ptr<Symbol> resolvedSym;
	try {
		resolvedSym = SymbolTable::resolve(name);
		output += createResolveDebugMsg(resolvedSym);
		assert(resolvedSym->getName() == name);
	}
	catch (UnresolvedSymbolException e) {
		output += createUnresolvedDebugMsg(name);
	}

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

string SymbolTableTestWrapper::createDeclareDebugMsg(shared_ptr<Symbol> symbol) const {
	const string symName = symbol->getName();
	const string symCategory = symbol->getCategoryName();
	const string symType = symbol->getType()->type2str();

	return "declare - " + symName + " - c_" + symCategory + " - t_" + symType + "\n";
}

string SymbolTableTestWrapper::createRedeclarationDebugMsg(shared_ptr<Symbol> symbol) const {
	const string symName = symbol->getName();

	return "declare - " + symName + " - " + "REDECLARATION\n";
}

string SymbolTableTestWrapper::createResolveDebugMsg(shared_ptr<Symbol> resolvedSym) const {
	const string resolvedSymName = resolvedSym->getName();
	const string symCategory = resolvedSym->getCategoryName();
	const string symType = resolvedSym->getType()->type2str();

	return "resolve - " + resolvedSymName + " - c_" + symCategory + " - t_" + symType + "\n";
}

string SymbolTableTestWrapper::createUnresolvedDebugMsg(string resolvedSymName) const {

	return "resolve - " + resolvedSymName + " - " "UNRESOLVED\n";
}