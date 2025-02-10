#include "script/neo.hpp"
#include <gtest/gtest.h>
class TestParser : public ::testing::Test {
protected:
};
TEST_F(TestParser, EmptyProgram) {
  neo::JSParser parser;
  std::wstring source = LR"(    
    
  )";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  delete node;
}

TEST_F(TestParser, ProgramInterpreter) {
  neo::JSParser parser;
  std::wstring source = LR"(#!/usr/bin/neo
   /*test comment*/)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_NE(program->interpreter, nullptr);
  ASSERT_EQ(program->interpreter->location.get(source), L"#!/usr/bin/neo");
  delete node;
}

TEST_F(TestParser, ProgramDirectives) {
  neo::JSParser parser;
  std::wstring source = LR"(
'use a';
'use b';/*test comment2*/'use c'/*
test multiline
*/'use d')";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_EQ(program->directives.size(), 4);
  ASSERT_EQ(program->directives[0]->location.get(source), L"'use a'");
  ASSERT_EQ(program->directives[1]->location.get(source), L"'use b'");
  ASSERT_EQ(program->directives[2]->location.get(source), L"'use c'");
  ASSERT_EQ(program->directives[3]->location.get(source), L"'use d'");
  delete node;
}

TEST_F(TestParser, EmptyStatement) {
  neo::JSParser parser;
  std::wstring source = LR"(;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_EMPTY);
  delete node;
}
TEST_F(TestParser, DebuggerStatement) {
  neo::JSParser parser;
  std::wstring source = LR"(debugger;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_DEBUGGER);
  delete node;
}
TEST_F(TestParser, DebuggerWithMultilineCommentExpression) {
  neo::JSParser parser;
  std::wstring source = LR"(debugger /*aaaa
*/"Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_DEBUGGER);
  ASSERT_EQ(program->body[1]->type, neo::JS_NODE_TYPE::STATEMENT_EXPRESSION);
  auto statement =
      dynamic_cast<neo::JSExpressionStatementNode *>(program->body[1]);
  ASSERT_EQ(statement->expression->type, neo::JS_NODE_TYPE::LITERAL_STRING);
  delete node;
}

TEST_F(TestParser, DebuggerWithExpression) {
  neo::JSParser parser;
  std::wstring source = LR"(debugger "Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::ERROR);
  delete node;
}

TEST_F(TestParser, DebuggerWithNewlineExpression) {
  neo::JSParser parser;
  std::wstring source = LR"(debugger 
"Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_DEBUGGER);
  ASSERT_EQ(program->body[1]->type, neo::JS_NODE_TYPE::STATEMENT_EXPRESSION);
  auto statement =
      dynamic_cast<neo::JSExpressionStatementNode *>(program->body[1]);
  ASSERT_EQ(statement->expression->type, neo::JS_NODE_TYPE::LITERAL_STRING);
  delete node;
}

TEST_F(TestParser, Return) {
  neo::JSParser parser;
  std::wstring source = LR"(return)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_RETURN);
  delete node;
}
TEST_F(TestParser, ReturnValue) {
  neo::JSParser parser;
  std::wstring source = LR"(return a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<neo::JSReturnStatement *>(program->body[0]);
  ASSERT_EQ(ret->value->type, neo::JS_NODE_TYPE::LITERAL_IDENTITY);
  delete node;
}
TEST_F(TestParser, ReturnUselessValue) {
  neo::JSParser parser;
  std::wstring source = LR"(return
a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<neo::JSReturnStatement *>(program->body[0]);
  ASSERT_EQ(ret->value, nullptr);
  delete node;
}
TEST_F(TestParser, ReturnCommentUselessValue) {
  neo::JSParser parser;
  std::wstring source = LR"(return /*
*/a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<neo::JSReturnStatement *>(program->body[0]);
  ASSERT_EQ(ret->value, nullptr);
  delete node;
}

TEST_F(TestParser, ReturnCommentValue) {
  neo::JSParser parser;
  std::wstring source = LR"(return /**/ a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<neo::JSReturnStatement *>(program->body[0]);
  ASSERT_EQ(ret->value->type, neo::JS_NODE_TYPE::LITERAL_IDENTITY);
  delete node;
}

TEST_F(TestParser, Label) {
  neo::JSParser parser;
  std::wstring source = LR"(a:b)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_LABEL);
  auto lab = dynamic_cast<neo::JSLabelStatement *>(program->body[0]);
  ASSERT_EQ(lab->label->location.get(source), L"a");
}
TEST_F(TestParser, LabelComment) {
  neo::JSParser parser;
  std::wstring source = LR"(a //test
:b)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_LABEL);
  auto lab = dynamic_cast<neo::JSLabelStatement *>(program->body[0]);
  ASSERT_EQ(lab->label->location.get(source), L"a");
}
TEST_F(TestParser, LabelWithoutBody) {
  neo::JSParser parser;
  std::wstring source = LR"(a :)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::ERROR);
}

TEST_F(TestParser, Break) {
  neo::JSParser parser;
  std::wstring source = LR"(break)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<neo::JSBreakStatement *>(program->body[0]);
  ASSERT_EQ(bre->label, nullptr);
}
TEST_F(TestParser, BreakLabel) {
  neo::JSParser parser;
  std::wstring source = LR"(break test)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<neo::JSBreakStatement *>(program->body[0]);
  ASSERT_EQ(bre->label->location.get(source), L"test");
}
TEST_F(TestParser, BreakUseLessLabel) {
  neo::JSParser parser;
  std::wstring source = LR"(break 
test)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<neo::JSBreakStatement *>(program->body[0]);
  ASSERT_EQ(bre->label, nullptr);
}
TEST_F(TestParser, BreakError) {
  neo::JSParser parser;
  std::wstring source = LR"(break "test")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::ERROR);
}
TEST_F(TestParser, If) {
  neo::JSParser parser;
  std::wstring source = LR"(if(a);)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_IF);
  auto ifs = dynamic_cast<neo::JSIfStatement *>(program->body[0]);
  ASSERT_EQ(ifs->condition->location.get(source), L"a");
  ASSERT_EQ(ifs->consequent->type, neo::JS_NODE_TYPE::STATEMENT_EMPTY);
  ASSERT_EQ(ifs->alternate, nullptr);
}
TEST_F(TestParser, IfElse) {
  neo::JSParser parser;
  std::wstring source = LR"(if(a);else;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<neo::JSProgramNode *>(node);
  ASSERT_TRUE(program->body.size() > 0);
  ASSERT_EQ(program->body[0]->type, neo::JS_NODE_TYPE::STATEMENT_IF);
  auto ifs = dynamic_cast<neo::JSIfStatement *>(program->body[0]);
  ASSERT_EQ(ifs->condition->location.get(source), L"a");
  ASSERT_EQ(ifs->consequent->type, neo::JS_NODE_TYPE::STATEMENT_EMPTY);
  ASSERT_EQ(ifs->alternate->type, neo::JS_NODE_TYPE::STATEMENT_EMPTY);
}
TEST_F(TestParser, IfWithoutAlt) {
  neo::JSParser parser;
  std::wstring source = LR"(if(a)else;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, neo::JS_NODE_TYPE::ERROR);
}