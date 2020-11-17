#include "utils.h"
#include "SymbolTable.h"
#include "StructSymbol.h"
#include "Exceptions.h"

#include <deque>

using namespace bluefin;
using std::dynamic_pointer_cast;
using std::deque;

namespace bluefin {
	// If the containing scope of the ctx is a StructSymbol, then this funcDef is inside a struct, aka, it's a method
	bool isStructMethod(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(ctx));
		return structSym != nullptr;
	}

	bool isStructMethod(shared_ptr<FunctionSymbol> funcSym, SymbolTable& symbolTable) {
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(funcSym));
		return structSym != nullptr;
	}

	shared_ptr<StructSymbol> getContainingStruct(shared_ptr<FunctionSymbol> funcSym, SymbolTable& symbolTable) {
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(funcSym));
		return structSym;
	}

	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::FuncDefContext* ctx, SymbolTable& symbolTable)
	{
		return dynamic_pointer_cast<StructSymbol>(symbolTable.getScope(ctx));
	}

	bool isStructField(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(varDeclScope);

		return structSym != nullptr; // if containing scope, when casted to structSymbol, is not null, then it is a structSymbol!
	}

	bool isGlobalVarDecl(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);
		return varDeclScope->getEnclosingScope() == nullptr; // if no enclosing scope, then we must be in global scope
	}

	// NOTE: a local varDecl's scope is not always a function. eg) the varDecl is inside a block
	// We say a varDecl is local if it's not global and it's not a struct field
	bool isLocalVarDecl(bluefinParser::VarDeclContext* ctx, SymbolTable& symTab)
	{
		return !(isGlobalVarDecl(ctx, symTab) || isStructField(ctx, symTab));
	}

	shared_ptr<StructSymbol> getContainingStruct(bluefinParser::VarDeclContext* ctx, SymbolTable& symbolTable)
	{
		shared_ptr<Scope> varDeclScope = symbolTable.getScope(ctx);
		return dynamic_pointer_cast<StructSymbol>(varDeclScope);
	}

	vector<shared_ptr<FunctionSymbol>> getVTableMethods(shared_ptr<StructSymbol> structSym) {
		// We first move up the hierarchy chain and collect every method with 'virtual' keyword (order matters!)
		// Then we move up the chain again and we collect the most recent override method to replace its corresponding
		// virtual one. This means that every virtual method can be replaced at most once. Since we move from the most
		// derived class to the parent class, the first override method we find is the msot derived one.
		// Note: it's possible for a virtual method to not be overriden by any subclass
		deque<shared_ptr<FunctionSymbol>> virtualMethods;
		unordered_map<string, shared_ptr<FunctionSymbol>> overridenMethods;

		// First, collect all virtual method
		shared_ptr<StructSymbol> currStructSym = structSym;
		while (currStructSym) {
			vector<shared_ptr<FunctionSymbol>> methods = currStructSym->getMethods();
			for (int i = methods.size() - 1; i >= 0; i--) { // moving backwards
				shared_ptr<FunctionSymbol> method = methods[i];
				if (method->isVirtual()) {
					virtualMethods.push_front(method);
					assert(overridenMethods.count(method->getName()) == 0);
					overridenMethods.emplace(method->getName(), nullptr);
				}
			}
			currStructSym = currStructSym->getSuperClass();
		}

		// Now get the most overriden method
		currStructSym = structSym;
		while (currStructSym) {
			vector<shared_ptr<FunctionSymbol>> methods = currStructSym->getMethods();
			for (auto method : methods) {
				if (method->isOverride() && overridenMethods.at(method->getName()) == nullptr) {
					overridenMethods.at(method->getName()) = method;
				}
			}
			currStructSym = currStructSym->getSuperClass();
		}
		
		// Now create the vtable methods
		vector<shared_ptr<FunctionSymbol>> vtableMethods(virtualMethods.size());
		for (int i = 0; i < virtualMethods.size(); i++) {
			string methodName = virtualMethods[i]->getName();
			if (overridenMethods.at(methodName))
				vtableMethods[i] = overridenMethods.at(methodName);
			else
				vtableMethods[i] = virtualMethods[i];
		}

		return vtableMethods;
	}

	bool shouldLLVMStructTypeContainExplicitVPtr(const shared_ptr<StructSymbol> structSym)
	{
		// If a class' hierarchy (parent and recursive parents) contains a virtual method, then
		// no need for explicit vptr since a parent would already have it
		// If the hierarchy doesn't contain a virtual method but our current class contains it
		// then we do need explicit vptr 

		bool isHierarchyContainsVirtualMethod = false; // hierarchy here excludes the current struct
		shared_ptr<StructSymbol> currStruct = structSym->getSuperClass();
		while (currStruct) {
			for (auto method : currStruct->getMethods()) {
				if (method->isVirtual()) {
					isHierarchyContainsVirtualMethod = true;
					break;
				}
			}
			currStruct = currStruct->getSuperClass();
		}

		if (isHierarchyContainsVirtualMethod) return false;

		bool isStructContainsVirtualMethod = false;
		for (auto method : structSym->getMethods()) {
			if (method->isVirtual()) {
				isStructContainsVirtualMethod = true;
				break;
			}
		}

		return isStructContainsVirtualMethod;
	}

	size_t getVTableMethodIndex(shared_ptr<StructSymbol> structSym, string methodName) {
		vector<shared_ptr<FunctionSymbol>> methods = getVTableMethods(structSym);
		for (int i = 0; i < methods.size(); i++) {
			if (methods[i]->getName() == methodName)
				return i;
		}

		throw MethodNoIndexInVTableException(methodName);
	}
}
