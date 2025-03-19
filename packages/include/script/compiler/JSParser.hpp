#pragma once

#include "../engine/JSEvalType.hpp"
#include "../util/JSAllocator.hpp"
#include "../util/JSLocation.hpp"
#include "../util/JSON.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <string>
#include <vector>

enum class JS_ACCESSOR_TYPE { GET, SET };

enum class JS_DECLARATION_TYPE {
  VAR,
  CONST,
  LET,
  FUNCTION,
};

enum class JS_NODE_TYPE {

  ERROR,
  TOKEN,
  PRIVATE_NAME,
  LITERAL_REGEX,
  LITERAL_NULL,
  LITERAL_STRING,
  LITERAL_BOOLEAN,
  LITERAL_NUMBER,
  LITERAL_COMMENT,
  LITERAL_MULTILINE_COMMENT,
  LITERAL_UNDEFINED,
  LITERAL_IDENTITY,
  LITERAL_TEMPLATE,
  LITERAL_BIGINT,
  // LITERAL_DECIMAL,
  LITERAL_THIS,
  LITERAL_SUPER,
  PROGRAM,
  STATEMENT_EMPTY,
  STATEMENT_BLOCK,
  STATEMENT_DEBUGGER,
  STATEMENT_RETURN,
  STATEMENT_LABEL,
  STATEMENT_BREAK,
  STATEMENT_CONTINUE,
  STATEMENT_IF,
  STATEMENT_SWITCH,
  STATEMENT_SWITCH_CASE,
  STATEMENT_THROW,
  STATEMENT_TRY,
  STATEMENT_TRY_CATCH,
  STATEMENT_WHILE,
  STATEMENT_DO_WHILE,
  STATEMENT_FOR,
  STATEMENT_FOR_IN,
  STATEMENT_FOR_OF,
  STATEMENT_FOR_AWAIT_OF,
  STATEMENT_EXPRESSION,
  VARIABLE_DECLARATOR,
  DECORATOR,
  DIRECTIVE,
  INTERPRETER_DIRECTIVE,
  OBJECT_PROPERTY,
  OBJECT_METHOD,
  OBJECT_ACCESSOR,
  // EXPRESSION_RECORD,
  // EXPRESSION_TUPLE,
  EXPRESSION_BINARY,
  EXPRESSION_MEMBER,
  EXPRESSION_OPTIONAL_MEMBER,
  EXPRESSION_COMPUTED_MEMBER,
  EXPRESSION_OPTIONAL_COMPUTED_MEMBER,
  EXPRESSION_CONDITION,
  EXPRESSION_CALL,
  EXPRESSION_OPTIONAL_CALL,
  EXPRESSION_NEW,
  EXPRESSION_DELETE,
  EXPRESSION_AWAIT,
  EXPRESSION_YIELD,
  EXPRESSION_YIELD_DELEGATE,
  EXPRESSION_VOID,
  EXPRESSION_TYPEOF,
  EXPRESSION_GROUP,
  EXPRESSION_ASSIGMENT,
  EXPRESSION_SPREAD,
  PATTERN_SPREAD_ITEM,
  PATTERN_OBJECT,
  PATTERN_OBJECT_ITEM,
  PATTERN_ARRAY,
  PATTERN_ARRAY_ITEM,
  CLASS_METHOD,
  CLASS_PROPERTY,
  CLASS_ACCESSOR,
  CLASS_STATIC_BLOCK,
  IMPORT_DECLARATION,
  IMPORT_SPECIFIER,
  IMPORT_DEFAULT,
  IMPORT_NAMESPACE,
  IMPORT_ATTARTUBE,
  EXPORT_DECLARATION,
  EXPORT_DEFAULT,
  EXPORT_SPECIFIER,
  EXPORT_NAMED,
  EXPORT_NAMESPACE,
  DECLARATION_FUNCTION_ARGUMENT,
  DECLARATION_ARROW_FUNCTION,
  DECLARATION_FUNCTION,
  DECLARATION_FUNCTION_BODY,
  DECLARATION_OBJECT,
  DECLARATION_ARRAY,
  DECLARATION_CLASS,
  DECLARATION_VARIABLE,
};

enum class JS_COMPILE_SCOPE_TYPE { BLOCK, LEX };

struct JSNode;

struct JSLexDeclaration {
  JS_DECLARATION_TYPE type;
  JSNode *declaration;
  std::wstring name;
};

struct JSCompileScope {
  JSAllocator *allocator;
  JS_COMPILE_SCOPE_TYPE type;
  JSCompileScope *parent;
  JSNode *node;
  std::vector<JSLexDeclaration> declarations;
  std::set<std::wstring> refs;
  JSCompileScope(JSAllocator *allocator, const JS_COMPILE_SCOPE_TYPE &type,
                 JSCompileScope *parent, JSNode *node)
      : allocator(allocator), type(type), parent(parent), node(node) {}
};

