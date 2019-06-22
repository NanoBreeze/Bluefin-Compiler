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

		/* Design decision for function scopes
		Unlike C, in Bluefin, function params and the function body are NOT different scopes; they're the same scope
		As a result, the following funcDef is illegal b/c of redefinition of variable a:
			int hi (float a) {
				int a;
			}
		This design has several implications:
			- scope begins at the start of the parameter list. '('. But since a paramList is optional
				(aka, rule will not be called), we start the scope at the end of the function name
			- Since we only want a single scope for the entire function definition, which starts at '(',
				we don't want '{' to create a new scope. As a result, we don't want enterBlock(..) to 
				be called, so we change the grammar from:
					funcDef : type ID '(' paramList? ')' block ;
				to: 
					funcDef : type ID '(' paramList? ')' '{' stmt* '}';
			- Since exitBlock(..) also won't be called to finish this scope, exitFuncDef(..) will be called.
		*/

		void enterFuncDef(bluefinParser::FuncDefContext*) override;

		void enterParam(bluefinParser::ParamContext*) override;
		void exitFuncDef(bluefinParser::FuncDefContext*) override;


		void enterStructDef(bluefinParser::StructDefContext*) override;
		void exitStructDef(bluefinParser::StructDefContext*) override;

		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;

		void enterBlock(bluefinParser::BlockContext*) override;
		void exitBlock(bluefinParser::BlockContext*) override;

	private:
		SymbolTable& symbolTable;
	};

}
