#pragma once

#include "antlr4-runtime.h"
#include "tree/ParseTreeProperty.h"
#include "../../generated/bluefin/bluefinBaseListener.h"
#include "../../symbolTable/Type.h"
#include "../../symbolTable/TypeContext.h"
#include <memory>
#include <string>
#include <map>

//#include "../src/PopulateSymTabListener.h"

namespace SymbolTableTests {

	using namespace bluefin;
	using std::shared_ptr;
	using std::string;
	using std::map;
	using antlr4::tree::ParseTree;

	class PostOrderPrintType : public bluefinBaseListener
	{
	public:

		PostOrderPrintType(string& output,  map<ParseTree*, TypeContext>& exprTypeContexts)
			: output{ output }, exprTypeContexts { exprTypeContexts }
		{ }

		void exitEveryRule(antlr4::ParserRuleContext*) override;

	private:
		string& output;
		map<ParseTree*, TypeContext> exprTypeContexts;	// stores the type associated for expressions. Enables type checking
	};
}
