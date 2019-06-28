#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include <array>
#include <map>
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/SymbolFactory.h"
#include "../symbolTable/Exceptions.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/TypeContext.h"

using bluefin::TypeContext;

// TODO: Adding this specialization here mostly so that Testbluefin can link to this. 
// Wtihout this, it complains that the ParseTreeProperty<...> implementation can't be found,
// This is b/c it's not linked to the implementation, as the implementation of
// templates are generated elsewhere, it seems


namespace bluefin {

	using antlr4::tree::ParseTree;
	using std::map;
	using std::pair;
	using std::dynamic_pointer_cast;
	using std::shared_ptr;
	using TP = Type::Possibility;

	class DecorateExprWithTypes : public bluefinBaseListener
	{
	public:

		// For testing, we'll pass in an adapter of a symbol table
		DecorateExprWithTypes(map<ParseTree*, shared_ptr<Scope>> scopeOfPrimaryIds, SymbolFactory& factory) :
			scopeOfPrimaryIdsVarDeclAndFuncDefs{ scopeOfPrimaryIds }, symbolFactory{ factory }, currFuncDefCtx{ nullptr }
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
		void exitVarDecl(bluefinParser::VarDeclContext*) override;
		void exitStmtReturn(bluefinParser::StmtReturnContext*) override;
		void exitStmtIf(bluefinParser::StmtIfContext*) override;
		void exitStmtWhile(bluefinParser::StmtWhileContext*) override;

		// used to setting the current funcDefContext so that a return expr can compare its type with its enclosing function's type
		void enterFuncDef(bluefinParser::FuncDefContext*) override;
		void exitFuncDef(bluefinParser::FuncDefContext*) override;

		inline map<ParseTree*, TypeContext> getTypeContexts() { return typeContexts; }

	private:
		SymbolFactory& symbolFactory;
		map<ParseTree*, shared_ptr<Scope>> scopeOfPrimaryIdsVarDeclAndFuncDefs;

		map<ParseTree*, TypeContext> typeContexts; // stores the type associated with expressions and functions. Enables type checking
		bluefinParser::FuncDefContext* currFuncDefCtx;

		shared_ptr<Symbol> resolve(const string name, shared_ptr<Scope> startScope);

		// ==== How to update type
		// 1. When we visit a primary, compute its type
		// 2. When we visit a non-terminal 
		//      a) For arithmetic expr, compute its own type from its children's type. Then check whether this expression has compatible types
		//			THen compute its children's promoteTo type
		//		b) For equality and relational, compute children's promotTo type and set own type to bool. Same wtih promoteTo type (since that won't change)
		shared_ptr<Type> getArithmeticExprType(shared_ptr<Type> left, shared_ptr<Type> right);
		shared_ptr<Type> getPromotionType(shared_ptr<Type> left, shared_ptr<Type> right);

		bool areSameTypes(shared_ptr <Type> left, shared_ptr<Type> right) const;
		bool areBothBoolType(shared_ptr<Type> left, shared_ptr<Type> right) const;
		bool areArithmeticallyCompatible(shared_ptr<Type> left, shared_ptr<Type> right) const;
		bool isAssignmentCompatible(shared_ptr<Type> left, shared_ptr<Type> right) const;

		// can't use pair with unordered_map here b/c pair doesn't have a hash key
		const map<pair<TP, TP>, shared_ptr<Type>> arithmeticExprType{ 
			{{TP::INT, TP::INT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::INT))))},
			{{TP::INT, TP::FLOAT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))},
			{{TP::FLOAT, TP::INT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))},
			{{TP::FLOAT, TP::FLOAT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))}
		};

		const map<pair<TP, TP>, shared_ptr<Type>> promotionFromTo{
			{{TP::BOOL, TP::BOOL}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL))))},
			{{TP::INT, TP::FLOAT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))},
			{{TP::INT, TP::INT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::INT))))},
			{{TP::FLOAT, TP::INT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))},
			{{TP::FLOAT, TP::FLOAT}, dynamic_pointer_cast<BuiltinTypeSymbol>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT))))}
		};
	};
}
