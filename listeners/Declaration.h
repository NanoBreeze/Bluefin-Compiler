#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include <map>
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/SymbolFactory.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/EventObserver.h"

namespace bluefin {

	using std::shared_ptr;
	using bluefin::Scope;
	using antlr4::tree::ParseTree;
	using std::map;

	/*
	Declares symbols. If necessary, will also resolve their types.
	Also annotates the ParseTree contexts with their appropriate scope, which will be passed to the resolution
	phase to resolve the containing symbols.
	*/
	class Declaration : public bluefinBaseListener
	{
	public:
		Declaration(SymbolTable& symbolTable, SymbolFactory& factory) 
			: symbolTable{ symbolTable }, symbolFactory{ factory }
		{}

		void enterVarDecl(bluefinParser::VarDeclContext*) override;

		/*
		Design decision for function params:
			A FunctionSymbol must store its parameters (as Symbols) somewhere so that when they are called,
			we can check the type and the number of arguments match. However, once we are in enterParam, there's 
			no way to refer to its FunctionSymbol. As a result, we must store the FunctionSymbol as a member variable
			and remember to set it upon entering the function, attach params to it, and then remove the refernce 
			upon exiting the function definition
		*/
		void enterFuncDef(bluefinParser::FuncDefContext*) override;
		void enterParam(bluefinParser::ParamContext*) override;
		void enterStructDef(bluefinParser::StructDefContext*) override;
		void exitStructDef(bluefinParser::StructDefContext* ctx) override;
		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;

		void enterBlock(bluefinParser::BlockContext*) override;
		void exitBlock(bluefinParser::BlockContext*) override;
		void exitFuncDef(bluefinParser::FuncDefContext*) override;
		void enterFuncCall(bluefinParser::FuncCallContext*) override;
		void enterMemberAccess(bluefinParser::MemberAccessContext*) override;
		void enterMethodCall(bluefinParser::MethodCallContext*) override;

		void attachEventObserver(shared_ptr<EventObserver>);
		void detachEventObserver(shared_ptr<EventObserver>); // is this even called? If arg not found, no error would be thrown

	private:
		SymbolTable& symbolTable;
		SymbolFactory& symbolFactory;
		vector<shared_ptr<EventObserver>> eventObservers;

		void broadcastEvent(ScopeEvent);
		void broadcastEvent(SuccessEvent, shared_ptr<Symbol>, shared_ptr<StructSymbol> structSym = nullptr);
		void broadcastEvent(ErrorEvent, string, shared_ptr<StructSymbol> structSym=nullptr);
		void broadcastEvent(PolymorphismErrorEvent, string);

		// to share a functionSymbol with its associated params
		shared_ptr<FunctionSymbol> currFunctionSym;

		// to know whether we're inside a method or function. 
		// If inside a method, may need to check for override keyword
		shared_ptr<StructSymbol> currStructSym;
	};
}
