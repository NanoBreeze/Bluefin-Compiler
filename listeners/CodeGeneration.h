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
#include "../symbolTable/FunctionSymbol.h"
#include "../symbolTable/Scope.h"
#include "../symbolTable/TypeContext.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace bluefin {

	using std::pair;
	using std::vector;
	using std::map;
	class IfStmtHelper {

	public:
		// NOTE: This object model needs some refactoring
		void addIfElseStmt(pair<bluefinParser::BlockContext*, llvm::BasicBlock*> thenPair,
			pair<bluefinParser::BlockContext*, llvm::BasicBlock*> elsePair,
			llvm::BasicBlock* mergeBlock, bluefinParser::ExprContext* exprNode, bluefinParser::StmtIfContext* ifStmtNode);
		void addIfStmt(pair<bluefinParser::BlockContext*, llvm::BasicBlock*> thenPair,
			llvm::BasicBlock* mergeBlock, bluefinParser::ExprContext* exprNode, bluefinParser::StmtIfContext* ifStmtNode);

		bool isThenBlockNode(bluefinParser::BlockContext*) const;
		bool isElseBlockNode(bluefinParser::BlockContext*) const;
		bluefinParser::ExprContext* getComparisonExpr(bluefinParser::BlockContext*) const;
		
		bool hasCorrespondingElseNode(bluefinParser::BlockContext*) const;
		llvm::BasicBlock* getBBForElse(bluefinParser::BlockContext* elseCtx) const;
		llvm::BasicBlock* getBBForThen(bluefinParser::BlockContext* ifCtx) const;
		llvm::BasicBlock* getBBForMerge(bluefinParser::BlockContext* ifOrElseCtx) const;
		llvm::BasicBlock* getBBForMerge(bluefinParser::StmtIfContext*) const;

	private:
		struct IfStmtInfo {
			bluefinParser::BlockContext* thenBlockNode;
			bluefinParser::BlockContext* elseBlockNode;
			llvm::BasicBlock* llvmThenBlockLabel;
			llvm::BasicBlock* llvmElseBlockLabel;
			llvm::BasicBlock* llvmMergeBlockLabel;
			bluefinParser::ExprContext* exprNode;
			bluefinParser::StmtIfContext* ifNode;
		};

		vector<IfStmtInfo> infos; // maybe a set is better
	};

	class WhileStmtHelper {
	public:
		void addWhileStmt(llvm::BasicBlock* llvmWhileLoopCmpLabel, llvm::BasicBlock* llvmWhileLoopBodyLabel, llvm::BasicBlock* llvmAfterWhileLoopLabel,
			bluefinParser::ExprContext* exprNode, bluefinParser::BlockContext* whileBlockNode, bluefinParser::StmtWhileContext* whileStmtNode);
	
		bluefinParser::ExprContext* getComparisonExpr(bluefinParser::BlockContext*) const;
		llvm::BasicBlock* getBBForLoopCmp(bluefinParser::BlockContext*) const;
		llvm::BasicBlock* getBBForLoopBody(bluefinParser::BlockContext*) const;
		llvm::BasicBlock* getBBForAfterLoop(bluefinParser::BlockContext*) const;
		llvm::BasicBlock* getBBForLoopCmp(bluefinParser::StmtWhileContext*) const;
		llvm::BasicBlock* getBBForAfterLoop(bluefinParser::StmtWhileContext*) const;
		bool isWhileBlockNode(bluefinParser::BlockContext*) const;

	private:
		struct WhileStmtInfo {
			llvm::BasicBlock* llvmWhileLoopCmpLabel;
			llvm::BasicBlock* llvmWhileLoopBodyLabel;
			llvm::BasicBlock* llvmAfterWhileLoopLabel;
			bluefinParser::ExprContext* exprNode;
			bluefinParser::BlockContext* whileBlockNode;
			bluefinParser::StmtWhileContext* whileStmtNode;
		};

		vector<WhileStmtInfo> infos;
	};

	class StructHelper {
	public:
		// To get the StructType, the user can simply call Codegen::getLLVMType(..)
		void addStructDef(shared_ptr<StructSymbol>, llvm::Function* ctor);
		llvm::Function* getCtor(shared_ptr<StructSymbol>);
		llvm::Value* getCtorThisPtr(shared_ptr<StructSymbol>);
		

	private:
		map<shared_ptr<StructSymbol>, llvm::Function*> structSymbolAndFuncs;
	};
}

