#include "DecorateExprWithTypes.h"

#include <memory>
#include <iostream>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;
using std::move;
using antlr4::tree::ParseTreeProperty;
using std::cerr; 
using std::endl;


/*
NOTE: by the time we're decorating expr with types, we expect all relevant symbols can be resolved, 
so calling symbolTable::resolve(..) should not throw an exception
*/

//=============== listener methods for expressions
void DecorateExprWithTypes::exitPrimaryInt(bluefinParser::PrimaryIntContext* ctx) {
	typeContexts.emplace(ctx, TypeContext { Type::INT() } ); 
}

void DecorateExprWithTypes::exitPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx) {
	typeContexts.emplace(ctx, TypeContext { Type::FLOAT() } );
}

void DecorateExprWithTypes::exitPrimaryString(bluefinParser::PrimaryStringContext* ctx) {
	typeContexts.emplace(ctx, TypeContext { Type::STRING() } );
}

void DecorateExprWithTypes::exitPrimaryBool(bluefinParser::PrimaryBoolContext* ctx) {
	typeContexts.emplace(ctx, TypeContext { Type::BOOL() } );
}

void DecorateExprWithTypes::exitPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	string varName = ctx->ID()->getText();
	shared_ptr<Symbol> resolvedSym = symbolTable.getResolvedSymbol(ctx);

	typeContexts.emplace(ctx, TypeContext { resolvedSym->getType() });
}

void DecorateExprWithTypes::exitPrimaryParenth(bluefinParser::PrimaryParenthContext* ctx) {
	Type subExprType = typeContexts.at(ctx->expr()).getEvalType(); // don't use [] b/c TypeContext doesn't have default ctor
	typeContexts.emplace(ctx, TypeContext{ subExprType });
}

void DecorateExprWithTypes::exitFuncCall(bluefinParser::FuncCallContext* ctx) {

	// make sure that for f(), f really is a function symbol and not, say, an int
	string funcName = ctx->ID()->getText();
	shared_ptr<Scope> scope = symbolTable.getScope(ctx);

	if (shared_ptr<FunctionSymbol> fSym = dynamic_pointer_cast<FunctionSymbol>(symbolTable.getResolvedSymbol(ctx))) {

		typeContexts.emplace(ctx, TypeContext { fSym->getType() });

		// check that the number of arguments matches the number of params
		vector<shared_ptr<Symbol>> params = fSym->getParams();
		size_t numArgs = ctx->argList() ? ctx->argList()->expr().size() : 0;

		if (numArgs == params.size()) {
			// now check that all args can be promoted to the right type (implicity assignment)
			for (size_t i = 0; i < params.size(); i++) {
				Type curParamType = params[i]->getType();
				ParseTree* curArgCtx = ctx->argList()->expr(i);
				TypeContext& curArgTypeCxt = typeContexts.at(curArgCtx);

				if (isSubExprTypeUsable(curArgTypeCxt.getEvalType())) {
					if (isBinaryOperatorOperandCompatible("=", curParamType, curArgTypeCxt.getEvalType())) {
						curArgTypeCxt.setPromotionType(getPromotionType(curArgTypeCxt.getEvalType(), curParamType));
					}
					else {
						typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
						broadcastEvent(SimpleTypeErrorEvent::INCOMPATIBLE_ARG_PARAM_TYPE, curParamType, curArgTypeCxt.getEvalType());
						return;
					}
				}
				else {
					return; // if a subexpr, which makes up one of the arguments had failed, don't broadcast the function error. 
					// We want the user to fix that one first
				}
			}
		}
		else {
			broadcastEvent(FunctionCallTypeErrorEvent::ARGS_AND_PARAMS_COUNT_DIFFER, ctx->ID()->getText(), numArgs, params.size(), false);
			typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
		}
	}
	else {
		cerr << "func call must be a function type. Eg, for f(1,2), f must be a FunctionSymbol" << endl;
		typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
		throw std::exception("At this point, all FunctionSymbol should have already resolved in Resolution stage. This exception should not be possible. ");
	}
}

