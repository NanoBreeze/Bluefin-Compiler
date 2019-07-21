
// Generated from C:/Users/Lenny/IdeaProjects/bluefin\bluefin.g4 by ANTLR 4.7.2


#include "bluefinListener.h"
#include "bluefinVisitor.h"

#include "bluefinParser.h"


using namespace antlrcpp;
using namespace bluefin;
using namespace antlr4;

bluefinParser::bluefinParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

bluefinParser::~bluefinParser() {
  delete _interpreter;
}

std::string bluefinParser::getGrammarFileName() const {
  return "bluefin.g4";
}

const std::vector<std::string>& bluefinParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& bluefinParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- ProgramContext ------------------------------------------------------------------

bluefinParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<bluefinParser::FuncDefContext *> bluefinParser::ProgramContext::funcDef() {
  return getRuleContexts<bluefinParser::FuncDefContext>();
}

bluefinParser::FuncDefContext* bluefinParser::ProgramContext::funcDef(size_t i) {
  return getRuleContext<bluefinParser::FuncDefContext>(i);
}

std::vector<bluefinParser::VarDeclContext *> bluefinParser::ProgramContext::varDecl() {
  return getRuleContexts<bluefinParser::VarDeclContext>();
}

bluefinParser::VarDeclContext* bluefinParser::ProgramContext::varDecl(size_t i) {
  return getRuleContext<bluefinParser::VarDeclContext>(i);
}

std::vector<bluefinParser::StructDefContext *> bluefinParser::ProgramContext::structDef() {
  return getRuleContexts<bluefinParser::StructDefContext>();
}

bluefinParser::StructDefContext* bluefinParser::ProgramContext::structDef(size_t i) {
  return getRuleContext<bluefinParser::StructDefContext>(i);
}


size_t bluefinParser::ProgramContext::getRuleIndex() const {
  return bluefinParser::RuleProgram;
}

void bluefinParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void bluefinParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


antlrcpp::Any bluefinParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::ProgramContext* bluefinParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 0, bluefinParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(41); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(41);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
      case 1: {
        setState(38);
        funcDef();
        break;
      }

      case 2: {
        setState(39);
        varDecl();
        break;
      }

      case 3: {
        setState(40);
        structDef();
        break;
      }

      }
      setState(43); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::T__12)
      | (1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid)
      | (1ULL << bluefinParser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

bluefinParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::TypeContext* bluefinParser::FuncDefContext::type() {
  return getRuleContext<bluefinParser::TypeContext>(0);
}

tree::TerminalNode* bluefinParser::FuncDefContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::ParamListContext* bluefinParser::FuncDefContext::paramList() {
  return getRuleContext<bluefinParser::ParamListContext>(0);
}

std::vector<bluefinParser::StmtContext *> bluefinParser::FuncDefContext::stmt() {
  return getRuleContexts<bluefinParser::StmtContext>();
}

bluefinParser::StmtContext* bluefinParser::FuncDefContext::stmt(size_t i) {
  return getRuleContext<bluefinParser::StmtContext>(i);
}


size_t bluefinParser::FuncDefContext::getRuleIndex() const {
  return bluefinParser::RuleFuncDef;
}

void bluefinParser::FuncDefContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncDef(this);
}

void bluefinParser::FuncDefContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncDef(this);
}


antlrcpp::Any bluefinParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::FuncDefContext* bluefinParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 2, bluefinParser::RuleFuncDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(45);
    type();
    setState(46);
    match(bluefinParser::ID);
    setState(47);
    match(bluefinParser::T__0);
    setState(49);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid)
      | (1ULL << bluefinParser::ID))) != 0)) {
      setState(48);
      paramList();
    }
    setState(51);
    match(bluefinParser::T__1);
    setState(52);
    match(bluefinParser::T__2);
    setState(56);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::T__0)
      | (1ULL << bluefinParser::T__2)
      | (1ULL << bluefinParser::T__5)
      | (1ULL << bluefinParser::T__7)
      | (1ULL << bluefinParser::T__8)
      | (1ULL << bluefinParser::T__10)
      | (1ULL << bluefinParser::T__11)
      | (1ULL << bluefinParser::T__16)
      | (1ULL << bluefinParser::T__17)
      | (1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid)
      | (1ULL << bluefinParser::FLOAT)
      | (1ULL << bluefinParser::BOOL)
      | (1ULL << bluefinParser::STRING)
      | (1ULL << bluefinParser::ID)
      | (1ULL << bluefinParser::INT))) != 0)) {
      setState(53);
      stmt();
      setState(58);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(59);
    match(bluefinParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

bluefinParser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::BuiltinTypeContext* bluefinParser::TypeContext::builtinType() {
  return getRuleContext<bluefinParser::BuiltinTypeContext>(0);
}

tree::TerminalNode* bluefinParser::TypeContext::ID() {
  return getToken(bluefinParser::ID, 0);
}


size_t bluefinParser::TypeContext::getRuleIndex() const {
  return bluefinParser::RuleType;
}

void bluefinParser::TypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterType(this);
}

void bluefinParser::TypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitType(this);
}


antlrcpp::Any bluefinParser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::TypeContext* bluefinParser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 4, bluefinParser::RuleType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(63);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case bluefinParser::TInt:
      case bluefinParser::TFloat:
      case bluefinParser::TString:
      case bluefinParser::TBool:
      case bluefinParser::TVoid: {
        enterOuterAlt(_localctx, 1);
        setState(61);
        builtinType();
        break;
      }

      case bluefinParser::ID: {
        enterOuterAlt(_localctx, 2);
        setState(62);
        match(bluefinParser::ID);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BuiltinTypeContext ------------------------------------------------------------------

bluefinParser::BuiltinTypeContext::BuiltinTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* bluefinParser::BuiltinTypeContext::TInt() {
  return getToken(bluefinParser::TInt, 0);
}

tree::TerminalNode* bluefinParser::BuiltinTypeContext::TFloat() {
  return getToken(bluefinParser::TFloat, 0);
}

tree::TerminalNode* bluefinParser::BuiltinTypeContext::TString() {
  return getToken(bluefinParser::TString, 0);
}

tree::TerminalNode* bluefinParser::BuiltinTypeContext::TBool() {
  return getToken(bluefinParser::TBool, 0);
}

tree::TerminalNode* bluefinParser::BuiltinTypeContext::TVoid() {
  return getToken(bluefinParser::TVoid, 0);
}


size_t bluefinParser::BuiltinTypeContext::getRuleIndex() const {
  return bluefinParser::RuleBuiltinType;
}

void bluefinParser::BuiltinTypeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBuiltinType(this);
}

void bluefinParser::BuiltinTypeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBuiltinType(this);
}