struct JSNode {
  JSLocation location;
  JS_NODE_TYPE type = JS_NODE_TYPE::ERROR;
  JSNode *parent = nullptr;
  JSCompileScope *scope = nullptr;
  std::vector<JSNode *> children;
  std::vector<JSNode *> comments;
  JSAllocator *allocator;
  JSNode(JSAllocator *allocator) : allocator(allocator) {}
  virtual ~JSNode() {
    for (auto child : children) {
      allocator->dispose(child);
    }
    children.clear();
    for (auto comment : comments) {
      allocator->dispose(comment);
    }
    comments.clear();
    if (scope) {
      allocator->dispose(scope);
      scope = nullptr;
    }
  }
  void addParent(JSNode *parent) {
    parent->children.push_back(this);
    this->parent = parent;
  }
  void removeParent(JSNode *parent) {
    auto it = std::find(parent->children.begin(), parent->children.end(), this);
    if (it != parent->children.end()) {
      parent->children.erase(it);
      this->parent = nullptr;
    }
  }
  template <class T> T *cast() { return dynamic_cast<T *>(this); }

public:
  virtual JSON toJSON(const std::wstring &filename,
                      const std::wstring &source) const {
    std::wstring type;
    switch (this->type) {
    case JS_NODE_TYPE::ERROR:
      type = L"ERROR";
      break;
    case JS_NODE_TYPE::TOKEN:
      type = L"TOKEN";
      break;
    case JS_NODE_TYPE::PRIVATE_NAME:
      type = L"PRIVATE_NAME";
      break;
    case JS_NODE_TYPE::LITERAL_REGEX:
      type = L"LITERAL_REGEX";
      break;
    case JS_NODE_TYPE::LITERAL_NULL:
      type = L"LITERAL_NULL";
      break;
    case JS_NODE_TYPE::LITERAL_STRING:
      type = L"LITERAL_STRING";
      break;
    case JS_NODE_TYPE::LITERAL_BOOLEAN:
      type = L"LITERAL_BOOLEAN";
      break;
    case JS_NODE_TYPE::LITERAL_NUMBER:
      type = L"LITERAL_NUMBER";
      break;
    case JS_NODE_TYPE::LITERAL_COMMENT:
      type = L"LITERAL_COMMENT";
      break;
    case JS_NODE_TYPE::LITERAL_MULTILINE_COMMENT:
      type = L"LITERAL_MULTILINE_COMMENT";
      break;
    case JS_NODE_TYPE::LITERAL_UNDEFINED:
      type = L"LITERAL_UNDEFINED";
      break;
    case JS_NODE_TYPE::LITERAL_IDENTITY:
      type = L"LITERAL_IDENTITY";
      break;
    case JS_NODE_TYPE::LITERAL_TEMPLATE:
      type = L"LITERAL_TEMPLATE";
      break;
    case JS_NODE_TYPE::LITERAL_BIGINT:
      type = L"LITERAL_BIGINT";
      break;
    case JS_NODE_TYPE::LITERAL_THIS:
      type = L"LITERAL_THIS";
      break;
    case JS_NODE_TYPE::LITERAL_SUPER:
      type = L"LITERAL_SUPER";
      break;
    case JS_NODE_TYPE::PROGRAM:
      type = L"PROGRAM";
      break;
    case JS_NODE_TYPE::STATEMENT_EMPTY:
      type = L"STATEMENT_EMPTY";
      break;
    case JS_NODE_TYPE::STATEMENT_BLOCK:
      type = L"STATEMENT_BLOCK";
      break;
    case JS_NODE_TYPE::STATEMENT_DEBUGGER:
      type = L"STATEMENT_DEBUGGER";
      break;
    case JS_NODE_TYPE::STATEMENT_RETURN:
      type = L"STATEMENT_RETURN";
      break;
    case JS_NODE_TYPE::STATEMENT_LABEL:
      type = L"STATEMENT_LABEL";
      break;
    case JS_NODE_TYPE::STATEMENT_BREAK:
      type = L"STATEMENT_BREAK";
      break;
    case JS_NODE_TYPE::STATEMENT_CONTINUE:
      type = L"STATEMENT_CONTINUE";
      break;
    case JS_NODE_TYPE::STATEMENT_IF:
      type = L"STATEMENT_IF";
      break;
    case JS_NODE_TYPE::STATEMENT_SWITCH:
      type = L"STATEMENT_SWITCH";
      break;
    case JS_NODE_TYPE::STATEMENT_SWITCH_CASE:
      type = L"STATEMENT_SWITCH_CASE";
      break;
    case JS_NODE_TYPE::STATEMENT_THROW:
      type = L"STATEMENT_THROW";
      break;
    case JS_NODE_TYPE::STATEMENT_TRY:
      type = L"STATEMENT_TRY";
      break;
    case JS_NODE_TYPE::STATEMENT_TRY_CATCH:
      type = L"STATEMENT_TRY_CATCH";
      break;
    case JS_NODE_TYPE::STATEMENT_WHILE:
      type = L"STATEMENT_WHILE";
      break;
    case JS_NODE_TYPE::STATEMENT_DO_WHILE:
      type = L"STATEMENT_DO_WHILE";
      break;
    case JS_NODE_TYPE::STATEMENT_FOR:
      type = L"STATEMENT_FOR";
      break;
    case JS_NODE_TYPE::STATEMENT_FOR_IN:
      type = L"STATEMENT_FOR_IN";
      break;
    case JS_NODE_TYPE::STATEMENT_FOR_OF:
      type = L"STATEMENT_FOR_OF";
      break;
    case JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF:
      type = L"STATEMENT_FOR_AWAIT_OF";
      break;
    case JS_NODE_TYPE::STATEMENT_EXPRESSION:
      type = L"STATEMENT_EXPRESSION";
      break;
    case JS_NODE_TYPE::DECLARATION_VARIABLE:
      type = L"DECLARATION_VARIABLE";
      break;
    case JS_NODE_TYPE::VARIABLE_DECLARATOR:
      type = L"VARIABLE_DECLARATOR";
      break;
    case JS_NODE_TYPE::DECORATOR:
      type = L"DECORATOR";
      break;
    case JS_NODE_TYPE::DIRECTIVE:
      type = L"DIRECTIVE";
      break;
    case JS_NODE_TYPE::INTERPRETER_DIRECTIVE:
      type = L"INTERPRETER_DIRECTIVE";
      break;
    case JS_NODE_TYPE::OBJECT_PROPERTY:
      type = L"OBJECT_PROPERTY";
      break;
    case JS_NODE_TYPE::OBJECT_METHOD:
      type = L"OBJECT_METHOD";
      break;
    case JS_NODE_TYPE::OBJECT_ACCESSOR:
      type = L"OBJECT_ACCESSOR";
      break;
    case JS_NODE_TYPE::EXPRESSION_BINARY:
      type = L"EXPRESSION_BINARY";
      break;
    case JS_NODE_TYPE::EXPRESSION_MEMBER:
      type = L"EXPRESSION_MEMBER";
      break;
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER:
      type = L"EXPRESSION_OPTIONAL_MEMBER";
      break;
    case JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER:
      type = L"EXPRESSION_COMPUTED_MEMBER";
      break;
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER:
      type = L"EXPRESSION_OPTIONAL_COMPUTED_MEMBER";
      break;
    case JS_NODE_TYPE::EXPRESSION_CONDITION:
      type = L"EXPRESSION_CONDITION";
      break;
    case JS_NODE_TYPE::EXPRESSION_CALL:
      type = L"EXPRESSION_CALL";
      break;
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL:
      type = L"EXPRESSION_OPTIONAL_CALL";
      break;
    case JS_NODE_TYPE::EXPRESSION_NEW:
      type = L"EXPRESSION_NEW";
      break;
    case JS_NODE_TYPE::EXPRESSION_DELETE:
      type = L"EXPRESSION_DELETE";
      break;
    case JS_NODE_TYPE::EXPRESSION_AWAIT:
      type = L"EXPRESSION_AWAIT";
      break;
    case JS_NODE_TYPE::EXPRESSION_YIELD:
      type = L"EXPRESSION_YIELD";
      break;
    case JS_NODE_TYPE::EXPRESSION_YIELD_DELEGATE:
      type = L"EXPRESSION_YIELD_DELEGATE";
      break;
    case JS_NODE_TYPE::EXPRESSION_VOID:
      type = L"EXPRESSION_VOID";
      break;
    case JS_NODE_TYPE::EXPRESSION_TYPEOF:
      type = L"EXPRESSION_TYPEOF";
      break;
    case JS_NODE_TYPE::EXPRESSION_GROUP:
      type = L"EXPRESSION_GROUP";
      break;
    case JS_NODE_TYPE::EXPRESSION_ASSIGMENT:
      type = L"EXPRESSION_ASSIGMENT";
      break;
    case JS_NODE_TYPE::EXPRESSION_SPREAD:
      type = L"EXPRESSION_SPREAD";
      break;
    case JS_NODE_TYPE::PATTERN_SPREAD_ITEM:
      type = L"PATTERN_SPREAD_ITEM";
      break;
    case JS_NODE_TYPE::PATTERN_OBJECT:
      type = L"PATTERN_OBJECT";
      break;
    case JS_NODE_TYPE::PATTERN_OBJECT_ITEM:
      type = L"PATTERN_OBJECT_ITEM";
      break;
    case JS_NODE_TYPE::PATTERN_ARRAY:
      type = L"PATTERN_ARRAY";
      break;
    case JS_NODE_TYPE::PATTERN_ARRAY_ITEM:
      type = L"PATTERN_ARRAY_ITEM";
      break;
    case JS_NODE_TYPE::CLASS_METHOD:
      type = L"CLASS_METHOD";
      break;
    case JS_NODE_TYPE::CLASS_PROPERTY:
      type = L"CLASS_PROPERTY";
      break;
    case JS_NODE_TYPE::CLASS_ACCESSOR:
      type = L"CLASS_ACCESSOR";
      break;
    case JS_NODE_TYPE::CLASS_STATIC_BLOCK:
      type = L"CLASS_STATIC_BLOCK";
      break;
    case JS_NODE_TYPE::IMPORT_DECLARATION:
      type = L"IMPORT_DECLARATION";
      break;
    case JS_NODE_TYPE::IMPORT_SPECIFIER:
      type = L"IMPORT_SPECIFIER";
      break;
    case JS_NODE_TYPE::IMPORT_DEFAULT:
      type = L"IMPORT_DEFAULT";
      break;
    case JS_NODE_TYPE::IMPORT_NAMESPACE:
      type = L"IMPORT_NAMESPACE";
      break;
    case JS_NODE_TYPE::IMPORT_ATTARTUBE:
      type = L"IMPORT_ATTARTUBE";
      break;
    case JS_NODE_TYPE::EXPORT_DECLARATION:
      type = L"EXPORT_DECLARATION";
      break;
    case JS_NODE_TYPE::EXPORT_NAMED:
      type = L"EXPORT_NAMED";
      break;
    case JS_NODE_TYPE::EXPORT_DEFAULT:
      type = L"EXPORT_DEFAULT";
      break;
    case JS_NODE_TYPE::EXPORT_SPECIFIER:
      type = L"EXPORT_SPECIFIER";
      break;
    case JS_NODE_TYPE::EXPORT_NAMESPACE:
      type = L"EXPORT_NAMESPACE";
      break;
    case JS_NODE_TYPE::DECLARATION_FUNCTION_ARGUMENT:
      type = L"DECLARATION_FUNCTION_ARGUMENT";
      break;
    case JS_NODE_TYPE::DECLARATION_ARROW_FUNCTION:
      type = L"DECLARATION_ARROW_FUNCTION";
      break;
    case JS_NODE_TYPE::DECLARATION_FUNCTION:
      type = L"DECLARATION_FUNCTION";
      break;
    case JS_NODE_TYPE::DECLARATION_FUNCTION_BODY:
      type = L"DECLARATION_FUNCTION_BODY";
      break;
    case JS_NODE_TYPE::DECLARATION_OBJECT:
      type = L"DECLARATION_OBJECT";
      break;
    case JS_NODE_TYPE::DECLARATION_ARRAY:
      type = L"DECLARATION_ARRAY";
      break;
    case JS_NODE_TYPE::DECLARATION_CLASS:
      type = L"DECLARATION_CLASS";
      break;
    }
    return JSON{}
        .setObject()
        .setField(L"type", JSON{}.setString(type))
        .setField(L"source", JSON{}.setString(location.get(source)));
  }
};

