#include "DecorateExprWithTypes.h"

#include <memory>

using namespace bluefin;
using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;


//=============== listener methods for expressions
void DecorateExprWithTypes::exitPrimaryInt(bluefinParser::PrimaryIntContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::INT)));
}

void DecorateExprWithTypes::exitPrimaryFloat(bluefinParser::PrimaryFloatContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::FLOAT)));
}

void DecorateExprWithTypes::exitPrimaryString(bluefinParser::PrimaryStringContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::STRING)));
}

void DecorateExprWithTypes::exitPrimaryBool(bluefinParser::PrimaryBoolContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL)));
}

void DecorateExprWithTypes::exitPrimaryId(bluefinParser::PrimaryIdContext* ctx) {
	shared_ptr<Scope> scope = scopeOfPrimaryIds.get(ctx);
	shared_ptr<Symbol> resolvedSym = resolve(ctx->ID()->getText(), scope);

	if (resolvedSym) // maybe the symbol is unresolved?
	{
		exprTypes.put(ctx, resolvedSym->getType()); 
	}
}

void DecorateExprWithTypes::exitPrimaryParenth(bluefinParser::PrimaryParenthContext* ctx) {
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr());
	exprTypes.put(ctx, subExprType);
}

void DecorateExprWithTypes::exitFuncCall(bluefinParser::FuncCallContext* ctx) {
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr());
	exprTypes.put(ctx, subExprType);
}

void DecorateExprWithTypes::exitUnaryExpr(bluefinParser::UnaryExprContext* ctx) {
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr());
	exprTypes.put(ctx, subExprType);
}

void DecorateExprWithTypes::exitMultiExpr(bluefinParser::MultiExprContext* ctx) {
	// assume both left and right operands have same type. Arbitrarily return left operand's type
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr(0));
	exprTypes.put(ctx, subExprType);
	
}

void DecorateExprWithTypes::exitAddExpr(bluefinParser::AddExprContext* ctx) {
	// assume both left and right operands have same type. Arbitrarily return left operand's type
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr(0));
	exprTypes.put(ctx, subExprType);
}

void DecorateExprWithTypes::exitRelExpr(bluefinParser::RelExprContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL)));
}

void DecorateExprWithTypes::exitEqualityExpr(bluefinParser::EqualityExprContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL)));
}

void DecorateExprWithTypes::exitLogicalANDExpr(bluefinParser::LogicalANDExprContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL)));
}

void DecorateExprWithTypes::exitLogicalORExpr(bluefinParser::LogicalORExprContext* ctx) {
	exprTypes.put(ctx, dynamic_pointer_cast<Type>(
			symbolFactory.createBuiltinTypeSymbol(BuiltinTypeSymbol::Possibilities::BOOL)));
}

void DecorateExprWithTypes::exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext* ctx) {
	// assume both left and right operands have same type. Arbitrarily return left operand's type
	shared_ptr<Type> subExprType = exprTypes.get(ctx->expr(0));
	exprTypes.put(ctx, subExprType);
}

void DecorateExprWithTypes::exitMemberAccess(bluefinParser::MemberAccessContext* ctx) {
	// put type of member. Eg, first.a could have type int
	shared_ptr<StructSymbol> structType = static_pointer_cast<StructSymbol>(exprTypes.get(ctx->expr()));
	shared_ptr<Type> memberType = structType->resolveMember(ctx->ID()->getText())->getType();
	exprTypes.put(ctx, memberType);
}


shared_ptr<Symbol> DecorateExprWithTypes::resolve(const string name, shared_ptr<Scope> startScope) {

	shared_ptr<Scope> scope = startScope;

	do {
		shared_ptr<Symbol> sym = scope->resolve(name);
		if (sym) { return sym; }
	} while (scope = scope->getEnclosingScope());

	throw UnresolvedSymbolException(name);
}