antlrcpp::Any bluefinParser::BuiltinTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitBuiltinType(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::BuiltinTypeContext* bluefinParser::builtinType() {
  BuiltinTypeContext *_localctx = _tracker.createInstance<BuiltinTypeContext>(_ctx, getState());
  enterRule(_localctx, 6, bluefinParser::RuleBuiltinType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(65);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamListContext ------------------------------------------------------------------

bluefinParser::ParamListContext::ParamListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<bluefinParser::ParamContext *> bluefinParser::ParamListContext::param() {
  return getRuleContexts<bluefinParser::ParamContext>();
}

bluefinParser::ParamContext* bluefinParser::ParamListContext::param(size_t i) {
  return getRuleContext<bluefinParser::ParamContext>(i);
}


size_t bluefinParser::ParamListContext::getRuleIndex() const {
  return bluefinParser::RuleParamList;
}

void bluefinParser::ParamListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParamList(this);
}

void bluefinParser::ParamListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParamList(this);
}


antlrcpp::Any bluefinParser::ParamListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitParamList(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::ParamListContext* bluefinParser::paramList() {
  ParamListContext *_localctx = _tracker.createInstance<ParamListContext>(_ctx, getState());
  enterRule(_localctx, 8, bluefinParser::RuleParamList);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(67);
    param();
    setState(72);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == bluefinParser::T__4) {
      setState(68);
      match(bluefinParser::T__4);
      setState(69);
      param();
      setState(74);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParamContext ------------------------------------------------------------------

bluefinParser::ParamContext::ParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::TypeContext* bluefinParser::ParamContext::type() {
  return getRuleContext<bluefinParser::TypeContext>(0);
}

tree::TerminalNode* bluefinParser::ParamContext::ID() {
  return getToken(bluefinParser::ID, 0);
}


size_t bluefinParser::ParamContext::getRuleIndex() const {
  return bluefinParser::RuleParam;
}

void bluefinParser::ParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParam(this);
}

void bluefinParser::ParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParam(this);
}


antlrcpp::Any bluefinParser::ParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitParam(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::ParamContext* bluefinParser::param() {
  ParamContext *_localctx = _tracker.createInstance<ParamContext>(_ctx, getState());
  enterRule(_localctx, 10, bluefinParser::RuleParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    type();
    setState(76);
    match(bluefinParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

bluefinParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<bluefinParser::StmtContext *> bluefinParser::BlockContext::stmt() {
  return getRuleContexts<bluefinParser::StmtContext>();
}

bluefinParser::StmtContext* bluefinParser::BlockContext::stmt(size_t i) {
  return getRuleContext<bluefinParser::StmtContext>(i);
}


size_t bluefinParser::BlockContext::getRuleIndex() const {
  return bluefinParser::RuleBlock;
}

void bluefinParser::BlockContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBlock(this);
}

void bluefinParser::BlockContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBlock(this);
}


antlrcpp::Any bluefinParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::BlockContext* bluefinParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 12, bluefinParser::RuleBlock);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(78);
    match(bluefinParser::T__2);
    setState(82);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::T__0)
      | (1ULL << bluefinParser::T__2)
      | (1ULL << bluefinParser::T__5)
      | (1ULL << bluefinParser::T__7)
      | (1ULL << bluefinParser::T__8)
      | (1ULL << bluefinParser::T__10)
      | (1ULL << bluefinParser::T__11)
      | (1ULL << bluefinParser::T__16)
      | (1ULL << bluefinParser::T__17)
      | (1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid)
      | (1ULL << bluefinParser::FLOAT)
      | (1ULL << bluefinParser::BOOL)
      | (1ULL << bluefinParser::STRING)
      | (1ULL << bluefinParser::ID)
      | (1ULL << bluefinParser::INT))) != 0)) {
      setState(79);
      stmt();
      setState(84);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(85);
    match(bluefinParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

bluefinParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::StmtIfContext* bluefinParser::StmtContext::stmtIf() {
  return getRuleContext<bluefinParser::StmtIfContext>(0);
}

bluefinParser::StmtWhileContext* bluefinParser::StmtContext::stmtWhile() {
  return getRuleContext<bluefinParser::StmtWhileContext>(0);
}

bluefinParser::StmtReturnContext* bluefinParser::StmtContext::stmtReturn() {
  return getRuleContext<bluefinParser::StmtReturnContext>(0);
}

bluefinParser::StmtBreakContext* bluefinParser::StmtContext::stmtBreak() {
  return getRuleContext<bluefinParser::StmtBreakContext>(0);
}

bluefinParser::StmtContinueContext* bluefinParser::StmtContext::stmtContinue() {
  return getRuleContext<bluefinParser::StmtContinueContext>(0);
}

bluefinParser::BlockContext* bluefinParser::StmtContext::block() {
  return getRuleContext<bluefinParser::BlockContext>(0);
}

bluefinParser::StmtExprContext* bluefinParser::StmtContext::stmtExpr() {
  return getRuleContext<bluefinParser::StmtExprContext>(0);
}

bluefinParser::VarDeclContext* bluefinParser::StmtContext::varDecl() {
  return getRuleContext<bluefinParser::VarDeclContext>(0);
}


size_t bluefinParser::StmtContext::getRuleIndex() const {
  return bluefinParser::RuleStmt;
}

void bluefinParser::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void bluefinParser::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}


antlrcpp::Any bluefinParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtContext* bluefinParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 14, bluefinParser::RuleStmt);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(95);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(87);
      stmtIf();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(88);
      stmtWhile();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(89);
      stmtReturn();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(90);
      stmtBreak();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(91);
      stmtContinue();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(92);
      block();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(93);
      stmtExpr();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(94);
      varDecl();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtIfContext ------------------------------------------------------------------

bluefinParser::StmtIfContext::StmtIfContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::ExprContext* bluefinParser::StmtIfContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

std::vector<bluefinParser::BlockContext *> bluefinParser::StmtIfContext::block() {
  return getRuleContexts<bluefinParser::BlockContext>();
}

bluefinParser::BlockContext* bluefinParser::StmtIfContext::block(size_t i) {
  return getRuleContext<bluefinParser::BlockContext>(i);
}


size_t bluefinParser::StmtIfContext::getRuleIndex() const {
  return bluefinParser::RuleStmtIf;
}

void bluefinParser::StmtIfContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtIf(this);
}

void bluefinParser::StmtIfContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtIf(this);
}


antlrcpp::Any bluefinParser::StmtIfContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtIf(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtIfContext* bluefinParser::stmtIf() {
  StmtIfContext *_localctx = _tracker.createInstance<StmtIfContext>(_ctx, getState());
  enterRule(_localctx, 16, bluefinParser::RuleStmtIf);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(97);
    match(bluefinParser::T__5);
    setState(98);
    match(bluefinParser::T__0);
    setState(99);
    expr(0);
    setState(100);
    match(bluefinParser::T__1);
    setState(101);
    block();
    setState(104);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == bluefinParser::T__6) {
      setState(102);
      match(bluefinParser::T__6);
      setState(103);
      block();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtWhileContext ------------------------------------------------------------------

bluefinParser::StmtWhileContext::StmtWhileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::ExprContext* bluefinParser::StmtWhileContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

bluefinParser::BlockContext* bluefinParser::StmtWhileContext::block() {
  return getRuleContext<bluefinParser::BlockContext>(0);
}


size_t bluefinParser::StmtWhileContext::getRuleIndex() const {
  return bluefinParser::RuleStmtWhile;
}

void bluefinParser::StmtWhileContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtWhile(this);
}

void bluefinParser::StmtWhileContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtWhile(this);
}