struct JSTokenNode : public JSNode {
  JSTokenNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::TOKEN;
  }
};

struct JSErrorNode : public JSNode {
  std::wstring message;
  JSErrorNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::ERROR;
  }
};

struct JSStringLiteralNode : public JSNode {
  JSStringLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_STRING;
  }
};

struct JSNumberLiteralNode : public JSNode {
  JSNumberLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_NUMBER;
  }
};

struct JSBigIntLiteralNode : public JSNode {
  JSBigIntLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_BIGINT;
  }
};

struct JSRegexpLiteralNode : public JSNode {
  JSRegexpLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_REGEX;
  }
};

struct JSIdentityLiteralNode : public JSNode {
  JSIdentityLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_IDENTITY;
  }
};

struct JSPrivateNameNode : public JSNode {
  JSPrivateNameNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PRIVATE_NAME;
  }
};

struct JSNullLiteralNode : public JSNode {
  JSNullLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_NULL;
  }
};

struct JSUndefinedLiteralNode : public JSNode {
  JSUndefinedLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_UNDEFINED;
  }
};

struct JSBooleanLiteralNode : public JSNode {
  JSBooleanLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_BOOLEAN;
  }
};

struct JSTemplateLiteralNode : public JSNode {
  JSNode *tag{};
  std::vector<JSNode *> quasis;
  std::vector<JSNode *> expressions;
  JSTemplateLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_TEMPLATE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (tag) {
      json.setField(L"tag", tag->toJSON(filename, source));
    }
    auto &jsonQuasis =
        json.setField(L"quasis", JSON{}.setArray()).getField(L"quasis");
    for (size_t index = 0; index < quasis.size(); index++) {
      jsonQuasis.setIndex(index, quasis[index]->toJSON(filename, source));
    }
    auto &jsonExpressions = json.setField(L"expressions", JSON{}.setArray())
                                .getField(L"expressions");
    for (size_t index = 0; index < expressions.size(); index++) {
      jsonExpressions.setIndex(index,
                               expressions[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSThisLiteralNode : public JSNode {
  JSThisLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_THIS;
  }
};

struct JSSuperLiteralNode : public JSNode {
  JSSuperLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_SUPER;
  }
};

struct JSCommentLiteralNode : public JSNode {
  JSCommentLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_COMMENT;
  }
};

struct JSMultilineCommentLiteralNode : public JSNode {
  JSMultilineCommentLiteralNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::LITERAL_MULTILINE_COMMENT;
  }
};

struct JSFunctionBaseNode : public JSNode {
  JSNode *body{};
  bool async{};
  bool generator{};
  std::vector<JSNode *> arguments;
  std::set<std::wstring> closure;
  JSFunctionBaseNode(JSAllocator *allocator) : JSNode(allocator) {}
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"body", body->toJSON(filename, source));
    auto &jsonArguments =
        json.setField(L"arguments", JSON{}.setArray()).getField(L"arguments");
    for (size_t index = 0; index < arguments.size(); index++) {
      jsonArguments.setIndex(index, arguments[index]->toJSON(filename, source));
    }
    auto &jsonClosure =
        json.setField(L"closure", JSON{}.setArray()).getField(L"closure");
    std::vector arr(closure.begin(), closure.end());
    for (size_t index = 0; index < arr.size(); index++) {
      jsonClosure.setIndex(index, JSON{}.setString(arr[index]));
    }
    return json;
  }
};

