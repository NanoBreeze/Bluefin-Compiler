
// Generated from C:/Users/Lenny/IdeaProjects/bluefin\bluefin.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "bluefinParser.h"


namespace bluefin {

/**
 * This interface defines an abstract listener for a parse tree produced by bluefinParser.
 */
class  bluefinListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(bluefinParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(bluefinParser::ProgramContext *ctx) = 0;

  virtual void enterFuncDef(bluefinParser::FuncDefContext *ctx) = 0;
  virtual void exitFuncDef(bluefinParser::FuncDefContext *ctx) = 0;

  virtual void enterType(bluefinParser::TypeContext *ctx) = 0;
  virtual void exitType(bluefinParser::TypeContext *ctx) = 0;

  virtual void enterBuiltinType(bluefinParser::BuiltinTypeContext *ctx) = 0;
  virtual void exitBuiltinType(bluefinParser::BuiltinTypeContext *ctx) = 0;

  virtual void enterParamList(bluefinParser::ParamListContext *ctx) = 0;
  virtual void exitParamList(bluefinParser::ParamListContext *ctx) = 0;

  virtual void enterParam(bluefinParser::ParamContext *ctx) = 0;
  virtual void exitParam(bluefinParser::ParamContext *ctx) = 0;

  virtual void enterBlock(bluefinParser::BlockContext *ctx) = 0;
  virtual void exitBlock(bluefinParser::BlockContext *ctx) = 0;

  virtual void enterStmt(bluefinParser::StmtContext *ctx) = 0;
  virtual void exitStmt(bluefinParser::StmtContext *ctx) = 0;

  virtual void enterStmtIf(bluefinParser::StmtIfContext *ctx) = 0;
  virtual void exitStmtIf(bluefinParser::StmtIfContext *ctx) = 0;

  virtual void enterStmtWhile(bluefinParser::StmtWhileContext *ctx) = 0;
  virtual void exitStmtWhile(bluefinParser::StmtWhileContext *ctx) = 0;

  virtual void enterStmtReturn(bluefinParser::StmtReturnContext *ctx) = 0;
  virtual void exitStmtReturn(bluefinParser::StmtReturnContext *ctx) = 0;

  virtual void enterStmtBreak(bluefinParser::StmtBreakContext *ctx) = 0;
  virtual void exitStmtBreak(bluefinParser::StmtBreakContext *ctx) = 0;

  virtual void enterStmtContinue(bluefinParser::StmtContinueContext *ctx) = 0;
  virtual void exitStmtContinue(bluefinParser::StmtContinueContext *ctx) = 0;

  virtual void enterStmtExpr(bluefinParser::StmtExprContext *ctx) = 0;
  virtual void exitStmtExpr(bluefinParser::StmtExprContext *ctx) = 0;

  virtual void enterStructDef(bluefinParser::StructDefContext *ctx) = 0;
  virtual void exitStructDef(bluefinParser::StructDefContext *ctx) = 0;

  virtual void enterVarDecl(bluefinParser::VarDeclContext *ctx) = 0;
  virtual void exitVarDecl(bluefinParser::VarDeclContext *ctx) = 0;

  virtual void enterUnaryExpr(bluefinParser::UnaryExprContext *ctx) = 0;
  virtual void exitUnaryExpr(bluefinParser::UnaryExprContext *ctx) = 0;

  virtual void enterAddExpr(bluefinParser::AddExprContext *ctx) = 0;
  virtual void exitAddExpr(bluefinParser::AddExprContext *ctx) = 0;

  virtual void enterMultiExpr(bluefinParser::MultiExprContext *ctx) = 0;
  virtual void exitMultiExpr(bluefinParser::MultiExprContext *ctx) = 0;

  virtual void enterSimpleAssignExpr(bluefinParser::SimpleAssignExprContext *ctx) = 0;
  virtual void exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext *ctx) = 0;

  virtual void enterLogicalANDExpr(bluefinParser::LogicalANDExprContext *ctx) = 0;
  virtual void exitLogicalANDExpr(bluefinParser::LogicalANDExprContext *ctx) = 0;

  virtual void enterRelExpr(bluefinParser::RelExprContext *ctx) = 0;
  virtual void exitRelExpr(bluefinParser::RelExprContext *ctx) = 0;

  virtual void enterPrimaExpr(bluefinParser::PrimaExprContext *ctx) = 0;
  virtual void exitPrimaExpr(bluefinParser::PrimaExprContext *ctx) = 0;

  virtual void enterPostExpr(bluefinParser::PostExprContext *ctx) = 0;
  virtual void exitPostExpr(bluefinParser::PostExprContext *ctx) = 0;

  virtual void enterEqualityExpr(bluefinParser::EqualityExprContext *ctx) = 0;
  virtual void exitEqualityExpr(bluefinParser::EqualityExprContext *ctx) = 0;

  virtual void enterLogicalORExpr(bluefinParser::LogicalORExprContext *ctx) = 0;
  virtual void exitLogicalORExpr(bluefinParser::LogicalORExprContext *ctx) = 0;

  virtual void enterPostfixExpr(bluefinParser::PostfixExprContext *ctx) = 0;
  virtual void exitPostfixExpr(bluefinParser::PostfixExprContext *ctx) = 0;

  virtual void enterArgList(bluefinParser::ArgListContext *ctx) = 0;
  virtual void exitArgList(bluefinParser::ArgListContext *ctx) = 0;

  virtual void enterPrimaryExpr(bluefinParser::PrimaryExprContext *ctx) = 0;
  virtual void exitPrimaryExpr(bluefinParser::PrimaryExprContext *ctx) = 0;


};

}  // namespace bluefin
