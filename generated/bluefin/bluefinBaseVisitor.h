
// Generated from C:/Users/Lenny/IdeaProjects/bluefin\bluefin.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "bluefinVisitor.h"


namespace bluefin {

/**
 * This class provides an empty implementation of bluefinVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  bluefinBaseVisitor : public bluefinVisitor {
public:

  virtual antlrcpp::Any visitProgram(bluefinParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncDef(bluefinParser::FuncDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitType(bluefinParser::TypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBuiltinType(bluefinParser::BuiltinTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParamList(bluefinParser::ParamListContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParam(bluefinParser::ParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlock(bluefinParser::BlockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmt(bluefinParser::StmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtIf(bluefinParser::StmtIfContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtWhile(bluefinParser::StmtWhileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtReturn(bluefinParser::StmtReturnContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtBreak(bluefinParser::StmtBreakContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtContinue(bluefinParser::StmtContinueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStmtExpr(bluefinParser::StmtExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStructDef(bluefinParser::StructDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSuperClass(bluefinParser::SuperClassContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVarDecl(bluefinParser::VarDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMemberAccess(bluefinParser::MemberAccessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMultiExpr(bluefinParser::MultiExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryFloat(bluefinParser::PrimaryFloatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLogicalANDExpr(bluefinParser::LogicalANDExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryId(bluefinParser::PrimaryIdContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryBool(bluefinParser::PrimaryBoolContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryInt(bluefinParser::PrimaryIntContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLogicalORExpr(bluefinParser::LogicalORExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryString(bluefinParser::PrimaryStringContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnaryExpr(bluefinParser::UnaryExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrimaryParenth(bluefinParser::PrimaryParenthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAddExpr(bluefinParser::AddExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncCall(bluefinParser::FuncCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRelExpr(bluefinParser::RelExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEqualityExpr(bluefinParser::EqualityExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMethodCall(bluefinParser::MethodCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArgList(bluefinParser::ArgListContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace bluefin
