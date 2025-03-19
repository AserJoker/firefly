#include "script/compiler/JSParser.hpp"
#include <gtest/gtest.h>
#include <string>
class TestParser : public ::testing::Test {};
TEST_F(TestParser, EmptyProgram) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(    
    
  )";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  delete node;
}

TEST_F(TestParser, ProgramInterpreter) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(#!/usr/bin/neo
   /*test comment*/)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_NE(program->interpreter, nullptr);
  ASSERT_EQ(program->interpreter->location.get(source), L"#!/usr/bin/neo");
  delete node;
}

TEST_F(TestParser, ProgramDirectives) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(
'use a';
'use b';/*test comment2*/'use c'/*
test multiline
*/'use d')";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_EQ(program->directives.size(), 4);
  ASSERT_EQ(program->directives[0]->location.get(source), L"'use a'");
  ASSERT_EQ(program->directives[1]->location.get(source), L"'use b'");
  ASSERT_EQ(program->directives[2]->location.get(source), L"'use c'");
  ASSERT_EQ(program->directives[3]->location.get(source), L"'use d'");
  delete node;
}

TEST_F(TestParser, EmptyStatement) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_EMPTY);
  delete node;
}
TEST_F(TestParser, DebuggerStatement) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(debugger;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type,
            JS_NODE_TYPE::STATEMENT_DEBUGGER);
  delete node;
}
TEST_F(TestParser, DebuggerWithMultilineCommentExpression) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(debugger /*aaaa
*/"Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type,
            JS_NODE_TYPE::STATEMENT_DEBUGGER);
  ASSERT_EQ(program->statements[1]->type,
            JS_NODE_TYPE::STATEMENT_EXPRESSION);
  auto statement =
      dynamic_cast<JSExpressionStatementNode *>(program->statements[1]);
  ASSERT_EQ(statement->expression->type, JS_NODE_TYPE::LITERAL_STRING);
  delete node;
}

TEST_F(TestParser, DebuggerWithExpression) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(debugger "Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::ERROR);
  delete node;
}

TEST_F(TestParser, DebuggerWithNewlineExpression) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(debugger 
"Hello world")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type,
            JS_NODE_TYPE::STATEMENT_DEBUGGER);
  ASSERT_EQ(program->statements[1]->type,
            JS_NODE_TYPE::STATEMENT_EXPRESSION);
  auto statement =
      dynamic_cast<JSExpressionStatementNode *>(program->statements[1]);
  ASSERT_EQ(statement->expression->type, JS_NODE_TYPE::LITERAL_STRING);
  delete node;
}

TEST_F(TestParser, Return) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(return)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_RETURN);
  delete node;
}
TEST_F(TestParser, ReturnValue) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(return a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<JSReturnStatementNode *>(program->statements[0]);
  ASSERT_NE(ret->value, nullptr);
  ASSERT_EQ(ret->value->type, JS_NODE_TYPE::LITERAL_IDENTITY);
  delete node;
}
TEST_F(TestParser, ReturnUselessValue) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(return
a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<JSReturnStatementNode *>(program->statements[0]);
  ASSERT_EQ(ret->value, nullptr);
  delete node;
}
TEST_F(TestParser, ReturnCommentUselessValue) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(return /*
*/a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<JSReturnStatementNode *>(program->statements[0]);
  ASSERT_EQ(ret->value, nullptr);
  delete node;
}

TEST_F(TestParser, ReturnCommentValue) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(return /**/ a)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_RETURN);
  auto ret = dynamic_cast<JSReturnStatementNode *>(program->statements[0]);
  ASSERT_EQ(ret->value->type, JS_NODE_TYPE::LITERAL_IDENTITY);
  delete node;
}

TEST_F(TestParser, Label) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(a:b)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_LABEL);
  auto lab = dynamic_cast<JSLabelStatementNode *>(program->statements[0]);
  ASSERT_EQ(lab->label->location.get(source), L"a");
}
TEST_F(TestParser, LabelComment) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(a //test
:b)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_LABEL);
  auto lab = dynamic_cast<JSLabelStatementNode *>(program->statements[0]);
  ASSERT_EQ(lab->label->location.get(source), L"a");
}
TEST_F(TestParser, LabelWithoutBody) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(a :)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::ERROR);
}

