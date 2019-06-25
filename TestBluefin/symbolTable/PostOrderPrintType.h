#pragma once

#include "antlr4-runtime.h"
#include "tree/ParseTreeProperty.h"
#include "../../generated/bluefin/bluefinBaseListener.h"
#include "../../symbolTable/Type.h"
#include <memory>
#include <string>

//#include "../src/PopulateSymTabListener.h"

namespace SymbolTableTests {

	using namespace bluefin;
	using antlr4::tree::ParseTreeProperty;
	using std::shared_ptr;
	using std::string;

	class PostOrderPrintType : public bluefinBaseListener
	{
	public:

		PostOrderPrintType(string& output,  ParseTreeProperty<shared_ptr<Type>>& exprTypes)
			: output{ output }, exprTypes { exprTypes }
		{ }

		void exitEveryRule(antlr4::ParserRuleContext*) override;

	private:
		string& output;
		ParseTreeProperty<shared_ptr<Type>> exprTypes;	// stores the type associated for expressions. Enables type checking
	};
}
