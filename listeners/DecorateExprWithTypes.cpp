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

using SFB = SymbolFactory::Builtin;

//=============== listener methods for expressions
void DecorateExprWithTypes::exitPrimaryInt(bluefinParser::PrimaryIntContext* ctx) {

	typeContexts.emplace(ctx, 
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::INT)), 
		}
	); // what is this monstrosity?
}

void DecorateExprWithTypes::exitPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx) {
	typeContexts.emplace(ctx, 
		TypeContext{ 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::FLOAT))
		}
	);
}

void DecorateExprWithTypes::exitPrimaryString(bluefinParser::PrimaryStringContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::STRING))
		}
	);
}

void DecorateExprWithTypes::exitPrimaryBool(bluefinParser::PrimaryBoolContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::BOOL))
		}
	);
}

void DecorateExprWithTypes::exitPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	shared_ptr<Scope> scope = scopeOfPrimaryIdsVarDeclAndFuncDefs.at(ctx);
	shared_ptr<Symbol> resolvedSym = resolve(ctx->ID()->getText(), scope);

	if (resolvedSym) // maybe the symbol is unresolved?
	{
		typeContexts.emplace(ctx, TypeContext { resolvedSym->getType() });
	}
}

void DecorateExprWithTypes::exitPrimaryParenth(bluefinParser::PrimaryParenthContext* ctx) {
	shared_ptr<Type> subExprType = typeContexts.at(ctx->expr()).getEvalType(); // don't use [] b/c TypeContext doesn't have default ctor
	typeContexts.emplace(ctx, TypeContext{ subExprType });
}

void DecorateExprWithTypes::exitFuncCall(bluefinParser::FuncCallContext* ctx) {

	// make sure that for f(), f really is a function symbol and not, say, an int
	shared_ptr<Scope> scope = scopeOfPrimaryIdsVarDeclAndFuncDefs.at(ctx);

	if (shared_ptr<FunctionSymbol> fSym = 
		dynamic_pointer_cast<FunctionSymbol>(resolve(ctx->ID()->getText(), scope))) {
		
		typeContexts.emplace(ctx, TypeContext { fSym->getType() });

		// check that the number of arguments matches the number of params
		vector<shared_ptr<Symbol>> params = fSym->getParams();
		size_t numArgs = ctx->argList() ? ctx->argList()->expr().size() : 0;

		if (numArgs == params.size()) {
			// now check that all args can be promoted to the right type (implicity assignment)
			for (size_t i = 0; i < params.size(); i++) {
				shared_ptr<Type> curParamType = params[i]->getType();
				ParseTree* curArgCtx = ctx->argList()->expr(i);
				TypeContext& curArgTypeCxt = typeContexts.at(curArgCtx);

				if (isAssignmentCompatible(curParamType, curArgTypeCxt.getEvalType())) {
					curArgTypeCxt.setPromotionType(
						getPromotionType(curParamType, curArgTypeCxt.getEvalType()));
				}
				else {
					cerr << "Argument and param must have compatible types" << endl;
				}
			}
		}
		else {
			cerr << "Function call " << ctx->ID()->getText() << " doesn't have same number of args as function param" << endl;
		}
	}
	else {
		cerr << "func call must be a function type. Eg, for f(1,2), f must be a FunctionSymbol" << endl;
	}
}

void bluefin::DecorateExprWithTypes::exitMethodCall(bluefinParser::MethodCallContext* ctx)
{
	TypeContext& structMemberTypeContext = typeContexts.at(ctx->expr());

	if (shared_ptr<StructSymbol> structType =
		dynamic_pointer_cast<StructSymbol>(structMemberTypeContext.getEvalType())) { //almost identical to functionCall

		if (shared_ptr<FunctionSymbol> methodSym =
			dynamic_pointer_cast<FunctionSymbol>(structType->resolveMember(ctx->ID()->getText()))) {

			typeContexts.emplace(ctx, TypeContext{ methodSym->getType() });

			// check that the number of arguments matches the number of params
			vector<shared_ptr<Symbol>> params = methodSym->getParams();
			size_t numArgs = ctx->argList() ? ctx->argList()->expr().size() : 0;

			if (numArgs == params.size()) {
				// now check that all args can be promoted to the right type (implicity assignment)
				for (size_t i = 0; i < params.size(); i++) {
					shared_ptr<Type> curParamType = params[i]->getType();
					ParseTree* curArgCtx = ctx->argList()->expr(i);
					TypeContext& curArgTypeCxt = typeContexts.at(curArgCtx);

					if (isAssignmentCompatible(curParamType, curArgTypeCxt.getEvalType())) {
						curArgTypeCxt.setPromotionType(
							getPromotionType(curParamType, curArgTypeCxt.getEvalType()));

						typeContexts.emplace(ctx, TypeContext{ methodSym->getType() });
					}
					else {
						cerr << "Argument and param must have compatible types" << endl;
					}
				}
			}
			else {
				cerr << "Method call " << ctx->ID()->getText() << " doesn't have same number of args as function param" << endl;
			}
		}
		else {
			cerr << "method call must be a function type. Eg, for f(1,2), f must be a FunctionSymbol" << endl;
		}
	}
	else {
		cerr << "id must be a struct type. Eg, x.y, x must be a struct type" << endl;
	}



	
}

