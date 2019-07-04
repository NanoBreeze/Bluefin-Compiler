
// Generated from C:/Users/Lenny/IdeaProjects/bluefin\bluefin.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "bluefinListener.h"


namespace bluefin {

/**
 * This class provides an empty implementation of bluefinListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  bluefinBaseListener : public bluefinListener {
public:

  virtual void enterProgram(bluefinParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(bluefinParser::ProgramContext * /*ctx*/) override { }

  virtual void enterFuncDef(bluefinParser::FuncDefContext * /*ctx*/) override { }
  virtual void exitFuncDef(bluefinParser::FuncDefContext * /*ctx*/) override { }

  virtual void enterType(bluefinParser::TypeContext * /*ctx*/) override { }
  virtual void exitType(bluefinParser::TypeContext * /*ctx*/) override { }

  virtual void enterBuiltinType(bluefinParser::BuiltinTypeContext * /*ctx*/) override { }
  virtual void exitBuiltinType(bluefinParser::BuiltinTypeContext * /*ctx*/) override { }

  virtual void enterParamList(bluefinParser::ParamListContext * /*ctx*/) override { }
  virtual void exitParamList(bluefinParser::ParamListContext * /*ctx*/) override { }

  virtual void enterParam(bluefinParser::ParamContext * /*ctx*/) override { }
  virtual void exitParam(bluefinParser::ParamContext * /*ctx*/) override { }

  virtual void enterBlock(bluefinParser::BlockContext * /*ctx*/) override { }
  virtual void exitBlock(bluefinParser::BlockContext * /*ctx*/) override { }

  virtual void enterStmt(bluefinParser::StmtContext * /*ctx*/) override { }
  virtual void exitStmt(bluefinParser::StmtContext * /*ctx*/) override { }

  virtual void enterStmtIf(bluefinParser::StmtIfContext * /*ctx*/) override { }
  virtual void exitStmtIf(bluefinParser::StmtIfContext * /*ctx*/) override { }

  virtual void enterStmtWhile(bluefinParser::StmtWhileContext * /*ctx*/) override { }
  virtual void exitStmtWhile(bluefinParser::StmtWhileContext * /*ctx*/) override { }

  virtual void enterStmtReturn(bluefinParser::StmtReturnContext * /*ctx*/) override { }
  virtual void exitStmtReturn(bluefinParser::StmtReturnContext * /*ctx*/) override { }

  virtual void enterStmtBreak(bluefinParser::StmtBreakContext * /*ctx*/) override { }
  virtual void exitStmtBreak(bluefinParser::StmtBreakContext * /*ctx*/) override { }

  virtual void enterStmtContinue(bluefinParser::StmtContinueContext * /*ctx*/) override { }
  virtual void exitStmtContinue(bluefinParser::StmtContinueContext * /*ctx*/) override { }

  virtual void enterStmtExpr(bluefinParser::StmtExprContext * /*ctx*/) override { }
  virtual void exitStmtExpr(bluefinParser::StmtExprContext * /*ctx*/) override { }

  virtual void enterStructDef(bluefinParser::StructDefContext * /*ctx*/) override { }
  virtual void exitStructDef(bluefinParser::StructDefContext * /*ctx*/) override { }

  virtual void enterSuperClass(bluefinParser::SuperClassContext * /*ctx*/) override { }
  virtual void exitSuperClass(bluefinParser::SuperClassContext * /*ctx*/) override { }

  virtual void enterVarDecl(bluefinParser::VarDeclContext * /*ctx*/) override { }
  virtual void exitVarDecl(bluefinParser::VarDeclContext * /*ctx*/) override { }

  virtual void enterMemberAccess(bluefinParser::MemberAccessContext * /*ctx*/) override { }
  virtual void exitMemberAccess(bluefinParser::MemberAccessContext * /*ctx*/) override { }

  virtual void enterMultiExpr(bluefinParser::MultiExprContext * /*ctx*/) override { }
  virtual void exitMultiExpr(bluefinParser::MultiExprContext * /*ctx*/) override { }

  virtual void enterPrimaryFloat(bluefinParser::PrimaryFloatContext * /*ctx*/) override { }
  virtual void exitPrimaryFloat(bluefinParser::PrimaryFloatContext * /*ctx*/) override { }

  virtual void enterSimpleAssignExpr(bluefinParser::SimpleAssignExprContext * /*ctx*/) override { }
  virtual void exitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext * /*ctx*/) override { }

  virtual void enterLogicalANDExpr(bluefinParser::LogicalANDExprContext * /*ctx*/) override { }
  virtual void exitLogicalANDExpr(bluefinParser::LogicalANDExprContext * /*ctx*/) override { }

  virtual void enterPrimaryId(bluefinParser::PrimaryIdContext * /*ctx*/) override { }
  virtual void exitPrimaryId(bluefinParser::PrimaryIdContext * /*ctx*/) override { }

  virtual void enterPrimaryBool(bluefinParser::PrimaryBoolContext * /*ctx*/) override { }
  virtual void exitPrimaryBool(bluefinParser::PrimaryBoolContext * /*ctx*/) override { }

  virtual void enterPrimaryInt(bluefinParser::PrimaryIntContext * /*ctx*/) override { }
  virtual void exitPrimaryInt(bluefinParser::PrimaryIntContext * /*ctx*/) override { }

  virtual void enterLogicalORExpr(bluefinParser::LogicalORExprContext * /*ctx*/) override { }
  virtual void exitLogicalORExpr(bluefinParser::LogicalORExprContext * /*ctx*/) override { }

  virtual void enterPrimaryString(bluefinParser::PrimaryStringContext * /*ctx*/) override { }
  virtual void exitPrimaryString(bluefinParser::PrimaryStringContext * /*ctx*/) override { }

  virtual void enterUnaryExpr(bluefinParser::UnaryExprContext * /*ctx*/) override { }
  virtual void exitUnaryExpr(bluefinParser::UnaryExprContext * /*ctx*/) override { }

  virtual void enterPrimaryParenth(bluefinParser::PrimaryParenthContext * /*ctx*/) override { }
  virtual void exitPrimaryParenth(bluefinParser::PrimaryParenthContext * /*ctx*/) override { }

  virtual void enterAddExpr(bluefinParser::AddExprContext * /*ctx*/) override { }
  virtual void exitAddExpr(bluefinParser::AddExprContext * /*ctx*/) override { }

  virtual void enterFuncCall(bluefinParser::FuncCallContext * /*ctx*/) override { }
  virtual void exitFuncCall(bluefinParser::FuncCallContext * /*ctx*/) override { }

  virtual void enterRelExpr(bluefinParser::RelExprContext * /*ctx*/) override { }
  virtual void exitRelExpr(bluefinParser::RelExprContext * /*ctx*/) override { }

  virtual void enterEqualityExpr(bluefinParser::EqualityExprContext * /*ctx*/) override { }
  virtual void exitEqualityExpr(bluefinParser::EqualityExprContext * /*ctx*/) override { }

  virtual void enterMethodCall(bluefinParser::MethodCallContext * /*ctx*/) override { }
  virtual void exitMethodCall(bluefinParser::MethodCallContext * /*ctx*/) override { }

  virtual void enterArgList(bluefinParser::ArgListContext * /*ctx*/) override { }
  virtual void exitArgList(bluefinParser::ArgListContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace bluefin