namespace bluefin {

	using std::shared_ptr;
	using std::map;
	using std::stack;
	using std::pair;
	using std::vector;
	using std::unique_ptr;
	using antlr4::tree::ParseTree;

	class llvm::Value;
	class llvm::Function;
	class llvm::Type;
	class llvm::StructType;
	class llvm::GetElementPtrInst;
	class llvm::GlobalVariable;

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

		void enterVarDecl(bluefinParser::VarDeclContext*) override;
		void exitVarDecl(bluefinParser::VarDeclContext*) override;
		void enterFuncDef(bluefinParser::FuncDefContext*) override;
		void exitFuncDef(bluefinParser::FuncDefContext*) override;
		void enterStructDef(bluefinParser::StructDefContext*) override;
		void exitStructDef(bluefinParser::StructDefContext* ctx) override;
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
		void exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext*) override;

		void exitFuncCall(bluefinParser::FuncCallContext*) override;
		void exitStmtReturn(bluefinParser::StmtReturnContext*) override;
		void exitMemberAccess(bluefinParser::MemberAccessContext*) override;
		void exitMethodCall(bluefinParser::MethodCallContext*) override;

		void enterStmtIf(bluefinParser::StmtIfContext*) override;
		void exitStmtIf(bluefinParser::StmtIfContext*) override;
		void enterStmtWhile(bluefinParser::StmtWhileContext*) override;
		void exitStmtWhile(bluefinParser::StmtWhileContext*) override;
		void enterBlock(bluefinParser::BlockContext*) override;
		void exitBlock(bluefinParser::BlockContext*) override;
		
		void exitProgram(bluefinParser::ProgramContext*) override;

		string dump();
		bool isCodeGenOK();

	private:
		SymbolTable& symbolTable;
		const map<ParseTree*, TypeContext>& typeContexts; // stores the type associated with expressions and functions. Enables type checking and casting

		map<ParseTree*, llvm::Value*> values; // stores the type associated ctx node with the LLVM value
		map<shared_ptr<Symbol>, llvm::Value*> resolvedSymAndAddrs; // Matches a resolved Symbol with its address. The resolved symbol is used
		// either for local variable (AllocaInst) or global variable (GlobalVariable). Struct field addresses are placed in elementPtrs
		
		// Given two member accesses (a.fooStruct.c, and x.fooStruct.y), the 'fooStruct' member resolves to the instance field of the StructDef that 
		// corresponds to 'a' type (yeah this is wordy). That's a problem, since if we associate pointer addresses to them, we can get incorrect results
		// As a result, don't associate field pointers to symbols. Instead, associate to the memberAccessContext
		map<bluefinParser::ExprContext*, llvm::GetElementPtrInst*> elementPtrs; 
		
		unordered_map<Type, llvm::Type*> bluefinToLLVMTypes;
		llvm::Value* currentMethodThis;
		map<shared_ptr<FunctionSymbol>, llvm::Function*> methodToLLVMFunctions;

		unique_ptr<llvm::LLVMContext> TheContext;
		unique_ptr<llvm::Module> TheModule;
		unique_ptr<llvm::IRBuilder<>> Builder;

		IfStmtHelper ifStmtHelper;
		WhileStmtHelper whileStmtHelper;
		StructHelper structHelper;

		vector<llvm::Function*> internalFunctionsForVarDeclExpr;
		map<llvm::GlobalVariable*, llvm::Function*> functionsThatInitializeGlobalVarDecl; // global varDecl may need to be initialized. This stores the function that initializes them

		llvm::Type* getLLVMType(Type) const; // return the LLVMType that corresponds with our BluefinType

		llvm::Value* createCastIfNecessary(bluefinParser::ExprContext*, llvm::Value*) const;
		llvm::Function* createFunction(shared_ptr<FunctionSymbol>, llvm::StructType*) const;
		llvm::Function* createFunction(shared_ptr<FunctionSymbol>) const;
		llvm::Function* createCtor(llvm::StructType*) const;
		llvm::Function* createGlobalVarDeclFunction(string id) const;
		llvm::StructType* createStruct(shared_ptr<StructSymbol>) const;

		llvm::GetElementPtrInst* createStructElementAddr(shared_ptr<StructSymbol>, string fieldName, llvm::Value* structAddr);
		// CallCtor?
	};
}