struct JSProgramNode : public JSNode {
  JSNode *interpreter{};
  std::vector<JSNode *> directives;
  std::vector<JSNode *> statements;
  JSProgramNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PROGRAM;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (interpreter) {
      json.setField(L"interpreter", interpreter->toJSON(filename, source));
    }
    auto &jsonDirectives =
        json.setField(L"directives", JSON{}.setArray()).getField(L"directives");
    for (size_t index = 0; index < directives.size(); index++) {
      jsonDirectives.setIndex(index,
                              directives[index]->toJSON(filename, source));
    }
    auto &jsonStatements =
        json.setField(L"statements", JSON{}.setArray()).getField(L"statements");
    for (size_t index = 0; index < statements.size(); index++) {
      jsonStatements.setIndex(index,
                              statements[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSInterpreterDirectiveNode : public JSNode {
  JSInterpreterDirectiveNode(JSAllocator *allocator) : JSNode(allocator) {
    this->type = JS_NODE_TYPE::INTERPRETER_DIRECTIVE;
  }
};

struct JSDirectiveNode : public JSNode {
  JSDirectiveNode(JSAllocator *allocator) : JSNode(allocator) {
    this->type = JS_NODE_TYPE::DIRECTIVE;
  }
};

struct JSEmptyStatementNode : public JSNode {
  JSEmptyStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_EMPTY;
  }
};

struct JSBlockStatementNode : public JSNode {
  std::vector<JSNode *> statements;
  JSBlockStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_BLOCK;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonStatements =
        json.setField(L"statements", JSON{}.setArray()).getField(L"statements");
    for (size_t index = 0; index < statements.size(); index++) {
      jsonStatements.setIndex(index,
                              statements[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSDebuggerStatementNode : public JSNode {
  JSDebuggerStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_DEBUGGER;
  }
};

struct JSReturnStatementNode : public JSNode {
  JSNode *value{};
  JSReturnStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_RETURN;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};

struct JSLabelStatementNode : public JSNode {
  JSNode *label{};
  JSNode *statement{};
  JSLabelStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_LABEL;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"label", label->toJSON(filename, source));
    json.setField(L"statement", statement->toJSON(filename, source));
    return json;
  }
};

struct JSBreakStatementNode : public JSNode {
  JSNode *label{};
  JSBreakStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_BREAK;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (label) {
      json.setField(L"label", label->toJSON(filename, source));
    }
    return json;
  }
};

struct JSContinueStatementNode : public JSNode {
  JSNode *label{};
  JSContinueStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_CONTINUE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (label) {
      json.setField(L"label", label->toJSON(filename, source));
    }
    return json;
  }
};

struct JSIfStatementNode : public JSNode {
  JSNode *condition{};
  JSNode *alternate{};
  JSNode *consequent{};
  JSIfStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_IF;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    if (alternate) {
      json.setField(L"alternate", alternate->toJSON(filename, source));
    }
    if (consequent) {
      json.setField(L"consequent", consequent->toJSON(filename, source));
    }
    return json;
  }
};

struct JSSwitchStatementNode : public JSNode {
  JSNode *condition{};
  std::vector<JSNode *> cases;
  JSSwitchStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_SWITCH;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    auto &jsonCases =
        json.setField(L"cases", JSON{}.setArray()).getField(L"cases");
    for (size_t index = 0; index < cases.size(); index++) {
      jsonCases.setIndex(index, cases[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSSwitchCaseStatementNode : public JSNode {
  JSNode *match{};
  std::vector<JSNode *> statements;
  JSSwitchCaseStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_SWITCH_CASE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (match) {
      json.setField(L"match", match->toJSON(filename, source));
    }
    auto &jsonStatements =
        json.setField(L"statements", JSON{}.setArray()).getField(L"statements");
    for (size_t index = 0; index < statements.size(); index++) {
      jsonStatements.setIndex(index,
                              statements[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSThrowStatementNode : public JSNode {
  JSNode *value{};
  JSThrowStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_THROW;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};

struct JSTryStatementNode : public JSNode {
  JSNode *body{};
  JSNode *onerror{};
  JSNode *onfinish{};
  JSTryStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_TRY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    if (onerror) {
      json.setField(L"onerror", onerror->toJSON(filename, source));
    }
    if (onfinish) {
      json.setField(L"onfinish", onfinish->toJSON(filename, source));
    }
    return json;
  }
};

struct JSTryCatchStatementNode : public JSNode {
  JSNode *identifier{};
  JSNode *body{};
  JSTryCatchStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_TRY_CATCH;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (identifier) {
      json.setField(L"identifier", identifier->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSWhileStatementNode : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSWhileStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_WHILE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSDoWhileStatementNode : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSDoWhileStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_DO_WHILE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSForStatementNode : public JSNode {
  JSNode *init{};
  JSNode *condition{};
  JSNode *after{};
  JSNode *body{};
  JSForStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_FOR;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (init) {
      json.setField(L"init", init->toJSON(filename, source));
    }
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    if (after) {
      json.setField(L"after", after->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSForInStatementNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForInStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_FOR_IN;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (left) {
      json.setField(L"left", left->toJSON(filename, source));
    }
    if (right) {
      json.setField(L"right", right->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSForOfStatementNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForOfStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_FOR_OF;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (left) {
      json.setField(L"left", left->toJSON(filename, source));
    }
    if (right) {
      json.setField(L"right", right->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSForAwaitOfStatementNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForAwaitOfStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (left) {
      json.setField(L"left", left->toJSON(filename, source));
    }
    if (right) {
      json.setField(L"right", right->toJSON(filename, source));
    }
    if (body) {
      json.setField(L"body", body->toJSON(filename, source));
    }
    return json;
  }
};

struct JSExpressionStatementNode : public JSNode {
  JSNode *expression{};
  JSExpressionStatementNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::STATEMENT_EXPRESSION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (expression) {
      json.setField(L"expression", expression->toJSON(filename, source));
    }
    return json;
  }
};

struct JSBinaryExpressionNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *opt{};
  JSBinaryExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_BINARY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (left) {
      json.setField(L"left", left->toJSON(filename, source));
    }
    if (opt) {
      json.setField(L"opt", opt->toJSON(filename, source));
    }
    if (right) {
      json.setField(L"right", right->toJSON(filename, source));
    }
    return json;
  }
};

struct JSAssigmentExpressionNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *opt{};
  JSAssigmentExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_ASSIGMENT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (left) {
      json.setField(L"left", left->toJSON(filename, source));
    }
    if (opt) {
      json.setField(L"opt", opt->toJSON(filename, source));
    }
    if (right) {
      json.setField(L"right", right->toJSON(filename, source));
    }
    return json;
  }
};

struct JSGroupExpressionNode : public JSNode {
  JSNode *expression{};
  JSGroupExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_GROUP;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (expression) {
      json.setField(L"expression", expression->toJSON(filename, source));
    }
    return json;
  }
};

struct JSMemberExpressionNode : public JSNode {
  JSNode *host{};
  JSNode *field{};
  JSMemberExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_MEMBER;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (host) {
      json.setField(L"host", host->toJSON(filename, source));
    }
    if (field) {
      json.setField(L"field", field->toJSON(filename, source));
    }
    return json;
  }
};

struct JSComputedMemberExpressionNode : public JSMemberExpressionNode {
  JSComputedMemberExpressionNode(JSAllocator *allocator)
      : JSMemberExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER;
  }
};

struct JSOptionalMemberExpressionNode : public JSMemberExpressionNode {
  JSOptionalMemberExpressionNode(JSAllocator *allocator)
      : JSMemberExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER;
  }
};

struct JSOptionalComputedMemberExpressionNode : public JSMemberExpressionNode {
  JSOptionalComputedMemberExpressionNode(JSAllocator *allocator)
      : JSMemberExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER;
  }
};

struct JSCallExpressionNode : public JSNode {
  JSNode *callee{};
  std::vector<JSNode *> arguments;
  JSCallExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_CALL;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (callee) {
      json.setField(L"callee", callee->toJSON(filename, source));
    }
    auto &jsonArgument =
        json.setField(L"arguments", JSON{}.setArray()).getField(L"arguments");
    for (size_t index = 0; index < arguments.size(); index++) {
      jsonArgument.setIndex(index, arguments[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSOptionalCallExpressionNode : public JSCallExpressionNode {
  JSOptionalCallExpressionNode(JSAllocator *allocator)
      : JSCallExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL;
  }
};

struct JSNewExpressionNode : public JSCallExpressionNode {
  JSNewExpressionNode(JSAllocator *allocator)
      : JSCallExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_NEW;
  }
};

struct JSAwaitExpressionNode : public JSNode {
  JSNode *value{};
  JSAwaitExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_AWAIT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSYieldExpressionNode : public JSNode {
  JSNode *value{};
  JSYieldExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_YIELD;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSYieldDelegateExpressionNode : public JSYieldExpressionNode {
  JSYieldDelegateExpressionNode(JSAllocator *allocator)
      : JSYieldExpressionNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_YIELD_DELEGATE;
  }
};

struct JSDeleteExpressionNode : public JSNode {
  JSNode *value{};
  JSDeleteExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_DELETE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSVoidExpressionNode : public JSNode {
  JSNode *value{};
  JSVoidExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_VOID;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSTypeofExpressioNode : public JSNode {
  JSNode *value{};
  JSTypeofExpressioNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_TYPEOF;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSConditionExpressionNode : public JSNode {
  JSNode *condition{};
  JSNode *alternate{};
  JSNode *consequent{};
  JSConditionExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_CONDITION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (condition) {
      json.setField(L"condition", condition->toJSON(filename, source));
    }
    if (alternate) {
      json.setField(L"alternate", alternate->toJSON(filename, source));
    }
    if (consequent) {
      json.setField(L"consequent", consequent->toJSON(filename, source));
    }
    return json;
  }
};

struct JSSpreadExpressionNode : public JSNode {
  JSNode *value{};
  JSSpreadExpressionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPRESSION_SPREAD;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSObjectPropertyNode : public JSNode {
  JSNode *key{};
  JSNode *value{};
  bool computed{};
  JSObjectPropertyNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::OBJECT_PROPERTY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"key", key->toJSON(filename, source));
    json.setField(L"value", value->toJSON(filename, source));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    return json;
  }
};

struct JSObjectMethodNode : public JSFunctionBaseNode {
  JSNode *key{};
  bool computed{};
  JSObjectMethodNode(JSAllocator *allocator) : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::OBJECT_METHOD;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"key", key->toJSON(filename, source));
    json.setField(L"async", JSON{}.setBoolean(async));
    json.setField(L"generator", JSON{}.setBoolean(generator));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    return json;
  }
};

struct JSObjectAccessorNode : public JSFunctionBaseNode {
  JSNode *key{};
  bool computed{};
  JS_ACCESSOR_TYPE kind;
  JSObjectAccessorNode(JSAllocator *allocator) : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::OBJECT_ACCESSOR;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"key", key->toJSON(filename, source));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    json.setField(
        L"kind",
        JSON{}.setString(kind == JS_ACCESSOR_TYPE::GET ? L"get" : L"set"));
    return json;
  }
};

struct JSObjectDeclarationNode : public JSNode {
  std::vector<JSNode *> properties;
  JSObjectDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_OBJECT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonProps =
        json.setField(L"properties", JSON{}.setArray()).getField(L"properties");
    for (size_t index = 0; index < properties.size(); index++) {
      jsonProps.setIndex(index, properties[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSArrayDeclarationNode : public JSNode {
  std::vector<JSNode *> items;
  JSArrayDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_ARRAY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonItems =
        json.setField(L"items", JSON{}.setArray()).getField(L"items");
    for (size_t index = 0; index < items.size(); index++) {
      jsonItems.setIndex(index, items[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSFunctionArgumentDeclarationNode : public JSNode {
  JSNode *identifier{};
  JSNode *value{};
  JSFunctionArgumentDeclarationNode(JSAllocator *allocator)
      : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_FUNCTION_ARGUMENT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};

struct JSFunctionBodyDeclarationNode : public JSNode {
  std::vector<JSNode *> directives;
  std::vector<JSNode *> statements;
  JSFunctionBodyDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_FUNCTION_BODY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonDirectives =
        json.setField(L"directives", JSON{}.setArray()).getField(L"directives");
    for (size_t index = 0; index < directives.size(); index++) {
      jsonDirectives.setIndex(index,
                              directives[index]->toJSON(filename, source));
    }
    auto &jsonStatements =
        json.setField(L"statements", JSON{}.setArray()).getField(L"statements");
    for (size_t index = 0; index < statements.size(); index++) {
      jsonStatements.setIndex(index,
                              statements[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSFunctionDeclarationNode : public JSFunctionBaseNode {
  JSNode *identifier{};
  JSFunctionDeclarationNode(JSAllocator *allocator)
      : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_FUNCTION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"async", JSON{}.setBoolean(async));
    json.setField(L"generator", JSON{}.setBoolean(generator));
    return json;
  }
};

struct JSArrowDeclarationNode : public JSFunctionBaseNode {
  JSArrowDeclarationNode(JSAllocator *allocator)
      : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_ARROW_FUNCTION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"async", JSON{}.setBoolean(async));
    return json;
  }
};

struct JSObjectPatternItemNode : public JSNode {
  JSNode *key{};
  JSNode *alias{};
  JSNode *value{};
  bool computed{};
  JSObjectPatternItemNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PATTERN_OBJECT_ITEM;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (key) {
      json.setField(L"key", key->toJSON(filename, source));
    }
    if (alias) {
      json.setField(L"alias", alias->toJSON(filename, source));
    }
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    json.setField(L"computed", JSON{}.setBoolean(computed));
    return json;
  }
};

struct JSObjectPatternNode : public JSNode {
  std::vector<JSNode *> items;
  JSObjectPatternNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PATTERN_OBJECT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonItems =
        json.setField(L"items", JSON{}.setArray()).getField(L"items");
    for (size_t index = 0; index < items.size(); index++) {
      jsonItems.setIndex(index, items[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSArrayPatternItemNode : public JSNode {
  JSNode *alias{};
  JSNode *value{};
  JSArrayPatternItemNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PATTERN_ARRAY_ITEM;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (alias) {
      json.setField(L"alias", alias->toJSON(filename, source));
    }
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};

struct JSArrayPatternNode : public JSNode {
  std::vector<JSNode *> items;
  JSArrayPatternNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PATTERN_ARRAY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    auto &jsonItems =
        json.setField(L"items", JSON{}.setArray()).getField(L"items");
    for (size_t index = 0; index < items.size(); index++) {
      jsonItems.setIndex(index, items[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSSpreadPatternItemNode : public JSNode {
  JSNode *value{};
  JSSpreadPatternItemNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::PATTERN_SPREAD_ITEM;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSVariableDeclaraionNode : public JSNode {
  JS_DECLARATION_TYPE kind = JS_DECLARATION_TYPE::CONST;
  std::vector<JSNode *> declarations{};
  JSVariableDeclaraionNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_VARIABLE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    switch (kind) {
    case JS_DECLARATION_TYPE::VAR:
      json.setField(L"kind", JSON{}.setString(L"var"));
      break;
    case JS_DECLARATION_TYPE::CONST:
      json.setField(L"kind", JSON{}.setString(L"const"));
      break;
    case JS_DECLARATION_TYPE::LET:
      json.setField(L"kind", JSON{}.setString(L"let"));
      break;
    case JS_DECLARATION_TYPE::FUNCTION:
      json.setField(L"kind", JSON{}.setString(L"function"));
      break;
    }
    auto &jsonDeclarations = json.setField(L"declarations", JSON{}.setArray())
                                 .getField(L"declarations");
    for (size_t index = 0; index < declarations.size(); index++) {
      jsonDeclarations.setIndex(index,
                                declarations[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSVariableDeclaratorNode : public JSNode {
  JSNode *identifier{};
  JSNode *initialize{};
  JSVariableDeclaratorNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::VARIABLE_DECLARATOR;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    if (initialize) {
      json.setField(L"initialize", initialize->toJSON(filename, source));
    }
    return json;
  }
};

struct JSImportDeclarationNode : public JSNode {
  std::vector<JSNode *> specifiers;
  JSNode *source{};
  std::vector<JSNode *> attributes;
  JSImportDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::IMPORT_DECLARATION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"source", this->source->toJSON(filename, source));
    auto &jsonSpecifiers =
        json.setField(L"specifiers", JSON{}.setArray()).getField(L"specifiers");
    for (size_t index = 0; index < specifiers.size(); index++) {
      jsonSpecifiers.setIndex(index,
                              specifiers[index]->toJSON(filename, source));
    }
    auto &jsonAttributes =
        json.setField(L"attributes", JSON{}.setArray()).getField(L"attributes");
    for (size_t index = 0; index < attributes.size(); index++) {
      jsonAttributes.setIndex(index,
                              attributes[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSImportSpecifierNode : public JSNode {
  JSNode *identifier{};
  JSNode *alias{};
  JSImportSpecifierNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::IMPORT_SPECIFIER;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    if (alias) {
      json.setField(L"alias", alias->toJSON(filename, source));
    }
    return json;
  }
};

struct JSImportDefaultNode : public JSNode {
  JSNode *identifier{};
  JSImportDefaultNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::IMPORT_DEFAULT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    return json;
  }
};

struct JSImportNamespaceNode : public JSNode {
  JSNode *alias{};
  JSImportNamespaceNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::IMPORT_NAMESPACE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"alias", alias->toJSON(filename, source));
    return json;
  }
};

struct JSImportAttributeNode : public JSNode {
  JSNode *key{};
  JSNode *value{};
  JSImportAttributeNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::IMPORT_ATTARTUBE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"key", key->toJSON(filename, source));
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSExportDeclarationNode : public JSNode {
  std::vector<JSNode *> specifiers;
  JSNode *source{};
  std::vector<JSNode *> attributes;
  JSExportDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPORT_DECLARATION;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"source", this->source->toJSON(filename, source));
    auto &jsonSpecifiers =
        json.setField(L"specifiers", JSON{}.setArray()).getField(L"specifiers");
    for (size_t index = 0; index < specifiers.size(); index++) {
      jsonSpecifiers.setIndex(index,
                              specifiers[index]->toJSON(filename, source));
    }
    auto &jsonAttributes =
        json.setField(L"attributes", JSON{}.setArray()).getField(L"attributes");
    for (size_t index = 0; index < attributes.size(); index++) {
      jsonAttributes.setIndex(index,
                              attributes[index]->toJSON(filename, source));
    }
    return json;
  }
};

struct JSExportDefaultNode : public JSNode {
  JSNode *expression{};
  JSExportDefaultNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPORT_DEFAULT;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"expression", expression->toJSON(filename, source));
    return json;
  }
};

struct JSExportSpecifierNode : public JSNode {
  JSNode *identifier{};
  JSNode *alias{};
  JSExportSpecifierNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPORT_SPECIFIER;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"alias", alias->toJSON(filename, source));
    return json;
  }
};

struct JSExportNamedNode : public JSNode {
  JSNode *declaration{};
  JSExportNamedNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPORT_NAMED;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"declaration", declaration->toJSON(filename, source));
    return json;
  }
};

struct JSExportNamespaceNode : public JSNode {
  JSNode *alias{};
  JSExportNamespaceNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::EXPORT_NAMESPACE;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"alias", alias->toJSON(filename, source));
    return json;
  }
};

struct JSClassMethodNode : public JSFunctionBaseNode {
  bool static_{};
  bool computed{};
  JSNode *identifier{};
  JSClassMethodNode(JSAllocator *allocator) : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::CLASS_METHOD;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"async", JSON{}.setBoolean(async));
    json.setField(L"generator", JSON{}.setBoolean(generator));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    json.setField(L"static", JSON{}.setBoolean(static_));
    return json;
  }
};

struct JSClassPropertyNode : public JSNode {
  bool static_{};
  bool computed{};
  JSNode *identifier{};
  JSNode *value{};
  JSClassPropertyNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::CLASS_PROPERTY;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"value", value->toJSON(filename, source));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    json.setField(L"static", JSON{}.setBoolean(static_));
    return json;
  }
};

struct JSClassAccessorNode : public JSFunctionBaseNode {
  JS_ACCESSOR_TYPE kind = JS_ACCESSOR_TYPE::GET;
  bool computed{};
  bool static_{};
  JSNode *identifier{};
  JSClassAccessorNode(JSAllocator *allocator) : JSFunctionBaseNode(allocator) {
    type = JS_NODE_TYPE::CLASS_ACCESSOR;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSFunctionBaseNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"computed", JSON{}.setBoolean(computed));
    json.setField(L"static", JSON{}.setBoolean(static_));
    json.setField(
        L"kind",
        JSON{}.setString(kind == JS_ACCESSOR_TYPE::GET ? L"get" : L"set"));
    return json;
  }
};

struct JSStaticBlockNode : public JSNode {
  JSNode *statement{};
  JSStaticBlockNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::CLASS_STATIC_BLOCK;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"statement", statement->toJSON(filename, source));
    return json;
  }
};

struct JSClassDeclarationNode : public JSNode {
  JSNode *extends{};
  JSNode *identifier{};
  std::set<std::wstring> closure{};
  std::vector<JSNode *> properties;
  JSClassDeclarationNode(JSAllocator *allocator) : JSNode(allocator) {
    type = JS_NODE_TYPE::DECLARATION_CLASS;
  }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    json.setField(L"extends", extends->toJSON(filename, source));
    auto &jsonProp =
        json.setField(L"properties", JSON{}.setArray()).getField(L"properties");
    for (size_t index = 0; index < properties.size(); index++) {
      jsonProp.setIndex(index, properties[index]->toJSON(filename, source));
    }
    return json;
  }
};

class JSParser {
private:
  JSCompileScope *_scope = nullptr;
  JSAllocator *_allocator{};
  JS_EVAL_TYPE _type{JS_EVAL_TYPE::PROGRAM};

private:
  JSCompileScope *pushScope(const JS_COMPILE_SCOPE_TYPE &type, JSNode *node) {
    auto scope = _allocator->create<JSCompileScope>(type, _scope, node);
    _scope = scope;
    return scope;
  }

  void popScope() { _scope = _scope->parent; }

  JSNode *resolveDeclarator(const JS_DECLARATION_TYPE &type,
                            const std::wstring &source, JSNode *identifier,
                            JSNode *declaration);

  JSNode *declarVariable(const JS_DECLARATION_TYPE &type,
                         const std::wstring &source, JSNode *declaration);

  void resolveBinding(const std::wstring &source, JSNode *node);

private:
  JSNode *createError(const std::wstring &message, const std::wstring &source,
                      const JSPosition &end) {

    auto *node = _allocator->create<JSErrorNode>();
    node->message = message;
    node->location = getLocation(source, {}, end);
    return node;
  }

private:
  bool isLineTerminator(wchar_t c) {
    if (c == 0xa) {
      return true;
    }
    if (c == 0xd) {
      return true;
    }
    if (c == 0x2028) {
      return true;
    }
    if (c == 0x2029) {
      return true;
    }
    return false;
  }

  bool isWhiteSpace(wchar_t c) {

    if (c == 0x9) {
      return true;
    }
    if (c == 0xb) {
      return true;
    }
    if (c == 0xc) {
      return true;
    }
    if (c == 0x20) {
      return true;
    }
    if (c == 0xa0) {
      return true;
    }
    if (c == 0xfeff) {
      return true;
    }
    return false;
  }

  bool isKeyword(const std::wstring &source, const JSLocation &location) {
    static std::array keywords = {
        L"break",      L"case",     L"catch",   L"class",  L"const",
        L"continue",   L"debugger", L"default", L"delete", L"do",
        L"else",       L"export",   L"extends", L"false",  L"finally",
        L"for",        L"function", L"if",      L"import", L"in",
        L"instanceof", L"new",      L"null",    L"return", L"super",
        L"switch",     L"this",     L"throw",   L"true",   L"try",
        L"typeof",     L"var",      L"void",    L"while",  L"with",
        L"let",        L"static"};
    for (auto &keyword : keywords) {
      if (location.is(source, keyword)) {
        return true;
      }
    }
    return false;
  }

private:
  JSNode *findOptional(JSNode *node) {
    if (node->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL ||
        node->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER ||
        node->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER) {
      return node;
    }
    if (node->type == JS_NODE_TYPE::EXPRESSION_MEMBER) {
      return findOptional(dynamic_cast<JSMemberExpressionNode *>(node)->host);
    }
    if (node->type == JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) {
      return findOptional(
          dynamic_cast<JSComputedMemberExpressionNode *>(node)->host);
    }
    return nullptr;
  }

  JSNode *checkAssigment(JSNode *node, const std::wstring &source) {
    if (node->type == JS_NODE_TYPE::EXPRESSION_GROUP) {
      return checkAssigment(
          dynamic_cast<JSGroupExpressionNode *>(node)->expression, source);
    }
    if (node->type == JS_NODE_TYPE::LITERAL_IDENTITY ||
        node->type == JS_NODE_TYPE::PATTERN_ARRAY ||
        node->type == JS_NODE_TYPE::PATTERN_OBJECT) {
      return nullptr;
    }
    auto opt = findOptional(node);
    if (opt) {
      if (opt->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL) {
        return createError(L"Invalid left-hand side in assignment", source,
                           dynamic_cast<JSOptionalCallExpressionNode *>(opt)
                               ->callee->location.end);
      }
      if (opt->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER) {
        return createError(L"Invalid left-hand side in assignment", source,
                           dynamic_cast<JSOptionalMemberExpressionNode *>(opt)
                               ->host->location.end);
      }
      if (opt->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER) {
        return createError(
            L"Invalid left-hand side in assignment", source,
            dynamic_cast<JSOptionalComputedMemberExpressionNode *>(opt)
                ->host->location.end);
      }
    }
    if (node->type == JS_NODE_TYPE::EXPRESSION_MEMBER ||
        node->type == JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) {
      return nullptr;
    }
    return createError(L"Invalid left-hand side in assignment", source,
                       node->location.start);
  }

  JSLocation getLocation(const std::wstring &source, const JSPosition &start,
                         const JSPosition &end) {
    JSLocation loc;
    loc.start = {
        .funcname = L"neo.compile", .line = 1, .column = 1, .offset = 0};
    while (loc.start.offset < start.offset) {
      if (isLineTerminator(source[loc.start.offset])) {
        if (source[loc.start.offset] != '\r') {
          loc.start.column = 1;
          loc.start.line++;
        }
      } else {
        loc.start.column++;
      }
      loc.start.offset++;
    }

    loc.end = {.funcname = L"neo.compile", .line = 1, .column = 1, .offset = 0};
    while (loc.end.offset < end.offset) {
      if (isLineTerminator(source[loc.end.offset])) {
        if (source[loc.end.offset] != '\r') {
          loc.end.column = 1;
          loc.end.line++;
        }
      } else {
        loc.end.column++;
      }
      loc.end.offset++;
    }
    return loc;
  }

  bool skipWhiteSpace(const std::wstring &source, JSPosition &position) {
    if (isWhiteSpace(source[position.offset])) {
      position.offset++;
      return true;
    }
    return false;
  }

  bool skipLineTerminator(const std::wstring &source, JSPosition &position) {
    if (isLineTerminator(source[position.offset])) {
      position.offset++;
      return true;
    }
    return false;
  }

  bool skipInvisible(const std::wstring &source, JSPosition &position) {
    return skipWhiteSpace(source, position) ||
           skipLineTerminator(source, position);
  }

private:
  bool checkIdentifier(const std::vector<std::wstring> &identifiers,
                       const std::wstring &source, JSPosition &position,
                       JSNode **ptoken = nullptr) {
    auto current = position;
    auto token = readIdentifyLiteral(source, current);
    if (!token) {
      return false;
    }
    for (auto &identifier : identifiers) {
      if (token->location.is(source, identifier)) {
        position = current;
        if (ptoken) {
          *ptoken = token;
        } else {
          _allocator->dispose(token);
        }
        return true;
      }
    }
    _allocator->dispose(token);
    return false;
  }
  
  bool checkSymbol(const std::vector<std::wstring> &symbols,
                   const std::wstring &source, JSPosition &position,
                   JSNode **ptoken = nullptr) {
    auto current = position;
    auto token = readSymbolToken(source, current);
    if (!token) {
      return false;
    }
    for (auto &symbol : symbols) {
      if (token->location.is(source, symbol)) {
        position = current;
        if (ptoken) {
          *ptoken = token;
        } else {
          _allocator->dispose(token);
        }
        return true;
      }
    }
    _allocator->dispose(token);
    return false;
  }

  JSNode *readSymbolToken(const std::wstring &source, JSPosition &position);

  JSNode *readComments(const std::wstring &source, JSPosition &position,
                       std::vector<JSNode *> &comments);

private:
  JSNode *readTemplateLiteral(const std::wstring &source, JSPosition &position,
                              JSNode *tag = nullptr);

  JSNode *readPrivateName(const std::wstring &source, JSPosition &position);

  JSNode *readIdentifyLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readStringLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readNullLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readUndefinedLiteral(const std::wstring &source,
                               JSPosition &position);

  JSNode *readThisLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readSuperLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readBooleanLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readNumberLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readRegexpLiteral(const std::wstring &source, JSPosition &position);

  JSNode *readProgram(const std::wstring &source, JSPosition &position);

  JSNode *readInterpreterDirective(const std::wstring &source,
                                   JSPosition &position);

  JSNode *readDirective(const std::wstring &source, JSPosition &position);

  JSNode *readEmptyStatement(const std::wstring &source, JSPosition &position);

  JSNode *readBlockStatement(const std::wstring &source, JSPosition &position);

  JSNode *readDebuggerStatement(const std::wstring &source,
                                JSPosition &position);

  JSNode *readReturnStatement(const std::wstring &source, JSPosition &position);

  JSNode *readLabelStatement(const std::wstring &source, JSPosition &position);

  JSNode *readBreakStatement(const std::wstring &source, JSPosition &position);

  JSNode *readContinueStatement(const std::wstring &source,
                                JSPosition &position);

  JSNode *readIfStatement(const std::wstring &source, JSPosition &position);

  JSNode *readSwitchStatement(const std::wstring &source, JSPosition &position);

  JSNode *readSwitchCaseStatement(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readThrowStatement(const std::wstring &source, JSPosition &position);

  JSNode *readTryStatement(const std::wstring &source, JSPosition &position);

  JSNode *readTryCatchStatement(const std::wstring &source,
                                JSPosition &position);

  JSNode *readWhileStatement(const std::wstring &source, JSPosition &position);

  JSNode *readDoWhileStatement(const std::wstring &source,
                               JSPosition &position);

  JSNode *readForStatement(const std::wstring &source, JSPosition &position);

  JSNode *readForInStatement(const std::wstring &source, JSPosition &position);

  JSNode *readForOfStatement(const std::wstring &source, JSPosition &position);

  JSNode *readForAwaitOfStatement(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readVariableDeclarationStatement(const std::wstring &source,
                                           JSPosition &position);

  JSNode *readVariableDeclaration(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readVariableDeclarator(const std::wstring &source,
                                 JSPosition &position);

  JSNode *readImportNamespace(const std::wstring &source, JSPosition &position);

  JSNode *readImportDefault(const std::wstring &source, JSPosition &position);

  JSNode *readImportSpecifier(const std::wstring &source, JSPosition &position);

  JSNode *readImportAttribute(const std::wstring &source, JSPosition &position);

  JSNode *readImportDeclaration(const std::wstring &source,
                                JSPosition &position);

  JSNode *readExportSpecifier(const std::wstring &source, JSPosition &position);

  JSNode *readExportNamespaceSpecifier(const std::wstring &source,
                                       JSPosition &position);

  JSNode *readExportDefaultSpecifier(const std::wstring &source,
                                     JSPosition &position);

  JSNode *readExportDeclaration(const std::wstring &source,
                                JSPosition &position);

  JSNode *readStatement(const std::wstring &source, JSPosition &position);

  JSNode *readExpressionStatement(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readObjectProperty(const std::wstring &source, JSPosition &position);

  JSNode *readObjectMethod(const std::wstring &source, JSPosition &position);

  JSNode *readObjectAccessor(const std::wstring &source, JSPosition &position);

  JSNode *readObjectDeclaration(const std::wstring &source,
                                JSPosition &position);

  JSNode *readClassProperty(const std::wstring &source, JSPosition &position);

  JSNode *readClassMethod(const std::wstring &source, JSPosition &position);

  JSNode *readClassAccessor(const std::wstring &source, JSPosition &position);

  JSNode *readStaticBlock(const std::wstring &source, JSPosition &position);

  JSNode *readClassDeclaration(const std::wstring &source,
                               JSPosition &position);

  JSNode *readArrayDeclaration(const std::wstring &source,
                               JSPosition &position);

  JSNode *readSpreadPattern(const std::wstring &source, JSPosition &position);

  JSNode *readObjectPatternItem(const std::wstring &source,
                                JSPosition &position);

  JSNode *readObjectPattern(const std::wstring &source, JSPosition &position);

  JSNode *readArrayPatternItem(const std::wstring &source,
                               JSPosition &position);

  JSNode *readArrayPattern(const std::wstring &source, JSPosition &position);

  JSNode *readPattern(const std::wstring &source, JSPosition &position);

  JSNode *readPartPattern(const std::wstring &source, JSPosition &position);

  JSNode *readFunctionArgument(const std::wstring &source,
                               JSPosition &position);

  JSNode *readFunctionBodyDeclaration(const std::wstring &source,
                                      JSPosition &position);

  JSNode *readFunctionDeclaration(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readArrowFunctionDeclaration(const std::wstring &source,
                                       JSPosition &position);

  JSNode *readExpression19(const std::wstring &source, JSPosition &position);

  JSNode *readGroupExpression(const std::wstring &source, JSPosition &position);

  JSNode *readExpression18(const std::wstring &source, JSPosition &position);

  JSNode *readNewCallExpression(const std::wstring &source,
                                JSPosition &position);

  JSNode *readMemberExpression(const std::wstring &source, JSPosition &position,
                               JSNode *left);

  JSNode *readComputedMemberExpression(const std::wstring &source,
                                       JSPosition &position, JSNode *left);

  JSNode *readSpreadExpression(const std::wstring &source,
                               JSPosition &position);

  JSNode *readCallExpression(const std::wstring &source, JSPosition &position,
                             JSNode *left);

  JSNode *readOptionalMemberExpression(const std::wstring &source,
                                       JSPosition &position, JSNode *left);

  JSNode *readOptionalComputedMemberExpression(const std::wstring &source,
                                               JSPosition &position,
                                               JSNode *left);

  JSNode *readOptionalCallExpression(const std::wstring &source,
                                     JSPosition &position, JSNode *left);

  JSNode *readExpression17(const std::wstring &source, JSPosition &position);

  JSNode *readNewExpression(const std::wstring &source, JSPosition &position);

  JSNode *readExpression16(const std::wstring &source, JSPosition &position);

  JSNode *readExpression15(const std::wstring &source, JSPosition &position);

  JSNode *readTypeofExpression(const std::wstring &source,
                               JSPosition &position);

  JSNode *readVoidExpression(const std::wstring &source, JSPosition &position);

  JSNode *readDeleteExpression(const std::wstring &source,
                               JSPosition &position);

  JSNode *readAwaitExpression(const std::wstring &source, JSPosition &position);

  JSNode *readExpression14(const std::wstring &source, JSPosition &position);

  JSNode *readExpression13(const std::wstring &source, JSPosition &position);

  JSNode *readExpression12(const std::wstring &source, JSPosition &position);

  JSNode *readExpression11(const std::wstring &source, JSPosition &position);

  JSNode *readExpression10(const std::wstring &source, JSPosition &position);

  JSNode *readExpression9(const std::wstring &source, JSPosition &position);

  JSNode *readExpression8(const std::wstring &source, JSPosition &position);

  JSNode *readExpression7(const std::wstring &source, JSPosition &position);

  JSNode *readExpression6(const std::wstring &source, JSPosition &position);

  JSNode *readExpression5(const std::wstring &source, JSPosition &position);

  JSNode *readExpression4(const std::wstring &source, JSPosition &position);

  JSNode *readExpression3(const std::wstring &source, JSPosition &position);

  JSNode *readYieldExpression(const std::wstring &source, JSPosition &position);

  JSNode *readConditionExpression(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readAssigmentExpression(const std::wstring &source,
                                  JSPosition &position);

  JSNode *readExpression2(const std::wstring &source, JSPosition &position);

  JSNode *readExpression1(const std::wstring &source, JSPosition &position);

  JSNode *readExpression(const std::wstring &source, JSPosition &position);

  JSNode *readComment(const std::wstring &source, JSPosition &position,
                      bool *newline = nullptr);

public:
  virtual JSNode *parse(const std::wstring &source,
                        const JS_EVAL_TYPE &type = JS_EVAL_TYPE::PROGRAM) {
    this->_type = type;
    JSPosition pos = {};
    auto node = readProgram(source, pos);
    resolveBinding(source, node);
    return node;
  }

  JSParser(JSAllocator *allocator) : _allocator(allocator) {}

  virtual ~JSParser() {}
};