void DecorateExprWithTypes::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx) {
	shared_ptr<Type> subExprType = typeContexts.at(ctx->expr()).getEvalType();

	if (ctx->op->getText() == "!") {
		if (subExprType == BuiltinTypeSymbol::BOOL()) {
			cerr << "Bad unary ! operand type" << endl;
		}
	}
	else { // - (negative)
		if (subExprType != BTS::INT() && subExprType != BTS::FLOAT()) {
			cerr << "Bad unary - operand type" << endl;
		}
	}
	typeContexts.emplace(ctx, TypeContext { subExprType });
}

void DecorateExprWithTypes::exitMultiExpr(bluefinParser::MultiExprContext* ctx) {
	
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (areArithmeticallyCompatible(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		leftTypeContext.setPromotionType(
			getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
		rightTypeContext.setPromotionType(
			getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

		shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		typeContexts.emplace(ctx, TypeContext { exprType });
	}
	else {
		cerr << "Bad multiExpr operand types" << endl;
	}
}

void DecorateExprWithTypes::exitAddExpr(bluefinParser::AddExprContext* ctx) {
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (areArithmeticallyCompatible(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		leftTypeContext.setPromotionType(
			getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
		rightTypeContext.setPromotionType(
			getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

		shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		typeContexts.emplace(ctx, TypeContext { exprType });
	}
	else {
		cerr << "Bad addExpr operand types" << endl;
	}
}

void DecorateExprWithTypes::exitRelExpr(bluefinParser::RelExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::BOOL))
		}
	);

	// may need to promote children's type. eg, float > int
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (areArithmeticallyCompatible(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		leftTypeContext.setPromotionType(
			getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
		rightTypeContext.setPromotionType(
			getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
	}
	else {
		cerr << "Bad relExpr operand types" << endl;
	}
}

void DecorateExprWithTypes::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::BOOL))
		}
	);

	// may need to convert children's type. eg, float > int
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	// if lhs and rhs are both the same type or can be promoted to match each other (aka, arithmetically compatible), then they are valid semantics
	if (leftTypeContext.getEvalType() == rightTypeContext.getEvalType() ||	
		areArithmeticallyCompatible(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {

		leftTypeContext.setPromotionType(
			getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
		rightTypeContext.setPromotionType(
			getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
	}
	else {
		cerr << "Bad equalityExpr operand types" << endl;
	}

}

void DecorateExprWithTypes::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::BOOL))
		}
	);

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (areBothBoolType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		//
	}
	else {
		cerr << "Bad logicalANDExpr not good operand types" << endl;
	}
}

void DecorateExprWithTypes::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(symbolFactory.createBuiltinTypeSymbol(SFB::BOOL))
		}
	);

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (areBothBoolType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		//
	}
	else {
		cerr << "Bad logicalORExpr not good operand types" << endl;
	}

}

