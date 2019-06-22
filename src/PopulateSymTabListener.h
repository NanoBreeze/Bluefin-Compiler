#pragma once

#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"

namespace bluefin {

	class PopulateSymTabListener : public bluefinBaseListener
	{
	public:

		// For testing, we'll pass in an adapter of a symbol table
		PopulateSymTabListener(SymbolTable& symbolTable) : symbolTable{ symbolTable }
		{}

		void enterVarDecl(bluefinParser::VarDeclContext*) override;

		void enterFuncDef(bluefinParser::FuncDefContext*) override;

		void enterStructDef(bluefinParser::StructDefContext*) override;
		void exitStructDef(bluefinParser::StructDefContext*) override;

		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;

		void enterBlock(bluefinParser::BlockContext*) override;
		void exitBlock(bluefinParser::BlockContext*) override;

	private:
		SymbolTable& symbolTable;
	};

}