antlrcpp::Any bluefinParser::StmtWhileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtWhile(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtWhileContext* bluefinParser::stmtWhile() {
  StmtWhileContext *_localctx = _tracker.createInstance<StmtWhileContext>(_ctx, getState());
  enterRule(_localctx, 18, bluefinParser::RuleStmtWhile);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(106);
    match(bluefinParser::T__7);
    setState(107);
    match(bluefinParser::T__0);
    setState(108);
    expr(0);
    setState(109);
    match(bluefinParser::T__1);
    setState(110);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtReturnContext ------------------------------------------------------------------

bluefinParser::StmtReturnContext::StmtReturnContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::ExprContext* bluefinParser::StmtReturnContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}


size_t bluefinParser::StmtReturnContext::getRuleIndex() const {
  return bluefinParser::RuleStmtReturn;
}

void bluefinParser::StmtReturnContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtReturn(this);
}

void bluefinParser::StmtReturnContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtReturn(this);
}


antlrcpp::Any bluefinParser::StmtReturnContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtReturn(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtReturnContext* bluefinParser::stmtReturn() {
  StmtReturnContext *_localctx = _tracker.createInstance<StmtReturnContext>(_ctx, getState());
  enterRule(_localctx, 20, bluefinParser::RuleStmtReturn);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(112);
    match(bluefinParser::T__8);
    setState(114);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::T__0)
      | (1ULL << bluefinParser::T__16)
      | (1ULL << bluefinParser::T__17)
      | (1ULL << bluefinParser::FLOAT)
      | (1ULL << bluefinParser::BOOL)
      | (1ULL << bluefinParser::STRING)
      | (1ULL << bluefinParser::ID)
      | (1ULL << bluefinParser::INT))) != 0)) {
      setState(113);
      expr(0);
    }
    setState(116);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtBreakContext ------------------------------------------------------------------

bluefinParser::StmtBreakContext::StmtBreakContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t bluefinParser::StmtBreakContext::getRuleIndex() const {
  return bluefinParser::RuleStmtBreak;
}

void bluefinParser::StmtBreakContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtBreak(this);
}

void bluefinParser::StmtBreakContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtBreak(this);
}


antlrcpp::Any bluefinParser::StmtBreakContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtBreak(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtBreakContext* bluefinParser::stmtBreak() {
  StmtBreakContext *_localctx = _tracker.createInstance<StmtBreakContext>(_ctx, getState());
  enterRule(_localctx, 22, bluefinParser::RuleStmtBreak);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(118);
    match(bluefinParser::T__10);
    setState(119);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContinueContext ------------------------------------------------------------------

bluefinParser::StmtContinueContext::StmtContinueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t bluefinParser::StmtContinueContext::getRuleIndex() const {
  return bluefinParser::RuleStmtContinue;
}

void bluefinParser::StmtContinueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtContinue(this);
}

void bluefinParser::StmtContinueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtContinue(this);
}


antlrcpp::Any bluefinParser::StmtContinueContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtContinue(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtContinueContext* bluefinParser::stmtContinue() {
  StmtContinueContext *_localctx = _tracker.createInstance<StmtContinueContext>(_ctx, getState());
  enterRule(_localctx, 24, bluefinParser::RuleStmtContinue);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    match(bluefinParser::T__11);
    setState(122);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtExprContext ------------------------------------------------------------------

bluefinParser::StmtExprContext::StmtExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::ExprContext* bluefinParser::StmtExprContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}


size_t bluefinParser::StmtExprContext::getRuleIndex() const {
  return bluefinParser::RuleStmtExpr;
}

void bluefinParser::StmtExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmtExpr(this);
}

void bluefinParser::StmtExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmtExpr(this);
}


antlrcpp::Any bluefinParser::StmtExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStmtExpr(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StmtExprContext* bluefinParser::stmtExpr() {
  StmtExprContext *_localctx = _tracker.createInstance<StmtExprContext>(_ctx, getState());
  enterRule(_localctx, 26, bluefinParser::RuleStmtExpr);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(124);
    expr(0);
    setState(125);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StructDefContext ------------------------------------------------------------------

bluefinParser::StructDefContext::StructDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* bluefinParser::StructDefContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::SuperClassContext* bluefinParser::StructDefContext::superClass() {
  return getRuleContext<bluefinParser::SuperClassContext>(0);
}

std::vector<bluefinParser::VarDeclContext *> bluefinParser::StructDefContext::varDecl() {
  return getRuleContexts<bluefinParser::VarDeclContext>();
}

bluefinParser::VarDeclContext* bluefinParser::StructDefContext::varDecl(size_t i) {
  return getRuleContext<bluefinParser::VarDeclContext>(i);
}

std::vector<bluefinParser::FuncDefContext *> bluefinParser::StructDefContext::funcDef() {
  return getRuleContexts<bluefinParser::FuncDefContext>();
}

bluefinParser::FuncDefContext* bluefinParser::StructDefContext::funcDef(size_t i) {
  return getRuleContext<bluefinParser::FuncDefContext>(i);
}


size_t bluefinParser::StructDefContext::getRuleIndex() const {
  return bluefinParser::RuleStructDef;
}

void bluefinParser::StructDefContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStructDef(this);
}

void bluefinParser::StructDefContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStructDef(this);
}


antlrcpp::Any bluefinParser::StructDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitStructDef(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::StructDefContext* bluefinParser::structDef() {
  StructDefContext *_localctx = _tracker.createInstance<StructDefContext>(_ctx, getState());
  enterRule(_localctx, 28, bluefinParser::RuleStructDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(127);
    match(bluefinParser::T__12);
    setState(128);
    match(bluefinParser::ID);
    setState(130);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == bluefinParser::T__13) {
      setState(129);
      superClass();
    }
    setState(132);
    match(bluefinParser::T__2);
    setState(137);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << bluefinParser::TInt)
      | (1ULL << bluefinParser::TFloat)
      | (1ULL << bluefinParser::TString)
      | (1ULL << bluefinParser::TBool)
      | (1ULL << bluefinParser::TVoid)
      | (1ULL << bluefinParser::ID))) != 0)) {
      setState(135);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
      case 1: {
        setState(133);
        varDecl();
        break;
      }

      case 2: {
        setState(134);
        funcDef();
        break;
      }

      }
      setState(139);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(140);
    match(bluefinParser::T__3);
    setState(141);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SuperClassContext ------------------------------------------------------------------

bluefinParser::SuperClassContext::SuperClassContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* bluefinParser::SuperClassContext::ID() {
  return getToken(bluefinParser::ID, 0);
}


size_t bluefinParser::SuperClassContext::getRuleIndex() const {
  return bluefinParser::RuleSuperClass;
}

void bluefinParser::SuperClassContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSuperClass(this);
}

void bluefinParser::SuperClassContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSuperClass(this);
}