void DecorateExprWithTypes::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx) {

	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	if (isAssignmentCompatible(leftTypeContext.getEvalType(), rightTypeContext.getEvalType())) {
		rightTypeContext.setPromotionType(
			getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

		shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
		typeContexts.emplace(ctx, TypeContext { exprType });
	}
	else {
		cerr << "Bad assignment expr" << endl;
	}
}


void DecorateExprWithTypes::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {
	// put type of member. Eg, first.a could have type int
	TypeContext& typeContext = typeContexts.at(ctx->expr());
	if (shared_ptr<StructSymbol> structType =
		dynamic_pointer_cast<StructSymbol>(typeContext.getEvalType())) {

		shared_ptr<Type> memberType = structType->resolveMember(ctx->ID()->getText())->getType();
		typeContexts.emplace(ctx, TypeContext { memberType });
	}
	else {
		cerr << "id must be a struct type. Eg, x.y, x must be a struct type" << endl;
	}
}

// It doesn't make sense for a var declaration to have a type (the varsymbol can though)
// b/c it would have no meaning. 
// However, the rhs variable may need to be promoted
// eg, float a = 56
void DecorateExprWithTypes::exitVarDecl(bluefinParser::VarDeclContext* ctx) {
	if (ctx->expr()) {

		// find type of the variable
		shared_ptr<Scope> scope = scopeOfPrimaryIdsVarDeclAndFuncDefs.at(ctx);
		shared_ptr<Symbol> resolvedSym = resolve(ctx->ID()->getText(), scope);

		TypeContext& rightTypeContext = typeContexts.at(ctx->expr());

		if (isAssignmentCompatible(resolvedSym->getType(), rightTypeContext.getEvalType())) {
			
			// promotion is only possible for arithmetically compatible types.
			// TODO: I feel this layer of abstraction requires some thinking. What if promotion should be allowed in diff contexts
			if (areArithmeticallyCompatible(resolvedSym->getType(), rightTypeContext.getEvalType())) {
				rightTypeContext.setPromotionType(
					getPromotionType(rightTypeContext.getEvalType(), resolvedSym->getType()));
			}
		}
		else {
			cerr << "Bad varDecl expr" << endl;
		}
	}
}

// sets and unsets the current funcDefContext so that a return expr can compare its type with its enclosing function's type
void DecorateExprWithTypes::enterFuncDef(bluefinParser::FuncDefContext* ctx) {
	currFuncDefCtx = ctx;
}

void DecorateExprWithTypes::exitFuncDef(bluefinParser::FuncDefContext* ctx) {
	currFuncDefCtx = nullptr;
}

/* We're assuming a return statement will only occur within a function*/
void DecorateExprWithTypes::exitStmtReturn(bluefinParser::StmtReturnContext* ctx)  { 
	// match expr type with the function's return type

	// How do we get a stmtReturn's enclosing function?
	if (ctx->expr()) {
		TypeContext& retTypeCtx = typeContexts.at(ctx->expr());
		shared_ptr<Symbol> enclosingFuncSym = 
			resolve(currFuncDefCtx->ID()->getText(), scopeOfPrimaryIdsVarDeclAndFuncDefs.at(currFuncDefCtx)); // wow, this is long
		shared_ptr<Type> funcRetType = enclosingFuncSym->getType();

		if (isAssignmentCompatible(funcRetType, retTypeCtx.getEvalType())) {
			retTypeCtx.setPromotionType(getPromotionType(funcRetType, retTypeCtx.getEvalType()));
		}
		else {
			cerr << "Function return type and return expression's type must match" << endl;
		}
	}
}


void DecorateExprWithTypes::exitStmtIf(bluefinParser::StmtIfContext* ctx) {
	if (typeContexts.at(ctx->expr()).getEvalType() != BTS::BOOL()) {
		cerr << "if (expr) ... should be bool" << endl;
	}
}

void DecorateExprWithTypes::exitStmtWhile(bluefinParser::StmtWhileContext* ctx) {
	if (typeContexts.at(ctx->expr()).getEvalType() != BTS::BOOL()) {
		cerr << "if (expr) ... should be bool" << endl;
	}
}


shared_ptr<Symbol> DecorateExprWithTypes::resolve(const string name, shared_ptr<Scope> startScope) {

	shared_ptr<Scope> scope = startScope;

	do {
		shared_ptr<Symbol> sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = scope->getEnclosingScope());

	throw UnresolvedSymbolException(name);
}


bool DecorateExprWithTypes::areBothBoolType(shared_ptr<Type> left, shared_ptr<Type> right) const {
	return left == BTS::BOOL() && right == BTS::BOOL();
}

// Arithmetics (+,-,*,/) are allowed only on float and int
bool DecorateExprWithTypes::areArithmeticallyCompatible(shared_ptr<Type> left, shared_ptr <Type> right) const {
	return (left == BTS::INT() || left == BTS::FLOAT()) &&
		(right == BTS::INT() || right == BTS::FLOAT());
}

// used for simple assignment and for varDecl (eg, int a = 5;)
// Two types can be assignable if they are the same possibility or the rhs can be promoted to the lhs
bool DecorateExprWithTypes::isAssignmentCompatible(shared_ptr<Type> left, shared_ptr<Type> right) const {
	if (left == right) {
		return true;
	}

	// check if rhs can be promoted to lhs. The only promotion in this program is int->float, so then
	// we'll just check if lhs is float and rhs is int
	return left == BTS::FLOAT() && right == BTS::INT();
}

shared_ptr<Type> DecorateExprWithTypes::getArithmeticExprType(shared_ptr<Type> left, shared_ptr<Type> right) {
	return arithmeticExprType.at({ left, right });
}

shared_ptr<Type> DecorateExprWithTypes::getPromotionType(shared_ptr<Type> left,shared_ptr<Type>right) {
	return promotionFromTo.at({ left, right });
}