void bluefin::DecorateExprWithTypes::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
	TypeContext& structMemberTypeContext = typeContexts.at(ctx->expr());

	if (structMemberTypeContext.getEvalType().isUserDefinedType()) {
		shared_ptr<StructSymbol> structType = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(structMemberTypeContext.getEvalType()));

		string methodName = ctx->ID()->getText();
		if (shared_ptr<FunctionSymbol> methodSym =
			dynamic_pointer_cast<FunctionSymbol>(symbolTable.resolveMember(methodName, structType))) {

			typeContexts.emplace(ctx, TypeContext{ methodSym->getType() });

			// check that the number of arguments matches the number of params
			vector<shared_ptr<Symbol>> params = methodSym->getParams();
			size_t numArgs = ctx->argList() ? ctx->argList()->expr().size() : 0;

			if (numArgs == params.size()) {
				// now check that all args can be promoted to the right type (implicity assignment)
				for (size_t i = 0; i < params.size(); i++) {
					Type curParamType = params[i]->getType();
					ParseTree* curArgCtx = ctx->argList()->expr(i);
					TypeContext& curArgTypeCxt = typeContexts.at(curArgCtx);

					if (isSubExprTypeUsable(curArgTypeCxt.getEvalType())) {
						if (isBinaryOperatorOperandCompatible("=", curParamType, curArgTypeCxt.getEvalType())) {
							curArgTypeCxt.setPromotionType(getPromotionType(curArgTypeCxt.getEvalType(), curParamType));

							typeContexts.emplace(ctx, TypeContext { curArgTypeCxt.getPromotionType() }); // TODO: I think this line should be removed
						}
						else {
							typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
							broadcastEvent(SimpleTypeErrorEvent::INCOMPATIBLE_ARG_PARAM_TYPE, curParamType, curArgTypeCxt.getEvalType());
							return;
						}
					}
					else {
						return;
					}
				}
			}
			else {
				broadcastEvent(FunctionCallTypeErrorEvent::ARGS_AND_PARAMS_COUNT_DIFFER, ctx->ID()->getText(), numArgs, params.size(), true);
			}
		}
		else {
			cerr << "method call must be a function type. Eg, for f(1,2), f must be a FunctionSymbol" << endl;
			throw std::exception("At this point, all FunctionSymbol should have already resolved in Resolution stage. This exception should not be possible. ");
		}
	}
	else {
		cerr << "id must be a struct type. Eg, x.y, x must be a struct type" << endl;
		throw std::exception("At this point, all struct members should have already resolved in Resolution stage. This exception should not be possible. ");
	}
}