antlrcpp::Any bluefinParser::SuperClassContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitSuperClass(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::SuperClassContext* bluefinParser::superClass() {
  SuperClassContext *_localctx = _tracker.createInstance<SuperClassContext>(_ctx, getState());
  enterRule(_localctx, 30, bluefinParser::RuleSuperClass);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(143);
    match(bluefinParser::T__13);
    setState(144);
    match(bluefinParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

bluefinParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

bluefinParser::TypeContext* bluefinParser::VarDeclContext::type() {
  return getRuleContext<bluefinParser::TypeContext>(0);
}

tree::TerminalNode* bluefinParser::VarDeclContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::ExprContext* bluefinParser::VarDeclContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}


size_t bluefinParser::VarDeclContext::getRuleIndex() const {
  return bluefinParser::RuleVarDecl;
}

void bluefinParser::VarDeclContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterVarDecl(this);
}

void bluefinParser::VarDeclContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitVarDecl(this);
}


antlrcpp::Any bluefinParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::VarDeclContext* bluefinParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 32, bluefinParser::RuleVarDecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(146);
    type();
    setState(147);
    match(bluefinParser::ID);
    setState(150);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == bluefinParser::T__14) {
      setState(148);
      match(bluefinParser::T__14);
      setState(149);
      expr(0);
    }
    setState(152);
    match(bluefinParser::T__9);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

bluefinParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t bluefinParser::ExprContext::getRuleIndex() const {
  return bluefinParser::RuleExpr;
}

void bluefinParser::ExprContext::copyFrom(ExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- MemberAccessContext ------------------------------------------------------------------

bluefinParser::ExprContext* bluefinParser::MemberAccessContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

tree::TerminalNode* bluefinParser::MemberAccessContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::MemberAccessContext::MemberAccessContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::MemberAccessContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMemberAccess(this);
}
void bluefinParser::MemberAccessContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMemberAccess(this);
}

antlrcpp::Any bluefinParser::MemberAccessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitMemberAccess(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MultiExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::MultiExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::MultiExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::MultiExprContext::MultiExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::MultiExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMultiExpr(this);
}
void bluefinParser::MultiExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMultiExpr(this);
}

antlrcpp::Any bluefinParser::MultiExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitMultiExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryFloatContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::PrimaryFloatContext::FLOAT() {
  return getToken(bluefinParser::FLOAT, 0);
}

bluefinParser::PrimaryFloatContext::PrimaryFloatContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryFloatContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryFloat(this);
}
void bluefinParser::PrimaryFloatContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryFloat(this);
}

antlrcpp::Any bluefinParser::PrimaryFloatContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryFloat(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SimpleAssignExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::SimpleAssignExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::SimpleAssignExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::SimpleAssignExprContext::SimpleAssignExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::SimpleAssignExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSimpleAssignExpr(this);
}
void bluefinParser::SimpleAssignExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSimpleAssignExpr(this);
}

antlrcpp::Any bluefinParser::SimpleAssignExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitSimpleAssignExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LogicalANDExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::LogicalANDExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::LogicalANDExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::LogicalANDExprContext::LogicalANDExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::LogicalANDExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalANDExpr(this);
}
void bluefinParser::LogicalANDExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalANDExpr(this);
}

antlrcpp::Any bluefinParser::LogicalANDExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitLogicalANDExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryIdContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::PrimaryIdContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::PrimaryIdContext::PrimaryIdContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryIdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryId(this);
}
void bluefinParser::PrimaryIdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryId(this);
}

antlrcpp::Any bluefinParser::PrimaryIdContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryId(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryBoolContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::PrimaryBoolContext::BOOL() {
  return getToken(bluefinParser::BOOL, 0);
}

bluefinParser::PrimaryBoolContext::PrimaryBoolContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryBoolContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryBool(this);
}
void bluefinParser::PrimaryBoolContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryBool(this);
}

antlrcpp::Any bluefinParser::PrimaryBoolContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryBool(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryIntContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::PrimaryIntContext::INT() {
  return getToken(bluefinParser::INT, 0);
}

bluefinParser::PrimaryIntContext::PrimaryIntContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryIntContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryInt(this);
}
void bluefinParser::PrimaryIntContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryInt(this);
}

antlrcpp::Any bluefinParser::PrimaryIntContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryInt(this);
  else
    return visitor->visitChildren(this);
}
//----------------- LogicalORExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::LogicalORExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::LogicalORExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::LogicalORExprContext::LogicalORExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::LogicalORExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLogicalORExpr(this);
}
void bluefinParser::LogicalORExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLogicalORExpr(this);
}

antlrcpp::Any bluefinParser::LogicalORExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitLogicalORExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryStringContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::PrimaryStringContext::STRING() {
  return getToken(bluefinParser::STRING, 0);
}

bluefinParser::PrimaryStringContext::PrimaryStringContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryStringContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryString(this);
}
void bluefinParser::PrimaryStringContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryString(this);
}

antlrcpp::Any bluefinParser::PrimaryStringContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryString(this);
  else
    return visitor->visitChildren(this);
}
//----------------- UnaryExprContext ------------------------------------------------------------------

bluefinParser::ExprContext* bluefinParser::UnaryExprContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

bluefinParser::UnaryExprContext::UnaryExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::UnaryExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryExpr(this);
}
void bluefinParser::UnaryExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryExpr(this);
}

antlrcpp::Any bluefinParser::UnaryExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitUnaryExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PrimaryParenthContext ------------------------------------------------------------------

bluefinParser::ExprContext* bluefinParser::PrimaryParenthContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

bluefinParser::PrimaryParenthContext::PrimaryParenthContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::PrimaryParenthContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrimaryParenth(this);
}
void bluefinParser::PrimaryParenthContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrimaryParenth(this);
}

antlrcpp::Any bluefinParser::PrimaryParenthContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitPrimaryParenth(this);
  else
    return visitor->visitChildren(this);
}
//----------------- AddExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::AddExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::AddExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::AddExprContext::AddExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::AddExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddExpr(this);
}
void bluefinParser::AddExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddExpr(this);
}

antlrcpp::Any bluefinParser::AddExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitAddExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- FuncCallContext ------------------------------------------------------------------

tree::TerminalNode* bluefinParser::FuncCallContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::ArgListContext* bluefinParser::FuncCallContext::argList() {
  return getRuleContext<bluefinParser::ArgListContext>(0);
}

bluefinParser::FuncCallContext::FuncCallContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::FuncCallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFuncCall(this);
}
void bluefinParser::FuncCallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFuncCall(this);
}

antlrcpp::Any bluefinParser::FuncCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitFuncCall(this);
  else
    return visitor->visitChildren(this);
}
//----------------- RelExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::RelExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::RelExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::RelExprContext::RelExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::RelExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRelExpr(this);
}
void bluefinParser::RelExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRelExpr(this);
}

antlrcpp::Any bluefinParser::RelExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitRelExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- EqualityExprContext ------------------------------------------------------------------

std::vector<bluefinParser::ExprContext *> bluefinParser::EqualityExprContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::EqualityExprContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}