TEST_F(TestParser, Break) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(break)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<JSBreakStatementNode *>(program->statements[0]);
  ASSERT_EQ(bre->label, nullptr);
}
TEST_F(TestParser, BreakLabel) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(break test)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<JSBreakStatementNode *>(program->statements[0]);
  ASSERT_EQ(bre->label->location.get(source), L"test");
}
TEST_F(TestParser, BreakUseLessLabel) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(break 
test)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_BREAK);
  auto bre = dynamic_cast<JSBreakStatementNode *>(program->statements[0]);
  ASSERT_EQ(bre->label, nullptr);
}
TEST_F(TestParser, BreakError) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(break "test")";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::ERROR);
}
TEST_F(TestParser, If) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(if(a);)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_IF);
  auto ifs = dynamic_cast<JSIfStatementNode *>(program->statements[0]);
  ASSERT_EQ(ifs->condition->location.get(source), L"a");
  ASSERT_EQ(ifs->consequent->type, JS_NODE_TYPE::STATEMENT_EMPTY);
  ASSERT_EQ(ifs->alternate, nullptr);
}
TEST_F(TestParser, IfElse) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(if(a);else;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_TRUE(program->statements.size() > 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_IF);
  auto ifs = dynamic_cast<JSIfStatementNode *>(program->statements[0]);
  ASSERT_EQ(ifs->condition->location.get(source), L"a");
  ASSERT_EQ(ifs->consequent->type, JS_NODE_TYPE::STATEMENT_EMPTY);
  ASSERT_EQ(ifs->alternate->type, JS_NODE_TYPE::STATEMENT_EMPTY);
}
TEST_F(TestParser, IfWithoutAlt) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(if(a)else;)";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::ERROR);
}
TEST_F(TestParser, Switch) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(switch(a+b){case a:})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_SWITCH);
  auto sw = dynamic_cast<JSSwitchStatementNode *>(program->statements[0]);
  ASSERT_EQ(sw->cases.size(), 1);
  auto cas = dynamic_cast<JSSwitchCaseStatementNode *>(sw->cases[0]);
  ASSERT_EQ(cas->match->location.get(source), L"a");
  ASSERT_EQ(cas->statements.size(), 0);
}
TEST_F(TestParser, SwitchNoCase) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(switch(a+b){})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_SWITCH);
  auto sw = dynamic_cast<JSSwitchStatementNode *>(program->statements[0]);
  ASSERT_EQ(sw->cases.size(), 0);
}
TEST_F(TestParser, SwitchDefault) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(switch(a+b){case 1:default:})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::STATEMENT_SWITCH);
  auto sw = dynamic_cast<JSSwitchStatementNode *>(program->statements[0]);
  ASSERT_EQ(sw->cases.size(), 2);
  auto cas = dynamic_cast<JSSwitchCaseStatementNode *>(sw->cases[1]);
  ASSERT_EQ(cas->match, nullptr);
}
TEST_F(TestParser, Class) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test extends A().B {;;;;;})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->identifier->location.get(source), L"Test");
  ASSERT_EQ(clazz->extends->location.get(source), L"A().B");
  ASSERT_EQ(clazz->properties.size(), 0);
}
TEST_F(TestParser, ClassProperty) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { a= 1
b=2;c=3})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 3);
}
TEST_F(TestParser, ClassPropertyError) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { a= 1 b=2})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::ERROR);
}
TEST_F(TestParser, ClassStaticProperty) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static a = 1 })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassPropertyNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->static_, true);
  ASSERT_EQ(prop->identifier->location.get(source), L"a");
  ASSERT_EQ(prop->value->location.get(source), L"1");
}
TEST_F(TestParser, ClassComputedProperty) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test {  ["aaa"+"bbb"] = 1 })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassPropertyNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->computed, true);
  ASSERT_EQ(prop->identifier->location.get(source), LR"("aaa"+"bbb")");
  ASSERT_EQ(prop->value->location.get(source), L"1");
}
TEST_F(TestParser, ClassMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test {  method(){}method2(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 2);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(method)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
}

TEST_F(TestParser, ClassStaticMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static method(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(method)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->static_, true);
}
TEST_F(TestParser, ClassAsyncMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { async method(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(method)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->async, true);
}
TEST_F(TestParser, ClassGeneratorMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { * method(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(method)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->generator, true);
}
TEST_F(TestParser, ClassStaticNamedMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(static)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->static_, false);
}
TEST_F(TestParser, ClassAsyncNamedMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static async(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(async)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->static_, true);
}
TEST_F(TestParser, ClassFullNamedMethod) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static async* method(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassMethodNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(method)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->static_, true);
  ASSERT_EQ(prop->async, true);
  ASSERT_EQ(prop->generator, true);
}
TEST_F(TestParser, ClassGetAccessor) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { get data(){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassAccessorNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(data)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->kind, JS_ACCESSOR_TYPE::GET);
}
TEST_F(TestParser, ClassSetAccessor) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { set data(data){} })";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassAccessorNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(data)");
  ASSERT_EQ(prop->arguments.size(), 1);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->kind, JS_ACCESSOR_TYPE::SET);
}
TEST_F(TestParser, ClassStaticAccessor) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static get data(){} })";
  auto node = parser.parse(source,JS_EVAL_TYPE::PROGRAM);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSClassAccessorNode *>(clazz->properties[0]);
  ASSERT_EQ(prop->identifier->location.get(source), LR"(data)");
  ASSERT_EQ(prop->arguments.size(), 0);
  ASSERT_EQ(prop->body->location.get(source), L"{}");
  ASSERT_EQ(prop->kind, JS_ACCESSOR_TYPE::GET);
  ASSERT_EQ(prop->static_, true);
}
TEST_F(TestParser, ClassStaticBlock) {
  JSParser parser{new JSAllocator};
  std::wstring source = LR"(class Test { static{}})";
  auto node = parser.parse(source);
  ASSERT_EQ(node->type, JS_NODE_TYPE::PROGRAM);
  auto program = dynamic_cast<JSProgramNode *>(node);
  ASSERT_GT(program->statements.size(), 0);
  ASSERT_EQ(program->statements[0]->type, JS_NODE_TYPE::DECLARATION_CLASS);
  auto clazz =
      dynamic_cast<JSClassDeclarationNode *>(program->statements[0]);
  ASSERT_EQ(clazz->properties.size(), 1);
  auto prop = dynamic_cast<JSStaticBlockNode *>(clazz->properties[0]);
  ASSERT_NE(prop->statement, nullptr);
}
