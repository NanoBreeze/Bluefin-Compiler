
// Generated from C:/Users/Lenny/IdeaProjects/bluefin\bluefin.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"
#include "bluefinParser.h"


namespace bluefin {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by bluefinParser.
 */
class  bluefinVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by bluefinParser.
   */
    virtual antlrcpp::Any visitProgram(bluefinParser::ProgramContext *context) = 0;

    virtual antlrcpp::Any visitFuncDef(bluefinParser::FuncDefContext *context) = 0;

    virtual antlrcpp::Any visitType(bluefinParser::TypeContext *context) = 0;

    virtual antlrcpp::Any visitBuiltinType(bluefinParser::BuiltinTypeContext *context) = 0;

    virtual antlrcpp::Any visitParamList(bluefinParser::ParamListContext *context) = 0;

    virtual antlrcpp::Any visitParam(bluefinParser::ParamContext *context) = 0;

    virtual antlrcpp::Any visitBlock(bluefinParser::BlockContext *context) = 0;

    virtual antlrcpp::Any visitStmt(bluefinParser::StmtContext *context) = 0;

    virtual antlrcpp::Any visitStmtIf(bluefinParser::StmtIfContext *context) = 0;

    virtual antlrcpp::Any visitStmtWhile(bluefinParser::StmtWhileContext *context) = 0;

    virtual antlrcpp::Any visitStmtReturn(bluefinParser::StmtReturnContext *context) = 0;

    virtual antlrcpp::Any visitStmtBreak(bluefinParser::StmtBreakContext *context) = 0;

    virtual antlrcpp::Any visitStmtContinue(bluefinParser::StmtContinueContext *context) = 0;

    virtual antlrcpp::Any visitStmtExpr(bluefinParser::StmtExprContext *context) = 0;

    virtual antlrcpp::Any visitStructDef(bluefinParser::StructDefContext *context) = 0;

    virtual antlrcpp::Any visitSuperClass(bluefinParser::SuperClassContext *context) = 0;

    virtual antlrcpp::Any visitVarDecl(bluefinParser::VarDeclContext *context) = 0;

    virtual antlrcpp::Any visitMemberAccess(bluefinParser::MemberAccessContext *context) = 0;

    virtual antlrcpp::Any visitMultiExpr(bluefinParser::MultiExprContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryFloat(bluefinParser::PrimaryFloatContext *context) = 0;

    virtual antlrcpp::Any visitSimpleAssignExpr(bluefinParser::SimpleAssignExprContext *context) = 0;

    virtual antlrcpp::Any visitLogicalANDExpr(bluefinParser::LogicalANDExprContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryId(bluefinParser::PrimaryIdContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryBool(bluefinParser::PrimaryBoolContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryInt(bluefinParser::PrimaryIntContext *context) = 0;

    virtual antlrcpp::Any visitLogicalORExpr(bluefinParser::LogicalORExprContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryString(bluefinParser::PrimaryStringContext *context) = 0;

    virtual antlrcpp::Any visitUnaryExpr(bluefinParser::UnaryExprContext *context) = 0;

    virtual antlrcpp::Any visitPrimaryParenth(bluefinParser::PrimaryParenthContext *context) = 0;

    virtual antlrcpp::Any visitAddExpr(bluefinParser::AddExprContext *context) = 0;

    virtual antlrcpp::Any visitFuncCall(bluefinParser::FuncCallContext *context) = 0;

    virtual antlrcpp::Any visitRelExpr(bluefinParser::RelExprContext *context) = 0;

    virtual antlrcpp::Any visitEqualityExpr(bluefinParser::EqualityExprContext *context) = 0;

    virtual antlrcpp::Any visitMethodCall(bluefinParser::MethodCallContext *context) = 0;

    virtual antlrcpp::Any visitArgList(bluefinParser::ArgListContext *context) = 0;


};

}  // namespace bluefin
