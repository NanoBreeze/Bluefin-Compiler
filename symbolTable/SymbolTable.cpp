
#include <memory>
#include <exception>
#include "SymbolTable.h"
#include "StructSymbol.h"
#include "FunctionSymbol.h"
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

		parentTypes.emplace(Type::BOOL(), Type::getUnusableType());
		parentTypes.emplace(Type::INT(), Type::getUnusableType());
		parentTypes.emplace(Type::FLOAT(), Type::getUnusableType());
		parentTypes.emplace(Type::STRING(), Type::getUnusableType());
		parentTypes.emplace(Type::VOID(), Type::getUnusableType());
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

	bool SymbolTable::findCorrespondingVirtualMethod(const shared_ptr<FunctionSymbol> funcSym, const shared_ptr<StructSymbol> structSym) const {
		shared_ptr<StructSymbol> currStructSym = structSym->getSuperClass();
		string methodName = funcSym->getName();

		while (currStructSym) {
			shared_ptr<FunctionSymbol> resolvedFunc = dynamic_pointer_cast<FunctionSymbol>(currStructSym->resolve(methodName));
			if (resolvedFunc && resolvedFunc->isVirtual()) { // a virtual function has been found with the same name! Now check that its return and arg types are the same
				if (resolvedFunc->getType() != funcSym->getType()) return false;

				vector<shared_ptr<Symbol>> resolvedParams = resolvedFunc->getParams();
				vector<shared_ptr<Symbol>> currParams = funcSym->getParams();

				if (resolvedParams.size() != currParams.size()) return false;
				for (int i = 0; i < resolvedParams.size(); i++) {
					if (resolvedParams[i]->getType() != currParams[i]->getType()) {
						return false;
					}
				}

				return true;
			}
			currStructSym = currStructSym->getSuperClass();
		}

		return false;
	}

	shared_ptr<Symbol> SymbolTable::getSymbolMatchingType(Type type) const {

		if (typeSymbols.find(type) != typeSymbols.end())
			return typeSymbols.at(type);

		throw UnresolvedStructDefException(type.type2str()); // eg, for UndefinedType blah;, the `Type` attribute corresponding to 
		// blah's VariableSym is set to Type{"UndefinedType"}. When calling this method with it, we want to return the StructSymbol for "UndefinedType"
		// and throw an exception if we can't find it
	}

	void SymbolTable::saveParseTreeWithCurrentScope(ParseTree* parseTree) {
		assert (parseTreeContexts.count(parseTree) == 0); 
		Context context = {};
		context.scope = currScope; 
		// note, we don't set the symbol nor resolvedSym. Question, then when, if ever, will we set it?
		parseTreeContexts.emplace(parseTree, context);
	}

	void SymbolTable::saveParseTree(ParseTree* parseTree)
	{
		assert (parseTreeContexts.count(parseTree) == 0); 
		Context context = {};
		parseTreeContexts.emplace(parseTree, context);
	}

	void SymbolTable::addParseTreeContext(ParseTree* parseTree, shared_ptr<Scope> scope, shared_ptr<Symbol> sym)
	{
		assert (parseTreeContexts.count(parseTree) == 0); 
		Context context = {};
		context.scope = scope;
		context.sym = sym;
		parseTreeContexts.emplace(parseTree, context);
	}

	void SymbolTable::updateParseTreeContextExternalStructMember(ParseTree* parseTree, shared_ptr<StructSymbol> structSym, shared_ptr<Symbol> resSym)
	{
		parseTreeContexts.at(parseTree).resolvedSym = resSym;
		parseTreeContexts.at(parseTree).scope = structSym;
	}

	void SymbolTable::updateParseTreeContextWithResolvedSym(ParseTree* parseTree, shared_ptr<Symbol> resolvedSym)
	{
		parseTreeContexts.at(parseTree).resolvedSym = resolvedSym;
	}

	shared_ptr<Symbol> SymbolTable::getSymbol(ParseTree* parseTree) const
	{
		return parseTreeContexts.at(parseTree).sym;
	}

	shared_ptr<Symbol> SymbolTable::getResolvedSymbol(ParseTree* parseTree) const
	{
		return parseTreeContexts.at(parseTree).resolvedSym;
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
			assert(parentTypes.count(structSym->getType()) == 1);  // since parentTypes shared same key as typeSymbols
			throw RedeclarationException(structSym->getType().type2str());
		}

		typeSymbols.emplace(structSym->getType(), structSym);

		Type parentType = (structSym->getSuperClass()) ? structSym->getSuperClass()->getType() : Type::getUnusableType();
		parentTypes.emplace(structSym->getType(), parentType);
	}

	bool SymbolTable::isParentType(const Type child, const Type parent) const {
		// both types must be valid types that are already in the table (so they already have valid corresponding StructSym)
		assert(parentTypes.count(child) == 1 && parentTypes.count(parent) == 1);

		Type nextParent = child;
		while (nextParent != Type::getUnusableType()) {
			if (parentTypes.at(nextParent) == parent) {
				return true;
			}
			nextParent = parentTypes.at(nextParent);
		}
		return false;
	}
}
