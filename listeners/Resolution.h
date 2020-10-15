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
	class Resolution : public bluefinBaseListener
	{
	public:
		Resolution(SymbolTable& symTab) : symbolTable{symTab}
		{}

		void enterVarDecl(bluefinParser::VarDeclContext * ctx) override;
		void enterParam(bluefinParser::ParamContext * ctx) override;
		void enterFuncDef(bluefinParser::FuncDefContext * ctx) override;
		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;
		void exitMemberAccess(bluefinParser::MemberAccessContext*) override;
		void enterFuncCall(bluefinParser::FuncCallContext*) override;
		void exitMethodCall(bluefinParser::MethodCallContext*) override;

		void attachEventObserver(shared_ptr<EventObserver>);
		void detachEventObserver(shared_ptr<EventObserver>); // is this even called? If arg not found, no error would be thrown

	private:
		SymbolTable& symbolTable;
		vector<shared_ptr<EventObserver>> eventObservers;

		void broadcastEvent(SuccessEvent, shared_ptr<Symbol>, shared_ptr<StructSymbol> structSym = nullptr);
		void broadcastEvent(ErrorEvent, string, shared_ptr<StructSymbol> structSym=nullptr);
	};
}
