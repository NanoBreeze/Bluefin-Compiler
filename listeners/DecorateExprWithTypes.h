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
#include "../symbolTable/EventObserver.h"

using bluefin::TypeContext;

namespace bluefin {

	using antlr4::tree::ParseTree;
	using std::map;
	using std::pair;
	using std::dynamic_pointer_cast;
	using std::shared_ptr;

	class DecorateExprWithTypes : public bluefinBaseListener
	{
	public:

		DecorateExprWithTypes(SymbolFactory& factory, SymbolTable& symTab) :
			symbolFactory{ factory }, symbolTable{ symTab }, currFuncDefCtx{ nullptr }
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
		void exitMethodCall(bluefinParser::MethodCallContext*) override;
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
		void attachEventObserver(shared_ptr<EventObserver>);
		void detachEventObserver(shared_ptr<EventObserver>); // is this even called? If arg not found, no error would be thrown

	private:
		SymbolFactory& symbolFactory;
		SymbolTable& symbolTable;

		/* ==== How to compute type
		1. When we visit a primaryId, compute its type
		2. When we visit a non-terminal expression node, check children nodes (operands) have compatible type for this operator. 
	       Just because they have same type doesn't mean they are compatible with operator. eg) a + b; is not compatible if 'a' and 'b' are both structs. 
			a) If compatible, set both node's promoteTo type and set own node's evalType to the promoteToType
			b) If one of the node's evalType is null, then an error must have already occured. Set this node's evalType to null and don't broadcast error (don't want to duplicate it)
			c) If not compatible, broadcast error and set its own node's evalType to null
		3. For return statements, also check the type of the expr matches the (return) type of function
		4. For if/while statements, check that the expr has binary type
		5. Verify that for VarDecl, rhs and lhs match
		6. Verify function call parameter types. Also verify that the symbol indeed resolves to a FunctionSymbol. For StructSymbol, it should already have 
			been resolved in Resolution phase.
		NOTE: if a subexpression has an error, it will be broadcasted. Its parents' errors won't be broadcasted, to
		avoid too many unnecessary errors going around.
		*/
		map<ParseTree*, TypeContext> typeContexts; // stores the type associated with expressions and functions. Enables type checking

		bluefinParser::FuncDefContext* currFuncDefCtx;
		bool currFuncDefHasDirectReturnStmt;

		vector<shared_ptr<EventObserver>> eventObservers;

		Type getPromotionType(Type left, Type right);
		bool isBinaryOperatorOperandCompatible(string op, Type lhs, Type rhs) const;
		bool isUnaryOperatorOperandCompatible(string op, Type lhs) const;

		bool isSubExprTypeUsable(Type) const; // a helper to support our type computing algo

		void broadcastEvent(SimpleTypeErrorEvent, Type lhs, Type rhs = Type{ "" });
		void broadcastEvent(OperatorTypeErrorEvent, string op, Type lhs, Type rhs = Type{ "" });
		void broadcastEvent(FunctionCallTypeErrorEvent, string funcName, size_t argCount, size_t paramCount, bool isMethod);

		const map<pair<Type, Type>, Type> promotionFromTo{
			{{Type::BOOL(), Type::BOOL()}, Type::BOOL()},
			{{Type::INT(), Type::FLOAT()}, Type::FLOAT()},
			{{Type::INT(), Type::INT()}, Type::INT()},
			{{Type::FLOAT(), Type::INT()}, Type::FLOAT()},
			{{Type::FLOAT(), Type::FLOAT()}, Type::FLOAT()}
		};
	};
}
