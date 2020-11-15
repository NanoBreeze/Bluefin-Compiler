#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include <unordered_map>
#include "Scope.h"
#include "../TestBluefin/testutils.h"

namespace bluefin {

	using std::shared_ptr;
	using std::unique_ptr;
	using std::make_shared;
	using std::unordered_map;
	using antlr4::tree::ParseTree;

	class StructSymbol;
	class EventObserver;
	class FunctionSymbol;


	class SymbolTable
	{
	public:
		SymbolTable(); 
		
		void enterScope(const string scopeName="");

		void setCurrentScope(shared_ptr<Scope>);
	
		void exitScope();

		void declare(shared_ptr<Symbol> symbol, ParseTree* context=nullptr);

		//Find the name in the curr scope, if not, find in its parent scope, and continue bubbling upwards, 
		// including up to global scope
		shared_ptr<Symbol> resolve(const string name, const shared_ptr<Scope> scope) const;

		// Searches in the structSym and its parent classes. Don't search in global scope
		shared_ptr<Symbol> resolveMember(const string memberName, const shared_ptr<StructSymbol> structSym) const;

		// Searches in the structSym' parent chain for the specified virtual method. Don't search in global scope
		bool findCorrespondingVirtualMethod(const shared_ptr<FunctionSymbol> funcSym, const shared_ptr<StructSymbol> structSym) const;

		/* 
		Why might somebody want the current scope?
		1. Testing
		2. To map the current context with the current scope, passing the symbols for future listener passes
		*/
		inline shared_ptr<Scope> getCurrScope() const { return currScope; }

		shared_ptr<Symbol> getSymbolMatchingType(Type type) const;

		bool isParentType(const Type child, const Type parent) const;

		void saveParseTreeWithCurrentScope(ParseTree*);

		void saveParseTree(ParseTree*);

		// this function should be used only to add a new ParseTree* key with its Context
		void addParseTreeContext(ParseTree*, shared_ptr<Scope>, shared_ptr<Symbol>);

		void updateParseTreeContextExternalStructMember(ParseTree*, shared_ptr<StructSymbol> structSym, shared_ptr<Symbol> resSym);

		// The ParseTree* key/value must already exist.
		void updateParseTreeContextWithResolvedSym(ParseTree*, shared_ptr<Symbol> resolvedSym);

		shared_ptr<Symbol> getSymbol(ParseTree*) const;

		shared_ptr<Symbol> getResolvedSymbol(ParseTree*) const;

		shared_ptr<Scope> getScope(ParseTree*) const;

		shared_ptr<Scope> getScope(shared_ptr<Symbol>) const; // alternatively, we can make each Symbol contain a weak ref to its contianing scope

	private:
		void addUserDefinedType(shared_ptr<StructSymbol>);

		shared_ptr<Scope> currScope; // shared_ptr b/c it can refer to the same scope as a StructSymbol's
		unordered_map<Type, shared_ptr<Symbol>> typeSymbols;
		unordered_map<Type, Type> parentTypes; // the key is the child, value is the parent. This helps us determine inheritance structure based on Type
		// Used for typechecking

		/* Declaration stores the symbol and scope associated with ParseTree contexts, so that future passes
		(Resolution and type promotion) can access the scope info. Note, to resolve struct member access
		we must store the StructSymbol as a scope (eg, for a.b.c, we must store the StructSymbol for a.b 
		inside the context to later resolve c)
		Some ParseTree* do not have a dedicated symbol (eg, unlike VarDecl, which will create a Symbol, a 
		primaryId doesn't actually create one). Some can only be resolved into a Symbol. To avoid confusing 
		the two, we also create a resolvedSym field
		NOTE: This also elminiates the need for structSymbolStack. 
		Note 2: Although this works, this design may need to be refactored b/c not all ParseTree* have all three fields filled
		*/
		struct Context {
			shared_ptr<Scope> scope; // the scope that the Symbol (or ParseTree*) is in
			shared_ptr<Symbol> sym;
			shared_ptr<Symbol> resolvedSym; 
		};
		unordered_map<ParseTree*, Context> parseTreeContexts;
	};
}