bluefinParser::EqualityExprContext::EqualityExprContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::EqualityExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEqualityExpr(this);
}
void bluefinParser::EqualityExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEqualityExpr(this);
}

antlrcpp::Any bluefinParser::EqualityExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitEqualityExpr(this);
  else
    return visitor->visitChildren(this);
}
//----------------- MethodCallContext ------------------------------------------------------------------

bluefinParser::ExprContext* bluefinParser::MethodCallContext::expr() {
  return getRuleContext<bluefinParser::ExprContext>(0);
}

tree::TerminalNode* bluefinParser::MethodCallContext::ID() {
  return getToken(bluefinParser::ID, 0);
}

bluefinParser::ArgListContext* bluefinParser::MethodCallContext::argList() {
  return getRuleContext<bluefinParser::ArgListContext>(0);
}

bluefinParser::MethodCallContext::MethodCallContext(ExprContext *ctx) { copyFrom(ctx); }

void bluefinParser::MethodCallContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMethodCall(this);
}
void bluefinParser::MethodCallContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMethodCall(this);
}

antlrcpp::Any bluefinParser::MethodCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitMethodCall(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::ExprContext* bluefinParser::expr() {
   return expr(0);
}

bluefinParser::ExprContext* bluefinParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  bluefinParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  bluefinParser::ExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 34;
  enterRecursionRule(_localctx, 34, bluefinParser::RuleExpr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(172);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<PrimaryIntContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(155);
      match(bluefinParser::INT);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<PrimaryFloatContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(156);
      match(bluefinParser::FLOAT);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<PrimaryStringContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(157);
      match(bluefinParser::STRING);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<PrimaryIdContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(158);
      match(bluefinParser::ID);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<PrimaryBoolContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(159);
      match(bluefinParser::BOOL);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<PrimaryParenthContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(160);
      match(bluefinParser::T__0);
      setState(161);
      expr(0);
      setState(162);
      match(bluefinParser::T__1);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<FuncCallContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(164);
      match(bluefinParser::ID);
      setState(165);
      match(bluefinParser::T__0);
      setState(167);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << bluefinParser::T__0)
        | (1ULL << bluefinParser::T__16)
        | (1ULL << bluefinParser::T__17)
        | (1ULL << bluefinParser::FLOAT)
        | (1ULL << bluefinParser::BOOL)
        | (1ULL << bluefinParser::STRING)
        | (1ULL << bluefinParser::ID)
        | (1ULL << bluefinParser::INT))) != 0)) {
        setState(166);
        argList();
      }
      setState(169);
      match(bluefinParser::T__1);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<UnaryExprContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(170);
      dynamic_cast<UnaryExprContext *>(_localctx)->op = _input->LT(1);
      _la = _input->LA(1);
      if (!(_la == bluefinParser::T__16

      || _la == bluefinParser::T__17)) {
        dynamic_cast<UnaryExprContext *>(_localctx)->op = _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(171);
      expr(8);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(208);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(206);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 17, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<MultiExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(174);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(175);
          _la = _input->LA(1);
          if (!(_la == bluefinParser::T__18

          || _la == bluefinParser::T__19)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(176);
          expr(8);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<AddExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(177);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(178);
          _la = _input->LA(1);
          if (!(_la == bluefinParser::T__16

          || _la == bluefinParser::T__20)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(179);
          expr(7);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<RelExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(180);

          if (!(precpred(_ctx, 5))) throw FailedPredicateException(this, "precpred(_ctx, 5)");
          setState(181);
          _la = _input->LA(1);
          if (!((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << bluefinParser::T__21)
            | (1ULL << bluefinParser::T__22)
            | (1ULL << bluefinParser::T__23)
            | (1ULL << bluefinParser::T__24))) != 0))) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(182);
          expr(6);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<EqualityExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(183);

          if (!(precpred(_ctx, 4))) throw FailedPredicateException(this, "precpred(_ctx, 4)");
          setState(184);
          _la = _input->LA(1);
          if (!(_la == bluefinParser::T__25

          || _la == bluefinParser::T__26)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(185);
          expr(5);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<LogicalANDExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(186);

          if (!(precpred(_ctx, 3))) throw FailedPredicateException(this, "precpred(_ctx, 3)");

          setState(187);
          match(bluefinParser::T__27);
          setState(188);
          expr(4);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<LogicalORExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(189);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");

          setState(190);
          match(bluefinParser::T__28);
          setState(191);
          expr(3);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<SimpleAssignExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(192);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(193);
          match(bluefinParser::T__14);
          setState(194);
          expr(1);
          break;
        }

        case 8: {
          auto newContext = _tracker.createInstance<MethodCallContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(195);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(196);
          match(bluefinParser::T__15);
          setState(197);
          match(bluefinParser::ID);
          setState(198);
          match(bluefinParser::T__0);
          setState(200);
          _errHandler->sync(this);

          _la = _input->LA(1);
          if ((((_la & ~ 0x3fULL) == 0) &&
            ((1ULL << _la) & ((1ULL << bluefinParser::T__0)
            | (1ULL << bluefinParser::T__16)
            | (1ULL << bluefinParser::T__17)
            | (1ULL << bluefinParser::FLOAT)
            | (1ULL << bluefinParser::BOOL)
            | (1ULL << bluefinParser::STRING)
            | (1ULL << bluefinParser::ID)
            | (1ULL << bluefinParser::INT))) != 0)) {
            setState(199);
            argList();
          }
          setState(202);
          match(bluefinParser::T__1);
          break;
        }

        case 9: {
          auto newContext = _tracker.createInstance<MemberAccessContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(203);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(204);
          match(bluefinParser::T__15);
          setState(205);
          match(bluefinParser::ID);
          break;
        }

        } 
      }
      setState(210);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 18, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ArgListContext ------------------------------------------------------------------

bluefinParser::ArgListContext::ArgListContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<bluefinParser::ExprContext *> bluefinParser::ArgListContext::expr() {
  return getRuleContexts<bluefinParser::ExprContext>();
}

bluefinParser::ExprContext* bluefinParser::ArgListContext::expr(size_t i) {
  return getRuleContext<bluefinParser::ExprContext>(i);
}


size_t bluefinParser::ArgListContext::getRuleIndex() const {
  return bluefinParser::RuleArgList;
}

void bluefinParser::ArgListContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgList(this);
}

void bluefinParser::ArgListContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<bluefinListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgList(this);
}


antlrcpp::Any bluefinParser::ArgListContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<bluefinVisitor*>(visitor))
    return parserVisitor->visitArgList(this);
  else
    return visitor->visitChildren(this);
}

