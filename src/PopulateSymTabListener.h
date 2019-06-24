#pragma once

#include <stack>
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/StructSymbol.h"
#include "../symbolTable/SymbolFactory.h"

namespace bluefin {

	using std::stack;

	class PopulateSymTabListener : public bluefinBaseListener
	{
	public:

		// For testing, we'll pass in an adapter of a symbol table
		PopulateSymTabListener(SymbolTable& symbolTable, SymbolFactory& factory) 
			: symbolTable{ symbolTable }, symbolFactory{ factory }
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
		void exitMemberAccess(bluefinParser::MemberAccessContext*) override;

		void enterBlock(bluefinParser::BlockContext*) override;
		void exitBlock(bluefinParser::BlockContext*) override;

	private:
		SymbolTable& symbolTable;
		SymbolFactory& symbolFactory;


		/* 
		The purpose of this stack is to enable resolution of struct members. Since each 
		listener's return type is void, there's no implicit way for the child of a node 
		to return a value to its parent. We can't use the call stack so we create an 
		explicit stack. An example of the tree for a.b: 
										  stmtExpr
							expr:memberAccess		;
						expr:primaryId	.	b
								a

		1. We want to resolve 'a' as a StructSymbol
		2. Resolve 'b' as a member of 'a'
		We pass the resolution of 'a' to memberAccess so it can resolve 'b' from 'a' with a stack

		If primaryId's category is a StructSymbol, push onto stack
		memberAccess must pop from the stack to get the first component, which, ofc, we expect to be a StructSymbol.
		If a.b is a struct, then we must should it onto the stack
		TODO: To Verify. At the end of a stmExpr, we expect that the stack is empty (# of uses = # of resolved)
		*/
		stack<shared_ptr<StructSymbol>> structSymbolStack;
	};

}
