#include "pch.h"
#include "PostOrderPrintType.h"
#include <iostream>

#include "antlr4-runtime.h"

using namespace SymbolTableTests;

using namespace std;

void PostOrderPrintType::exitEveryRule(antlr4::ParserRuleContext* ctx)  {
	if (exprTypes.get(ctx)) { // has side effect, will end up populating map with each context
		string typeStr = exprTypes.get(ctx)->type2str();
		output += ctx->getText() + " - t_" + typeStr + "\n";
	}
}
