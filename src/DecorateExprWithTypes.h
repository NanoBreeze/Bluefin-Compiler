#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/SymbolFactory.h"
#include "../symbolTable/Exceptions.h"
#include "../symbolTable/Scope.h"

using bluefin::Type;
using bluefin::Scope;
using std::shared_ptr;

// TODO: Adding this specialization here mostly so that Testbluefin can link to this. 
// Wtihout this, it complains that the ParseTreeProperty<...> implementation can't be found,
// This is b/c it's not linked to the implementation, as the implementation of
// templates are generated elsewhere, it seems

template <>
class antlr4::tree::ParseTreeProperty<shared_ptr<Type>> {

  public:
    virtual shared_ptr<Type> get(ParseTree *node) {
      return _annotations[node];
    }
    virtual void put(ParseTree *node, shared_ptr<Type> value) {
      _annotations[node] = value;
    }
    virtual shared_ptr<Type> removeFrom(ParseTree *node) {
      auto value = _annotations[node];
      _annotations.erase(node);
      return value;
    }

  protected:
    std::map<ParseTree*, shared_ptr<Type>> _annotations;
};


namespace bluefin {

	using antlr4::tree::ParseTreeProperty;

	class DecorateExprWithTypes : public bluefinBaseListener
	{
	public:

		// For testing, we'll pass in an adapter of a symbol table
		DecorateExprWithTypes(ParseTreeProperty<shared_ptr<Scope>> scopeOfPrimaryIds, SymbolFactory& factory)
			: scopeOfPrimaryIds{ scopeOfPrimaryIds }, symbolFactory{ factory }
		{}

		//===== listener methods for obtaining/evaluating expression types
		// These are S-attributes so post-order traversal works
		void exitPrimaryInt(bluefinParser::PrimaryIntContext*) override;
		void exitPrimaryFloat(bluefinParser::PrimaryFloatContext*) override;
		void exitPrimaryString(bluefinParser::PrimaryStringContext*) override;
		void exitPrimaryBool(bluefinParser::PrimaryBoolContext*) override;
		void exitPrimaryId(bluefinParser::PrimaryIdContext*) override;
		void exitPrimaryParenth(bluefinParser::PrimaryParenthContext*) override;
		void exitFuncCall(bluefinParser::FuncCallContext*) override;
		void exitUnaryExpr(bluefinParser::UnaryExprContext*) override;
		void exitMultiExpr(bluefinParser::MultiExprContext*) override;
		void exitAddExpr(bluefinParser::AddExprContext*) override;
		void exitRelExpr(bluefinParser::RelExprContext*) override;
		void exitEqualityExpr(bluefinParser::EqualityExprContext*) override;
		void exitLogicalANDExpr(bluefinParser::LogicalANDExprContext*) override;
		void exitLogicalORExpr(bluefinParser::LogicalORExprContext*) override;
		void exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext*) override;
		void exitMemberAccess(bluefinParser::MemberAccessContext*) override;

		antlr4::tree::ParseTreeProperty<shared_ptr<Type>> getExprTypes() { return exprTypes; }	// stores the type associated for expressions. Enables type checking

	private:
		SymbolFactory& symbolFactory;

		ParseTreeProperty<shared_ptr<Scope>> scopeOfPrimaryIds;

		// stores the type associated for expressions. Enables type checking
		ParseTreeProperty<shared_ptr<Type>> exprTypes;

		shared_ptr<Symbol> resolve(const string name, shared_ptr<Scope> startScope);
	};
}
