#include "DecorateExprWithTypes.h"

#include <memory>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;
using std::move;
using antlr4::tree::ParseTreeProperty;

//=============== listener methods for expressions
void DecorateExprWithTypes::exitPrimaryInt(bluefinParser::PrimaryIntContext* ctx) {

	typeContexts.emplace(ctx, 
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::INT)))), //what the heck is this monstrosity!
		}
	); // what is this monstrosity?
}

void DecorateExprWithTypes::exitPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx) {
	typeContexts.emplace(ctx, 
		TypeContext{ 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT)))),
		}
	);
}

void DecorateExprWithTypes::exitPrimaryString(bluefinParser::PrimaryStringContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::STRING)))),
		}
	);
}

void DecorateExprWithTypes::exitPrimaryBool(bluefinParser::PrimaryBoolContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);
}

void DecorateExprWithTypes::exitPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	shared_ptr<Scope> scope = scopeOfPrimaryIds.at(ctx);
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
	shared_ptr<Type> subExprType = typeContexts.at(ctx->expr()).getEvalType();
	typeContexts.emplace(ctx, TypeContext { subExprType });
}

void DecorateExprWithTypes::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx) {
	shared_ptr<Type> subExprType = typeContexts.at(ctx->expr()).getEvalType();
	typeContexts.emplace(ctx, TypeContext { subExprType });
}

void DecorateExprWithTypes::exitMultiExpr(bluefinParser::MultiExprContext* ctx) {
	
	// TODO: get should be reference type
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	typeContexts.emplace(ctx, TypeContext { exprType });

}

void DecorateExprWithTypes::exitAddExpr(bluefinParser::AddExprContext* ctx) {
	// TODO: should be reference type
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	typeContexts.emplace(ctx, TypeContext { exprType });

}

void DecorateExprWithTypes::exitRelExpr(bluefinParser::RelExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);

	// may need to promote children's type. eg, float > int
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
}

void DecorateExprWithTypes::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);

	// may need to convert children's type. eg, float > int
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));

	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
}

void DecorateExprWithTypes::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);
}

void DecorateExprWithTypes::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx) {
	typeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);
}

void DecorateExprWithTypes::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx) {

	// TODO: get should be reference type
	TypeContext& leftTypeContext = typeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	typeContexts.emplace(ctx, TypeContext { exprType });
}

void DecorateExprWithTypes::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {
	// put type of member. Eg, first.a could have type int
	TypeContext& typeContext = typeContexts.at(ctx->expr());

	shared_ptr<StructSymbol> structType = static_pointer_cast<StructSymbol>(typeContext.getEvalType());
	shared_ptr<Type> memberType = structType->resolveMember(ctx->ID()->getText())->getType();
	typeContexts.emplace(ctx, TypeContext { memberType });
}

// It doesn't make sense for a var declaration to have a type (the varsymbol can though)
// b/c it would have no meaning. 
// However, the rhs variable may need to be promoted
// eg, float a = 56
void DecorateExprWithTypes::exitVarDecl(bluefinParser::VarDeclContext* ctx) {
	if (ctx->expr()) {

	// find type of the variable
	shared_ptr<Scope> scope = scopeOfPrimaryIds.at(ctx);
	shared_ptr<Symbol> resolvedSym = resolve(ctx->ID()->getText(), scope);
	
	TypeContext& rightTypeContext = typeContexts.at(ctx->expr());
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), resolvedSym->getType()));
	}
}

/* We're assuming a return statement will only occur within a function*/
void DecorateExprWithTypes::exitStmtReturn(bluefinParser::StmtReturnContext* ctx)  { 
	// match expr type with the function's return type

	/* How do we get a stmtReturn's enclosing function?
	if (ctx->expr()) {
		TypeContext& retTypeCtx = typeContexts.at(ctx->expr());

		retTypeCtx.setPromotionType(
			getPromotionType(funcRetType, retTypeCtx.getEvalType());
		)
	}
	*/
}


shared_ptr<Symbol> DecorateExprWithTypes::resolve(const string name, shared_ptr<Scope> startScope) {

	shared_ptr<Scope> scope = startScope;

	do {
		shared_ptr<Symbol> sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = scope->getEnclosingScope());

	throw UnresolvedSymbolException(name);
}

using TP = Type::Possibility;

shared_ptr<Type> DecorateExprWithTypes::getArithmeticExprType(shared_ptr<Type> left, shared_ptr<Type> right) {
	return arithmeticExprType.at({ left->getTypePossibility(), right->getTypePossibility() });
}

shared_ptr<Type> DecorateExprWithTypes::getPromotionType(shared_ptr<Type> left,shared_ptr<Type>right) {
	return promotionFromTo.at({ left->getTypePossibility(), right->getTypePossibility() });
}

