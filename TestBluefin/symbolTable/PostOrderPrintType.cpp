#include "pch.h"
#include "PostOrderPrintType.h"
#include <iostream>

#include "antlr4-runtime.h"

using namespace SymbolTableTests;

using namespace std;

/*
For type tests, the printed order will be:
varName - t_type - t_promotedType
*/
void PostOrderPrintType::exitEveryRule(antlr4::ParserRuleContext* ctx)  {
	if (exprTypeContexts.find(ctx) != exprTypeContexts.end()) {
		string typeStr = exprTypeContexts.at(ctx).getEvalType().type2str();
		string promoteTypeStr = exprTypeContexts.at(ctx).getPromotionType().type2str();
		output += ctx->getText() + " - t_" + typeStr + " - t_" + promoteTypeStr + "\n";
	}
}