void DecorateExprWithTypes::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx) {
	Type subExprType = typeContexts.at(ctx->expr()).getEvalType();
	if (isSubExprTypeUsable(subExprType)) {
		if (isUnaryOperatorOperandCompatible(ctx->op->getText(), subExprType)) {
			typeContexts.emplace(ctx, TypeContext { subExprType });
		}
		else {
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_UNARY_OPERATOR_OPERAND, ctx->op->getText(), subExprType);
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitMultiExpr(bluefinParser::MultiExprContext* ctx) {
	
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible(ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			Type typeOfCurrentExpr = getPromotionType(leftTypeContext.getPromotionType(), rightTypeContext.getPromotionType());
			typeContexts.emplace(ctx, TypeContext { typeOfCurrentExpr });
		}
		else {
			typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitAddExpr(bluefinParser::AddExprContext* ctx) {

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible(ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			Type typeOfCurrentExpr = getPromotionType(leftTypeContext.getPromotionType(), rightTypeContext.getPromotionType());
			typeContexts.emplace(ctx, TypeContext { typeOfCurrentExpr });
		}
		else {
			typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext { Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitRelExpr(bluefinParser::RelExprContext* ctx) {
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible(ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			typeContexts.emplace(ctx, TypeContext{ Type::BOOL() });
		}
		else {
			typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx) {

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible(ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			typeContexts.emplace(ctx, TypeContext{ Type::BOOL() });
		}
		else {
			typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, ctx->op->getText(), leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx) {

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible("&&", leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			typeContexts.emplace(ctx, TypeContext{ Type::BOOL() });
		}
		else {
			typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, "&&", leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx) {
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible("||", leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			typeContexts.emplace(ctx, TypeContext{ Type::BOOL() });
		}
		else {
			typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, "||", leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
	}
}

void DecorateExprWithTypes::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx) {
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isSubExprTypeUsable(leftTypeContext.getEvalType()) && isSubExprTypeUsable(rightTypeContext.getEvalType())) {
		if (isBinaryOperatorOperandCompatible("=", leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
			leftTypeContext.setPromotionType(
				getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
			rightTypeContext.setPromotionType(
				getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

			//typeContexts.emplace(ctx, TypeContext{ leftTypeContext.getEvalType() });
			typeContexts.emplace(ctx, TypeContext{ leftTypeContext.getPromotionType() }); // doesn't really matter here whether eval or promo type
		}
		else {
			typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
			broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, "=", leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		}
	}
	else {
		typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
	}
}


void DecorateExprWithTypes::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {

	// put type of member. Eg, first.a could have type int
	TypeContext& typeContext = typeContexts.at(ctx->expr());
	if (typeContext.getEvalType().isUserDefinedType()) {
		shared_ptr<StructSymbol> structSym = dynamic_pointer_cast<StructSymbol>(symbolTable.getSymbolMatchingType(typeContext.getEvalType())); // at this point, we expect this to pass

		Type memberType = symbolTable.resolveMember(ctx->ID()->getText(), structSym)->getType();
		typeContexts.emplace(ctx, TypeContext { memberType });
	}
	else {
		cerr << "id must be a struct type. Eg, x.y, x must be a struct type" << endl;
		throw std::exception("At this point, all struct members should have already resolved in Resolution stage. This exception should not be possible. ");
	}
}

// It doesn't make sense for a var declaration to have a type (the varsymbol can though) // b/c it would have no meaning. 
// However, the rhs variable may need to be promoted
// eg, float a = 56
void DecorateExprWithTypes::exitVarDecl(bluefinParser::VarDeclContext* ctx) {
	if (ctx->expr()) {

		// find type of the variable
		string varName = ctx->ID()->getText();
		shared_ptr<Symbol> sym = symbolTable.getSymbol(ctx);

		TypeContext& rightTypeContext = typeContexts.at(ctx->expr());

		if (isSubExprTypeUsable(rightTypeContext.getEvalType())) {
			if (isBinaryOperatorOperandCompatible("=", sym->getType(), rightTypeContext.getEvalType())) {

				// promote rhs type only if it isn't a user-defined type
				if (!sym->getType().isUserDefinedType())
					rightTypeContext.setPromotionType(getPromotionType(rightTypeContext.getEvalType(), sym->getType()));

				//Don't place ctx into typeContexts b/c it's a VarDecl, so it wouldn't make sense anyways
			}
			else {
				typeContexts.emplace(ctx, TypeContext{ Type::getUnusableType() });
				broadcastEvent(OperatorTypeErrorEvent::INCOMPATIBLE_BINARY_OPERATOR_OPERAND, "=", sym->getType(), rightTypeContext.getEvalType());
			}
		}
		// else {} if not usable, the expr's error would have already broadcasted, so no need to broadcast it again
	}
}

// sets and unsets the current funcDefContext so that a return expr can compare its type with its enclosing function's type
void DecorateExprWithTypes::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
	currFuncDefCtx = ctx;
	currFuncDefHasDirectReturnStmt = false;
}

void DecorateExprWithTypes::exitFuncDef(bluefinParser::FuncDefContext* ctx) {
	Type funcRetType = symbolTable.getSymbol(currFuncDefCtx)->getType();
	if (funcRetType != Type::VOID() && !currFuncDefHasDirectReturnStmt) //ensure that non-void function contains a return statement directly inside it
		broadcastEvent(SimpleTypeErrorEvent::MISSING_RETURN_STATEMENT, funcRetType);

	currFuncDefCtx = nullptr;
}

/* We're assuming a return statement will only occur within a function*/
// match expr type with the function's return type
void DecorateExprWithTypes::exitStmtReturn(bluefinParser::StmtReturnContext* ctx)  { 

	Type funcRetType = symbolTable.getSymbol(currFuncDefCtx)->getType();

	// How do we get a stmtReturn's enclosing function?
	if (ctx->expr()) {
		string enclosingFuncName = currFuncDefCtx->ID()->getText();
		shared_ptr<Scope> scopeOfFunc = symbolTable.getScope(currFuncDefCtx); 
		TypeContext& retTypeCtx = typeContexts.at(ctx->expr());

		if (isSubExprTypeUsable(retTypeCtx.getEvalType())) {
			if (isBinaryOperatorOperandCompatible("=", funcRetType, retTypeCtx.getEvalType())) {
				retTypeCtx.setPromotionType(getPromotionType(funcRetType, retTypeCtx.getEvalType()));
			}
			else {
				broadcastEvent(SimpleTypeErrorEvent::RETURN_INVALID_TYPE, funcRetType, retTypeCtx.getEvalType());
			}
		}
		// else {} the expr has problems inside it, which has already been reported, so no need to report the return expr problem
	}
	else {
		// Check the current function's return type. If it's void, then ok for return to not contain expr. But if function's return type
		// is not void, then this "return" statement is missing an expr
		if (symbolTable.getSymbol(currFuncDefCtx)->getType() != Type::VOID()) {
			broadcastEvent(SimpleTypeErrorEvent::RETURN_INVALID_TYPE, funcRetType, Type::VOID());
		}
	}

	if (ctx->parent->parent == currFuncDefCtx)
		currFuncDefHasDirectReturnStmt = true;	// this return statement is directly inside the current function, so MISSING_RETURN_STATEMENT won't appear
}

void DecorateExprWithTypes::exitStmtIf(bluefinParser::StmtIfContext* ctx) {
	TypeContext& subExpr = typeContexts.at(ctx->expr());

	if (isSubExprTypeUsable(subExpr.getEvalType())) {
		if (typeContexts.at(ctx->expr()).getEvalType() != Type::BOOL()) {
			broadcastEvent(SimpleTypeErrorEvent::IF_STATEMENT_NOT_BOOL, typeContexts.at(ctx->expr()).getEvalType());
		}
	}
	// if not usable type, the error for that had already been broadcasted, so don't broadcast an if statement error again
}

void DecorateExprWithTypes::exitStmtWhile(bluefinParser::StmtWhileContext* ctx) {
	TypeContext& subExpr = typeContexts.at(ctx->expr());

	if (isSubExprTypeUsable(subExpr.getEvalType())) {
		if (typeContexts.at(ctx->expr()).getEvalType() != Type::BOOL()) {
			broadcastEvent(SimpleTypeErrorEvent::WHILE_STATEMENT_NOT_BOOL, typeContexts.at(ctx->expr()).getEvalType());
		}
	}
}


// If both types are built in types, use the lookup table to find promotion type
// otherwise, the promotion type is whichever one is the base type of the other
Type DecorateExprWithTypes::getPromotionType(Type left, Type right) {
	if (left.isUserDefinedType()) {
		assert(right.isUserDefinedType());
		// get their base type
		if (symbolTable.isParentType(left, right)) {
			return right;
		}
		return left;
	}
	return promotionFromTo.at({ left, right });
}

bool DecorateExprWithTypes::isUnaryOperatorOperandCompatible(string op, Type type) const {

	assert(op == "-" || op == "!");

	if (op == "!") {
		return type == Type::BOOL();
	}
	else if (op == "-") {
		return type == Type::FLOAT() || type == Type::INT();
	}
	throw std::exception("Only two unary operators allowed. This exception should never be thrown");
	return false;
}

bool DecorateExprWithTypes::isBinaryOperatorOperandCompatible(string op, Type lhs, Type rhs) const {
	assert(op == "*" || op == "/" || op == "+" || op == "-" || op == "<" || op == ">" || op == "<=" || op == ">=" ||
		op == "==" || op == "!=" || op == "&&" || op == "||" || op == "=");

	if (op == "*" || op == "/" || op == "+" || op == "-" || op == "<" || op == ">" || op == "<=" || op == ">=") {
		// only allowed if both lhs and rhs are either int or float
		return (lhs == Type::INT() || lhs == Type::FLOAT()) && (rhs == Type::INT() || rhs == Type::FLOAT());
	}
	else if (op == "&&" || op == "||") {
		return lhs == Type::BOOL() && rhs == Type::BOOL();
	}
	else if (op == "=") {
		if (lhs == rhs) return true;
		if (symbolTable.isParentType(rhs, lhs)) return true; // child can be assigned to parent but not vice versa
		// eg) parent = child; // ok, but child = parent; // not ok. So, ok if lhs is parent of rhs

		return lhs == Type::FLOAT() && rhs == Type::INT();
	}
	else { //== or !=
		// valid only if exact same type, or they're both int/floats
		if (lhs == rhs) return true;
		return (lhs == Type::INT() || lhs == Type::FLOAT()) && (rhs == Type::INT() || rhs == Type::FLOAT());
	}
}

bool DecorateExprWithTypes::isSubExprTypeUsable(Type type) const {
	// If the type is empty, then it's not usable. If it's not empty, then usable
	return type != Type::getUnusableType();
}

void DecorateExprWithTypes::broadcastEvent(SimpleTypeErrorEvent e, Type lhs, Type rhs) {
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, lhs, rhs);
	}
}

void DecorateExprWithTypes::broadcastEvent(OperatorTypeErrorEvent e, string op, Type lhs, Type rhs) {
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, op, lhs, rhs);
	}
}

void DecorateExprWithTypes::broadcastEvent(FunctionCallTypeErrorEvent e, string funcName, size_t argCount, size_t paramCount, bool isMethod) {
	for ( shared_ptr<EventObserver> obs : eventObservers) {
		obs->onEvent(e, funcName, argCount, paramCount, isMethod);
	}
}

void DecorateExprWithTypes::attachEventObserver(shared_ptr<EventObserver> observer) {
	eventObservers.push_back(observer);
}

void DecorateExprWithTypes::detachEventObserver(shared_ptr<EventObserver> observer) {
	auto it = std::find(eventObservers.begin(), eventObservers.end(), observer);
	if (it != eventObservers.end()) {
		eventObservers.erase(it); 
	}
}
