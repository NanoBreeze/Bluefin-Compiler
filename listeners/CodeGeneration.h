#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include <map>
#include <stack>
#include <vector>
#include "../symbolTable/EventObserver.h"
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/StructSymbol.h"
#include "../symbolTable/Scope.h"

#include "llvm/IR/Value.h"


namespace bluefin {

	using std::shared_ptr;
	using std::map;
	using std::stack;
	using std::pair;
	using std::vector;
	using antlr4::tree::ParseTree;

	class EventObserver;

	/*
	Resolves symbol references. To do so we need the appropriate scope associated with the contexts. 
	This should be passed from the Declaration phase. Resolving symbols after all have been declared 
	allows forward reference. To restrict forward references (only to within classes), we compare the 
	position of the context to resolve with the position of the context of the actual resolved symbol 
	(assumes it is resolved). Illegal is current context occurs before the other one.
	*/
	class CodeGeneration : public bluefinBaseListener
	{
	public:
		CodeGeneration(SymbolTable& symTab);

		void enterFuncDef(bluefinParser::FuncDefContext*) override;
		void enterPrimaryBool(bluefinParser::PrimaryBoolContext*) override;
		void enterPrimaryInt(bluefinParser::PrimaryIntContext*) override;
		//void enterPrimaryFloat(bluefinParser::PrimaryFloatContext*) override;
		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;
		void exitAddExpr(bluefinParser::AddExprContext*) override;

		void attachEventObserver(shared_ptr<EventObserver>);
		void detachEventObserver(shared_ptr<EventObserver>); // is this even called? If arg not found, no error would be thrown
		void dump();

	private:
		SymbolTable& symbolTable;
		map<ParseTree*, llvm::Value*> values; // stores the type associated ctx node with the LLVM value
		map<shared_ptr<Symbol>, llvm::Value*> resolvedSymAndValues; // this is clumsy. It is used to resolve the Value associated with a primaryId. eg) a+6;

		vector<shared_ptr<EventObserver>> eventObservers;

		void broadcastEvent(SuccessEvent, shared_ptr<Symbol>, shared_ptr<StructSymbol> structSym = nullptr);
		void broadcastEvent(ErrorEvent, string, shared_ptr<StructSymbol> structSym=nullptr);
	};
}
