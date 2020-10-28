#pragma once

#include "antlr4-runtime.h"
#include <memory>
#include <map>
#include <stack>
#include <vector>
#include "../symbolTable/EventObserver.h"
#include "../generated/bluefin/bluefinBaseListener.h"
#include "../symbolTable/SymbolTable.h"
#include "../symbolTable/StructSymbol.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/TypeContext.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"


namespace bluefin {

	using std::shared_ptr;
	using std::map;
	using std::stack;
	using std::pair;
	using std::vector;
	using std::unique_ptr;
	using antlr4::tree::ParseTree;

	class llvm::Value;

	/*
	Resolves symbol references. To do so we need the appropriate scope associated with the contexts. 
	This should be passed from the Declaration phase. Resolving symbols after all have been declared 
	allows forward reference. To restrict forward references (only to within classes), we compare the 
	position of the context to resolve with the position of the context of the actual resolved symbol 
	(assumes it is resolved). Illegal is current context occurs before the other one.
	*/
	class CodeGeneration : public bluefinBaseListener
	{
	public:
		CodeGeneration(SymbolTable& symTab, const map<ParseTree*, TypeContext>& typeCxts, const string moduleName);

		void enterFuncDef(bluefinParser::FuncDefContext*) override;
		void enterPrimaryBool(bluefinParser::PrimaryBoolContext*) override;
		void enterPrimaryInt(bluefinParser::PrimaryIntContext*) override;
		void enterPrimaryFloat(bluefinParser::PrimaryFloatContext*) override;
		void enterPrimaryId(bluefinParser::PrimaryIdContext*) override;
		void exitPrimaryParenth(bluefinParser::PrimaryParenthContext*) override;

		void exitUnaryExpr(bluefinParser::UnaryExprContext*) override;
		void exitMultiExpr(bluefinParser::MultiExprContext*) override;
		void exitAddExpr(bluefinParser::AddExprContext*) override;
		void exitRelExpr(bluefinParser::RelExprContext*) override;
		void exitEqualityExpr(bluefinParser::EqualityExprContext*) override;
		void exitLogicalANDExpr(bluefinParser::LogicalANDExprContext*) override;
		void exitLogicalORExpr(bluefinParser::LogicalORExprContext*) override;

		string dump();

	private:
		SymbolTable& symbolTable;
		const map<ParseTree*, TypeContext>& typeContexts; // stores the type associated with expressions and functions. Enables type checking


		map<ParseTree*, llvm::Value*> values; // stores the type associated ctx node with the LLVM value
		map<shared_ptr<Symbol>, llvm::Value*> resolvedSymAndValues; // this is clumsy. It is used to resolve the Value associated with a primaryId. eg) a+6;

		unique_ptr<llvm::LLVMContext> TheContext;
		unique_ptr<llvm::Module> TheModule;
		unique_ptr<llvm::IRBuilder<>> Builder;
	};
}
