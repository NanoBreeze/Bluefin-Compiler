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

	exprTypeContexts.emplace(ctx, 
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::INT)))), //what the heck is this monstrosity!
		}
	); // what is this monstrosity?
}

void DecorateExprWithTypes::exitPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx) {
	exprTypeContexts.emplace(ctx, 
		TypeContext{ 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::FLOAT)))),
		}
	);
}

void DecorateExprWithTypes::exitPrimaryString(bluefinParser::PrimaryStringContext* ctx) {
	exprTypeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::STRING)))),
		}
	);
}

void DecorateExprWithTypes::exitPrimaryBool(bluefinParser::PrimaryBoolContext* ctx) {
	exprTypeContexts.emplace(ctx,
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
		exprTypeContexts.emplace(ctx, TypeContext { resolvedSym->getType() });
	}
}

void DecorateExprWithTypes::exitPrimaryParenth(bluefinParser::PrimaryParenthContext* ctx) {
	shared_ptr<Type> subExprType = exprTypeContexts.at(ctx->expr()).getEvalType(); // don't use [] b/c TypeContext doesn't have default ctor
	exprTypeContexts.emplace(ctx, TypeContext{ subExprType });
}

void DecorateExprWithTypes::exitFuncCall(bluefinParser::FuncCallContext* ctx) {
	shared_ptr<Type> subExprType = exprTypeContexts.at(ctx->expr()).getEvalType();
	exprTypeContexts.emplace(ctx, TypeContext { subExprType });
}

void DecorateExprWithTypes::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx) {
	shared_ptr<Type> subExprType = exprTypeContexts.at(ctx->expr()).getEvalType();
	exprTypeContexts.emplace(ctx, TypeContext { subExprType });
}

void DecorateExprWithTypes::exitMultiExpr(bluefinParser::MultiExprContext* ctx) {
	
	// TODO: get should be reference type
	TypeContext& leftTypeContext = exprTypeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = exprTypeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	exprTypeContexts.emplace(ctx, TypeContext { exprType });

}

void DecorateExprWithTypes::exitAddExpr(bluefinParser::AddExprContext* ctx) {
	// TODO: should be reference type
	TypeContext& leftTypeContext = exprTypeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = exprTypeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	exprTypeContexts.emplace(ctx, TypeContext { exprType });

}

void DecorateExprWithTypes::exitRelExpr(bluefinParser::RelExprContext* ctx) {
	exprTypeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);

	// may need to promote children's type. eg, float > int
	TypeContext& leftTypeContext = exprTypeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = exprTypeContexts.at(ctx->expr(1));

	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
}

void DecorateExprWithTypes::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx) {
	exprTypeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);

	// may need to convert children's type. eg, float > int
	TypeContext& leftTypeContext = exprTypeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = exprTypeContexts.at(ctx->expr(1));

	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));
}

void DecorateExprWithTypes::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx) {
	exprTypeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);
}

void DecorateExprWithTypes::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx) {
	exprTypeContexts.emplace(ctx,
		TypeContext { 
			dynamic_pointer_cast<Type>(shared_ptr<Symbol>(move(symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::BuiltinType::BOOL)))),
		}
	);
}

void DecorateExprWithTypes::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx) {

	// TODO: get should be reference type
	TypeContext& leftTypeContext = exprTypeContexts.at(ctx->expr(0));
	TypeContext& rightTypeContext = exprTypeContexts.at(ctx->expr(1));
	leftTypeContext.setPromotionType(
		getPromotionType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType()));
	rightTypeContext.setPromotionType(
		getPromotionType(rightTypeContext.getEvalType(), leftTypeContext.getEvalType()));

	shared_ptr<Type> exprType = getArithmeticExprType(leftTypeContext.getEvalType(), rightTypeContext.getEvalType());
	exprTypeContexts.emplace(ctx, TypeContext { exprType });
}

void DecorateExprWithTypes::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {
	// put type of member. Eg, first.a could have type int
	TypeContext& typeContext = exprTypeContexts.at(ctx->expr());

	shared_ptr<StructSymbol> structType = static_pointer_cast<StructSymbol>(typeContext.getEvalType());
	shared_ptr<Type> memberType = structType->resolveMember(ctx->ID()->getText())->getType();
	exprTypeContexts.emplace(ctx, TypeContext { memberType });
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