bluefinParser::ArgListContext* bluefinParser::argList() {
  ArgListContext *_localctx = _tracker.createInstance<ArgListContext>(_ctx, getState());
  enterRule(_localctx, 36, bluefinParser::RuleArgList);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    expr(0);
    setState(216);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == bluefinParser::T__4) {
      setState(212);
      match(bluefinParser::T__4);
      setState(213);
      expr(0);
      setState(218);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool bluefinParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 17: return exprSempred(dynamic_cast<ExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool bluefinParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 7);
    case 1: return precpred(_ctx, 6);
    case 2: return precpred(_ctx, 5);
    case 3: return precpred(_ctx, 4);
    case 4: return precpred(_ctx, 3);
    case 5: return precpred(_ctx, 2);
    case 6: return precpred(_ctx, 1);
    case 7: return precpred(_ctx, 10);
    case 8: return precpred(_ctx, 9);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> bluefinParser::_decisionToDFA;
atn::PredictionContextCache bluefinParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN bluefinParser::_atn;
std::vector<uint16_t> bluefinParser::_serializedATN;

std::vector<std::string> bluefinParser::_ruleNames = {
  "program", "funcDef", "type", "builtinType", "paramList", "param", "block", 
  "stmt", "stmtIf", "stmtWhile", "stmtReturn", "stmtBreak", "stmtContinue", 
  "stmtExpr", "structDef", "superClass", "varDecl", "expr", "argList"
};

std::vector<std::string> bluefinParser::_literalNames = {
  "", "'('", "')'", "'{'", "'}'", "','", "'if'", "'else'", "'while'", "'return'", 
  "';'", "'break'", "'continue'", "'struct'", "'extends'", "'='", "'.'", 
  "'-'", "'!'", "'*'", "'/'", "'+'", "'<'", "'<='", "'>'", "'>='", "'=='", 
  "'!='", "'&&'", "'||'", "'int'", "'float'", "'string'", "'bool'", "'void'"
};

std::vector<std::string> bluefinParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "", "", "", "", "", "", "", "TInt", "TFloat", "TString", 
  "TBool", "TVoid", "FLOAT", "BOOL", "STRING", "ID", "INT", "WS", "LINE_COMMENT", 
  "MULTILINE_COMMENT"
};

dfa::Vocabulary bluefinParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> bluefinParser::_tokenNames;

bluefinParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x2c, 0xde, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 0x4, 
    0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 0x12, 
    0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x3, 0x2, 0x3, 
    0x2, 0x3, 0x2, 0x6, 0x2, 0x2c, 0xa, 0x2, 0xd, 0x2, 0xe, 0x2, 0x2d, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x34, 0xa, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x39, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x3c, 
    0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x42, 0xa, 
    0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x49, 
    0xa, 0x6, 0xc, 0x6, 0xe, 0x6, 0x4c, 0xb, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x3, 0x8, 0x3, 0x8, 0x7, 0x8, 0x53, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 
    0x56, 0xb, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 
    0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x5, 0x9, 0x62, 0xa, 0x9, 
    0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 
    0x5, 0xa, 0x6b, 0xa, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 0x75, 0xa, 0xc, 0x3, 0xc, 
    0x3, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x5, 
    0x10, 0x85, 0xa, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x7, 0x10, 0x8a, 
    0xa, 0x10, 0xc, 0x10, 0xe, 0x10, 0x8d, 0xb, 0x10, 0x3, 0x10, 0x3, 0x10, 
    0x3, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 
    0x12, 0x3, 0x12, 0x5, 0x12, 0x99, 0xa, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 
    0x13, 0xaa, 0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 0x13, 0xaf, 
    0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 
    0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x5, 0x13, 0xcb, 0xa, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x7, 0x13, 0xd1, 0xa, 0x13, 0xc, 0x13, 0xe, 0x13, 0xd4, 0xb, 
    0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x7, 0x14, 0xd9, 0xa, 0x14, 0xc, 
    0x14, 0xe, 0x14, 0xdc, 0xb, 0x14, 0x3, 0x14, 0x2, 0x3, 0x24, 0x15, 0x2, 
    0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 
    0x1e, 0x20, 0x22, 0x24, 0x26, 0x2, 0x8, 0x3, 0x2, 0x20, 0x24, 0x3, 0x2, 
    0x13, 0x14, 0x3, 0x2, 0x15, 0x16, 0x4, 0x2, 0x13, 0x13, 0x17, 0x17, 
    0x3, 0x2, 0x18, 0x1b, 0x3, 0x2, 0x1c, 0x1d, 0x2, 0xf2, 0x2, 0x2b, 0x3, 
    0x2, 0x2, 0x2, 0x4, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x6, 0x41, 0x3, 0x2, 0x2, 
    0x2, 0x8, 0x43, 0x3, 0x2, 0x2, 0x2, 0xa, 0x45, 0x3, 0x2, 0x2, 0x2, 0xc, 
    0x4d, 0x3, 0x2, 0x2, 0x2, 0xe, 0x50, 0x3, 0x2, 0x2, 0x2, 0x10, 0x61, 
    0x3, 0x2, 0x2, 0x2, 0x12, 0x63, 0x3, 0x2, 0x2, 0x2, 0x14, 0x6c, 0x3, 
    0x2, 0x2, 0x2, 0x16, 0x72, 0x3, 0x2, 0x2, 0x2, 0x18, 0x78, 0x3, 0x2, 
    0x2, 0x2, 0x1a, 0x7b, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x7e, 0x3, 0x2, 0x2, 
    0x2, 0x1e, 0x81, 0x3, 0x2, 0x2, 0x2, 0x20, 0x91, 0x3, 0x2, 0x2, 0x2, 
    0x22, 0x94, 0x3, 0x2, 0x2, 0x2, 0x24, 0xae, 0x3, 0x2, 0x2, 0x2, 0x26, 
    0xd5, 0x3, 0x2, 0x2, 0x2, 0x28, 0x2c, 0x5, 0x4, 0x3, 0x2, 0x29, 0x2c, 
    0x5, 0x22, 0x12, 0x2, 0x2a, 0x2c, 0x5, 0x1e, 0x10, 0x2, 0x2b, 0x28, 
    0x3, 0x2, 0x2, 0x2, 0x2b, 0x29, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x2a, 0x3, 
    0x2, 0x2, 0x2, 0x2c, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x2b, 0x3, 0x2, 
    0x2, 0x2, 0x2d, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x2f, 0x30, 0x5, 0x6, 0x4, 0x2, 0x30, 0x31, 0x7, 0x28, 0x2, 0x2, 
    0x31, 0x33, 0x7, 0x3, 0x2, 0x2, 0x32, 0x34, 0x5, 0xa, 0x6, 0x2, 0x33, 
    0x32, 0x3, 0x2, 0x2, 0x2, 0x33, 0x34, 0x3, 0x2, 0x2, 0x2, 0x34, 0x35, 
    0x3, 0x2, 0x2, 0x2, 0x35, 0x36, 0x7, 0x4, 0x2, 0x2, 0x36, 0x3a, 0x7, 
    0x5, 0x2, 0x2, 0x37, 0x39, 0x5, 0x10, 0x9, 0x2, 0x38, 0x37, 0x3, 0x2, 
    0x2, 0x2, 0x39, 0x3c, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x38, 0x3, 0x2, 0x2, 
    0x2, 0x3a, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x3d, 0x3, 0x2, 0x2, 0x2, 
    0x3c, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x3d, 0x3e, 0x7, 0x6, 0x2, 0x2, 0x3e, 
    0x5, 0x3, 0x2, 0x2, 0x2, 0x3f, 0x42, 0x5, 0x8, 0x5, 0x2, 0x40, 0x42, 
    0x7, 0x28, 0x2, 0x2, 0x41, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x41, 0x40, 0x3, 
    0x2, 0x2, 0x2, 0x42, 0x7, 0x3, 0x2, 0x2, 0x2, 0x43, 0x44, 0x9, 0x2, 
    0x2, 0x2, 0x44, 0x9, 0x3, 0x2, 0x2, 0x2, 0x45, 0x4a, 0x5, 0xc, 0x7, 
    0x2, 0x46, 0x47, 0x7, 0x7, 0x2, 0x2, 0x47, 0x49, 0x5, 0xc, 0x7, 0x2, 
    0x48, 0x46, 0x3, 0x2, 0x2, 0x2, 0x49, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x4a, 
    0x48, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x4b, 0xb, 
    0x3, 0x2, 0x2, 0x2, 0x4c, 0x4a, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4e, 0x5, 
    0x6, 0x4, 0x2, 0x4e, 0x4f, 0x7, 0x28, 0x2, 0x2, 0x4f, 0xd, 0x3, 0x2, 
    0x2, 0x2, 0x50, 0x54, 0x7, 0x5, 0x2, 0x2, 0x51, 0x53, 0x5, 0x10, 0x9, 
    0x2, 0x52, 0x51, 0x3, 0x2, 0x2, 0x2, 0x53, 0x56, 0x3, 0x2, 0x2, 0x2, 
    0x54, 0x52, 0x3, 0x2, 0x2, 0x2, 0x54, 0x55, 0x3, 0x2, 0x2, 0x2, 0x55, 
    0x57, 0x3, 0x2, 0x2, 0x2, 0x56, 0x54, 0x3, 0x2, 0x2, 0x2, 0x57, 0x58, 
    0x7, 0x6, 0x2, 0x2, 0x58, 0xf, 0x3, 0x2, 0x2, 0x2, 0x59, 0x62, 0x5, 
    0x12, 0xa, 0x2, 0x5a, 0x62, 0x5, 0x14, 0xb, 0x2, 0x5b, 0x62, 0x5, 0x16, 
    0xc, 0x2, 0x5c, 0x62, 0x5, 0x18, 0xd, 0x2, 0x5d, 0x62, 0x5, 0x1a, 0xe, 
    0x2, 0x5e, 0x62, 0x5, 0xe, 0x8, 0x2, 0x5f, 0x62, 0x5, 0x1c, 0xf, 0x2, 
    0x60, 0x62, 0x5, 0x22, 0x12, 0x2, 0x61, 0x59, 0x3, 0x2, 0x2, 0x2, 0x61, 
    0x5a, 0x3, 0x2, 0x2, 0x2, 0x61, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x61, 0x5c, 
    0x3, 0x2, 0x2, 0x2, 0x61, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x61, 0x5e, 0x3, 
    0x2, 0x2, 0x2, 0x61, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x61, 0x60, 0x3, 0x2, 
    0x2, 0x2, 0x62, 0x11, 0x3, 0x2, 0x2, 0x2, 0x63, 0x64, 0x7, 0x8, 0x2, 
    0x2, 0x64, 0x65, 0x7, 0x3, 0x2, 0x2, 0x65, 0x66, 0x5, 0x24, 0x13, 0x2, 
    0x66, 0x67, 0x7, 0x4, 0x2, 0x2, 0x67, 0x6a, 0x5, 0xe, 0x8, 0x2, 0x68, 
    0x69, 0x7, 0x9, 0x2, 0x2, 0x69, 0x6b, 0x5, 0xe, 0x8, 0x2, 0x6a, 0x68, 
    0x3, 0x2, 0x2, 0x2, 0x6a, 0x6b, 0x3, 0x2, 0x2, 0x2, 0x6b, 0x13, 0x3, 
    0x2, 0x2, 0x2, 0x6c, 0x6d, 0x7, 0xa, 0x2, 0x2, 0x6d, 0x6e, 0x7, 0x3, 
    0x2, 0x2, 0x6e, 0x6f, 0x5, 0x24, 0x13, 0x2, 0x6f, 0x70, 0x7, 0x4, 0x2, 
    0x2, 0x70, 0x71, 0x5, 0xe, 0x8, 0x2, 0x71, 0x15, 0x3, 0x2, 0x2, 0x2, 
    0x72, 0x74, 0x7, 0xb, 0x2, 0x2, 0x73, 0x75, 0x5, 0x24, 0x13, 0x2, 0x74, 
    0x73, 0x3, 0x2, 0x2, 0x2, 0x74, 0x75, 0x3, 0x2, 0x2, 0x2, 0x75, 0x76, 
    0x3, 0x2, 0x2, 0x2, 0x76, 0x77, 0x7, 0xc, 0x2, 0x2, 0x77, 0x17, 0x3, 
    0x2, 0x2, 0x2, 0x78, 0x79, 0x7, 0xd, 0x2, 0x2, 0x79, 0x7a, 0x7, 0xc, 
    0x2, 0x2, 0x7a, 0x19, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x7c, 0x7, 0xe, 0x2, 
    0x2, 0x7c, 0x7d, 0x7, 0xc, 0x2, 0x2, 0x7d, 0x1b, 0x3, 0x2, 0x2, 0x2, 
    0x7e, 0x7f, 0x5, 0x24, 0x13, 0x2, 0x7f, 0x80, 0x7, 0xc, 0x2, 0x2, 0x80, 
    0x1d, 0x3, 0x2, 0x2, 0x2, 0x81, 0x82, 0x7, 0xf, 0x2, 0x2, 0x82, 0x84, 
    0x7, 0x28, 0x2, 0x2, 0x83, 0x85, 0x5, 0x20, 0x11, 0x2, 0x84, 0x83, 0x3, 
    0x2, 0x2, 0x2, 0x84, 0x85, 0x3, 0x2, 0x2, 0x2, 0x85, 0x86, 0x3, 0x2, 
    0x2, 0x2, 0x86, 0x8b, 0x7, 0x5, 0x2, 0x2, 0x87, 0x8a, 0x5, 0x22, 0x12, 
    0x2, 0x88, 0x8a, 0x5, 0x4, 0x3, 0x2, 0x89, 0x87, 0x3, 0x2, 0x2, 0x2, 
    0x89, 0x88, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x8d, 0x3, 0x2, 0x2, 0x2, 0x8b, 
    0x89, 0x3, 0x2, 0x2, 0x2, 0x8b, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8e, 
    0x3, 0x2, 0x2, 0x2, 0x8d, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x8f, 0x7, 
    0x6, 0x2, 0x2, 0x8f, 0x90, 0x7, 0xc, 0x2, 0x2, 0x90, 0x1f, 0x3, 0x2, 
    0x2, 0x2, 0x91, 0x92, 0x7, 0x10, 0x2, 0x2, 0x92, 0x93, 0x7, 0x28, 0x2, 
    0x2, 0x93, 0x21, 0x3, 0x2, 0x2, 0x2, 0x94, 0x95, 0x5, 0x6, 0x4, 0x2, 
    0x95, 0x98, 0x7, 0x28, 0x2, 0x2, 0x96, 0x97, 0x7, 0x11, 0x2, 0x2, 0x97, 
    0x99, 0x5, 0x24, 0x13, 0x2, 0x98, 0x96, 0x3, 0x2, 0x2, 0x2, 0x98, 0x99, 
    0x3, 0x2, 0x2, 0x2, 0x99, 0x9a, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x9b, 0x7, 
    0xc, 0x2, 0x2, 0x9b, 0x23, 0x3, 0x2, 0x2, 0x2, 0x9c, 0x9d, 0x8, 0x13, 
    0x1, 0x2, 0x9d, 0xaf, 0x7, 0x29, 0x2, 0x2, 0x9e, 0xaf, 0x7, 0x25, 0x2, 
    0x2, 0x9f, 0xaf, 0x7, 0x27, 0x2, 0x2, 0xa0, 0xaf, 0x7, 0x28, 0x2, 0x2, 
    0xa1, 0xaf, 0x7, 0x26, 0x2, 0x2, 0xa2, 0xa3, 0x7, 0x3, 0x2, 0x2, 0xa3, 
    0xa4, 0x5, 0x24, 0x13, 0x2, 0xa4, 0xa5, 0x7, 0x4, 0x2, 0x2, 0xa5, 0xaf, 
    0x3, 0x2, 0x2, 0x2, 0xa6, 0xa7, 0x7, 0x28, 0x2, 0x2, 0xa7, 0xa9, 0x7, 
    0x3, 0x2, 0x2, 0xa8, 0xaa, 0x5, 0x26, 0x14, 0x2, 0xa9, 0xa8, 0x3, 0x2, 
    0x2, 0x2, 0xa9, 0xaa, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xab, 0x3, 0x2, 0x2, 
    0x2, 0xab, 0xaf, 0x7, 0x4, 0x2, 0x2, 0xac, 0xad, 0x9, 0x3, 0x2, 0x2, 
    0xad, 0xaf, 0x5, 0x24, 0x13, 0xa, 0xae, 0x9c, 0x3, 0x2, 0x2, 0x2, 0xae, 
    0x9e, 0x3, 0x2, 0x2, 0x2, 0xae, 0x9f, 0x3, 0x2, 0x2, 0x2, 0xae, 0xa0, 
    0x3, 0x2, 0x2, 0x2, 0xae, 0xa1, 0x3, 0x2, 0x2, 0x2, 0xae, 0xa2, 0x3, 
    0x2, 0x2, 0x2, 0xae, 0xa6, 0x3, 0x2, 0x2, 0x2, 0xae, 0xac, 0x3, 0x2, 
    0x2, 0x2, 0xaf, 0xd2, 0x3, 0x2, 0x2, 0x2, 0xb0, 0xb1, 0xc, 0x9, 0x2, 
    0x2, 0xb1, 0xb2, 0x9, 0x4, 0x2, 0x2, 0xb2, 0xd1, 0x5, 0x24, 0x13, 0xa, 
    0xb3, 0xb4, 0xc, 0x8, 0x2, 0x2, 0xb4, 0xb5, 0x9, 0x5, 0x2, 0x2, 0xb5, 
    0xd1, 0x5, 0x24, 0x13, 0x9, 0xb6, 0xb7, 0xc, 0x7, 0x2, 0x2, 0xb7, 0xb8, 
    0x9, 0x6, 0x2, 0x2, 0xb8, 0xd1, 0x5, 0x24, 0x13, 0x8, 0xb9, 0xba, 0xc, 
    0x6, 0x2, 0x2, 0xba, 0xbb, 0x9, 0x7, 0x2, 0x2, 0xbb, 0xd1, 0x5, 0x24, 
    0x13, 0x7, 0xbc, 0xbd, 0xc, 0x5, 0x2, 0x2, 0xbd, 0xbe, 0x7, 0x1e, 0x2, 
    0x2, 0xbe, 0xd1, 0x5, 0x24, 0x13, 0x6, 0xbf, 0xc0, 0xc, 0x4, 0x2, 0x2, 
    0xc0, 0xc1, 0x7, 0x1f, 0x2, 0x2, 0xc1, 0xd1, 0x5, 0x24, 0x13, 0x5, 0xc2, 
    0xc3, 0xc, 0x3, 0x2, 0x2, 0xc3, 0xc4, 0x7, 0x11, 0x2, 0x2, 0xc4, 0xd1, 
    0x5, 0x24, 0x13, 0x3, 0xc5, 0xc6, 0xc, 0xc, 0x2, 0x2, 0xc6, 0xc7, 0x7, 
    0x12, 0x2, 0x2, 0xc7, 0xc8, 0x7, 0x28, 0x2, 0x2, 0xc8, 0xca, 0x7, 0x3, 
    0x2, 0x2, 0xc9, 0xcb, 0x5, 0x26, 0x14, 0x2, 0xca, 0xc9, 0x3, 0x2, 0x2, 
    0x2, 0xca, 0xcb, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 0x3, 0x2, 0x2, 0x2, 
    0xcc, 0xd1, 0x7, 0x4, 0x2, 0x2, 0xcd, 0xce, 0xc, 0xb, 0x2, 0x2, 0xce, 
    0xcf, 0x7, 0x12, 0x2, 0x2, 0xcf, 0xd1, 0x7, 0x28, 0x2, 0x2, 0xd0, 0xb0, 
    0x3, 0x2, 0x2, 0x2, 0xd0, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xd0, 0xb6, 0x3, 
    0x2, 0x2, 0x2, 0xd0, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xd0, 0xbc, 0x3, 0x2, 
    0x2, 0x2, 0xd0, 0xbf, 0x3, 0x2, 0x2, 0x2, 0xd0, 0xc2, 0x3, 0x2, 0x2, 
    0x2, 0xd0, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xd0, 0xcd, 0x3, 0x2, 0x2, 0x2, 
    0xd1, 0xd4, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd0, 0x3, 0x2, 0x2, 0x2, 0xd2, 
    0xd3, 0x3, 0x2, 0x2, 0x2, 0xd3, 0x25, 0x3, 0x2, 0x2, 0x2, 0xd4, 0xd2, 
    0x3, 0x2, 0x2, 0x2, 0xd5, 0xda, 0x5, 0x24, 0x13, 0x2, 0xd6, 0xd7, 0x7, 
    0x7, 0x2, 0x2, 0xd7, 0xd9, 0x5, 0x24, 0x13, 0x2, 0xd8, 0xd6, 0x3, 0x2, 
    0x2, 0x2, 0xd9, 0xdc, 0x3, 0x2, 0x2, 0x2, 0xda, 0xd8, 0x3, 0x2, 0x2, 
    0x2, 0xda, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xdb, 0x27, 0x3, 0x2, 0x2, 0x2, 
    0xdc, 0xda, 0x3, 0x2, 0x2, 0x2, 0x16, 0x2b, 0x2d, 0x33, 0x3a, 0x41, 
    0x4a, 0x54, 0x61, 0x6a, 0x74, 0x84, 0x89, 0x8b, 0x98, 0xa9, 0xae, 0xca, 
    0xd0, 0xd2, 0xda, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

bluefinParser::Initializer bluefinParser::_init;
