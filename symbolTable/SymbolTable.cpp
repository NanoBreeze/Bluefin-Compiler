
#include <memory>
#include <exception>
#include "SymbolTable.h"
#include "StructSymbol.h"
#include "BuiltinTypeSymbol.h"
#include "./Exceptions.h"


namespace bluefin {

	using std::dynamic_pointer_cast;
	using std::make_shared;

	SymbolTable::SymbolTable() : currScope{ new Scope(nullptr, "global") } {

		// Add all built-in types to the global scope
		currScope->declare(BuiltinTypeSymbol::BOOL());
		currScope->declare(BuiltinTypeSymbol::STRING());
		currScope->declare(BuiltinTypeSymbol::INT());
		currScope->declare(BuiltinTypeSymbol::FLOAT());
		currScope->declare(BuiltinTypeSymbol::VOID());
	}

	void SymbolTable::enterScope(const string scopeName) {
		shared_ptr<Scope> scope = make_shared<Scope>(currScope, scopeName);
		currScope = scope;
	}

	/*
	Sets the passed in scope as the current scope so all declares and resolves
	come from there. Right now, this is used only by StructScope to simplify its declaration logic.
	*/
	void SymbolTable::setCurrentScope(shared_ptr<Scope> scope)
	{
		assert(scope->getEnclosingScope() == currScope);

		currScope = scope;
	}

	void SymbolTable::exitScope() {
		currScope = currScope->getEnclosingScope();
	}

	void SymbolTable::declare(shared_ptr<Symbol> symbol, ParseTree* parseTree) {
		if (shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbol)) {
			addUserDefinedType(structSym);
		}

		parseTreeContexts.emplace(parseTree, Context{ currScope, symbol });
		currScope->declare(symbol); // it's important to call currScope->declare(..) after placing the parseTree into the map. Otherwise, 
		// an exception may be thrown and the parseTreeContexts won't contain it, which is not good for resolution phase.
	}

	shared_ptr<Symbol> SymbolTable::resolve(const string name, const shared_ptr<Scope> scope) const {

		shared_ptr<Scope> scopeToSearch = scope;

		do {
			shared_ptr<Symbol> sym = scopeToSearch->resolve(name);
			if (sym) { return sym; }
		} while (scopeToSearch = scopeToSearch->getParentScope());

		throw UnresolvedSymbolException(name);
	}

	shared_ptr<Symbol> SymbolTable::resolveMember(const string memberName, const shared_ptr<StructSymbol> structSym) const {

		shared_ptr<StructSymbol> currStructSym = structSym;

		do {
			shared_ptr<Symbol> member = currStructSym->resolve(memberName);
			if (member) { return member; }
		} while (currStructSym = currStructSym->getSuperClass());

		throw UnresolvedSymbolException(memberName);
	}

	shared_ptr<Symbol> SymbolTable::getSymbolMatchingType(Type type) const {

		if (typeSymbols.find(type) != typeSymbols.end())
			return typeSymbols.at(type);

		throw UnresolvedSymbolException(type.type2str());
	}

	void SymbolTable::saveParseTreeWithCurrentScope(ParseTree* parseTree) {
		Context context;
		context.scope = currScope; 
		context.sym = nullptr; // note, we don't set the symbol. Question, then when, if ever, will we set it?
		parseTreeContexts.emplace(parseTree, context);
	}

	shared_ptr<Scope> SymbolTable::getScope(ParseTree* parseTree) const {
		return parseTreeContexts.at(parseTree).scope;
	}

	shared_ptr<Scope> SymbolTable::getScope(shared_ptr<Symbol> sym) const {
		for (auto& it : parseTreeContexts) {
			if (it.second.sym == sym) {
				return it.second.scope;
			}
		}

		throw std::exception("Sym not in map");
	}

	void SymbolTable::addUserDefinedType(shared_ptr<StructSymbol> structSym) {
		if (typeSymbols.count(structSym->getType())) {
			throw RedeclarationException(structSym->getType().type2str());
		}

		typeSymbols.emplace(structSym->getType(), structSym);
	}
}
