#ifndef _NEO_JS_HEADER_
#define _NEO_JS_HEADER_
#include <algorithm>
#include <any>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace neo {
class JSRuntime;
class JSContext;
class JSValue;

enum class JS_TYPE {
  EXCEPTION,
  INTERRUPT,
  INTERNAL,
  UNINITIALIZED,
  UNDEFINED,
  NUMBER,
  STRING,
  BOOLEAN,
  BIGINT,
  SYMBOL,
  OBJECT,
  FUNCTION,
  CLASS,
};

struct JSPosition {
  std::wstring funcname;
  size_t line;
  size_t column;
  size_t offset;
};

struct JSStackFrame {
  JSPosition position;
  std::wstring filename;
};

struct JSLocation {
  std::wstring filename;
  JSPosition start;
  JSPosition end;
  bool is(const std::wstring &source, const std::wstring &pair) const {
    if (end.offset - start.offset != pair.length()) {
      return false;
    }
    for (size_t i = 0; i < pair.size(); i++) {
      if (source[i + start.offset] != pair[i]) {
        return false;
      }
    }
    return true;
  }
  std::wstring get(const std::wstring &source) const {
    std::wstring result;
    for (auto i = start.offset; i < end.offset; i++) {
      result += source[i];
    }
    return result;
  }
};

enum class JSON_NODE_TYPE { OBJECT, ARRAY, STRING, NUMBER, NIL, BOOLEAN };
class JSON {
private:
  JSON_NODE_TYPE _type;
  std::any _value;
  friend class JSONCheckedNumber;

private:
  using JSONNumberValue = double;
  using JSONStringValue = std::wstring;
  using JSONBooleanValue = bool;
  using JSONNullValue = std::nullptr_t;
  using JSONArrayValue = std::vector<JSON>;
  using JSONObjectValue = std::map<std::wstring, JSON>;

public:
  JSON() : _type(JSON_NODE_TYPE::NIL), _value(nullptr) {}

  JSON(const JSON &another) {
    switch (another._type) {
    case JSON_NODE_TYPE::OBJECT:
      _value = std::any_cast<const JSONObjectValue &>(another._value);
      break;
    case JSON_NODE_TYPE::ARRAY:
      _value = std::any_cast<const JSONArrayValue &>(another._value);
      break;
    case JSON_NODE_TYPE::STRING:
      _value = std::any_cast<const JSONStringValue &>(another._value);
      break;
    case JSON_NODE_TYPE::NUMBER:
      _value = std::any_cast<const JSONNumberValue &>(another._value);
      break;
    case JSON_NODE_TYPE::NIL:
      _value = nullptr;
      break;
    case JSON_NODE_TYPE::BOOLEAN:
      _value = std::any_cast<JSONBooleanValue>(another._value);
      break;
    }
    _type = another._type;
  }

  JSON &setNull() {
    _type = JSON_NODE_TYPE::NIL;
    _value = nullptr;
    return *this;
  }

  JSON &setNumber(double value) {
    _type = JSON_NODE_TYPE::NUMBER;
    _value = value;
    return *this;
  }

  JSON &setBoolean(bool value) {
    _type = JSON_NODE_TYPE::BOOLEAN;
    _value = value;
    return *this;
  }

  JSON &setString(const std::wstring &value) {
    _type = JSON_NODE_TYPE::STRING;
    std::wstring str;
    _value = std::wstring{value};
    return *this;
  }

  JSON &setObject() {
    _type = JSON_NODE_TYPE::OBJECT;
    _value = JSONObjectValue{};
    return *this;
  }

  JSON &setArray() {
    _type = JSON_NODE_TYPE::ARRAY;
    _value = JSONArrayValue{};
    return *this;
  }

  JSONNumberValue &getNumber() {
    return std::any_cast<JSONNumberValue &>(_value);
  }

  JSONStringValue &getString() {
    return std::any_cast<JSONStringValue &>(_value);
  }

  JSONBooleanValue &getBoolean() {
    return std::any_cast<JSONBooleanValue &>(_value);
  }

  const JSONNumberValue &getNumber() const {
    return std::any_cast<const JSONNumberValue &>(_value);
  }

  const JSONStringValue &getString() const {
    return std::any_cast<const JSONStringValue &>(_value);
  }

  const JSONBooleanValue &getBoolean() const {
    return std::any_cast<const JSONBooleanValue &>(_value);
  }

  JSON &setIndex(size_t index, const JSON &item) {
    auto &items = std::any_cast<JSONArrayValue &>(_value);
    while (items.size() <= index) {
      items.push_back(JSON{});
    }
    items[index] = item;
    return *this;
  }

  JSON &getIndex(size_t index) {
    static JSON nil;
    auto &items = std::any_cast<JSONArrayValue &>(_value);
    if (index < items.size()) {
      return items[index];
    }
    return nil;
  }
  size_t getLength() const {
    auto &items = std::any_cast<const JSONArrayValue &>(_value);
    return items.size();
  }
  const JSON &getIndex(size_t index) const {
    static JSON nil;
    auto &items = std::any_cast<const JSONArrayValue &>(_value);
    if (index < items.size()) {
      return items[index];
    }
    return nil;
  }
  JSON &setField(const std::wstring &name, const JSON &field) {
    auto &properties = std::any_cast<JSONObjectValue &>(_value);
    properties[name] = field;
    return *this;
  }
  JSON &getField(const std::wstring &name) {
    static JSON nil;
    auto &properties = std::any_cast<JSONObjectValue &>(_value);
    if (properties.contains(name)) {
      return properties.at(name);
    }
    return nil;
  }
  const JSON &getField(const std::wstring &name) const {
    static JSON nil;
    auto &properties = std::any_cast<const JSONObjectValue &>(_value);
    if (properties.contains(name)) {
      return properties.at(name);
    }
    return nil;
  }
  std::vector<std::wstring> keys() const {
    auto &properties = std::any_cast<const JSONObjectValue &>(_value);
    std::vector<std::wstring> keys;
    for (auto &[k, _] : properties) {
      keys.push_back(k);
    }
    return keys;
  }

  const JSON_NODE_TYPE &getType() const { return _type; }

public:
  static std::wstring stringify(const JSON &node) {
    switch (node._type) {
    case JSON_NODE_TYPE::OBJECT: {
      std::wstring str = L"{";
      auto keys = node.keys();
      for (size_t i = 0; i < keys.size(); i++) {
        if (i != 0) {
          str += L",";
        }
        str += L"\"" + keys[i] + L"\":";
        str += stringify(node.getField(keys[i]));
      }
      str += L"}";
      return str;
    }
    case JSON_NODE_TYPE::ARRAY: {
      std::wstring str = L"[";
      for (size_t i = 0; i < node.getLength(); i++) {
        if (i != 0) {
          str += L",";
        }
        str += stringify(node.getIndex(i));
      }
      str += L"]";
      return str;
    }
    case JSON_NODE_TYPE::STRING: {
      std::wstring str = L"\"";
      for (auto &ch : node.getString()) {
        if (ch == '\n') {
          str += L"\\n";
        } else if (ch == '\r') {
          str += L"\\r";
        } else if (ch == '\"') {
          str += L"\\\"";
        } else {
          str += ch;
        }
      }
      str += L"\"";
      return str;
    }
    case JSON_NODE_TYPE::NUMBER: {
      return std::to_wstring(node.getNumber());
    }
    case JSON_NODE_TYPE::NIL:
      return L"null";
    case JSON_NODE_TYPE::BOOLEAN:
      if (node.getBoolean()) {
        return L"true";
      } else {
        return L"false";
      }
    }
    return L"";
  }
};

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
  JS_COMPILE_SCOPE_TYPE type;
  JSCompileScope *parent;
  JSNode *node;
  std::vector<JSLexDeclaration> declarations;
  std::set<std::wstring> refs;
};

struct JSNode {
  JSLocation location;
  JS_NODE_TYPE type = JS_NODE_TYPE::ERROR;
  JSNode *parent = nullptr;
  JSCompileScope *scope = nullptr;
  std::vector<JSNode *> children;
  std::vector<JSNode *> comments;
  virtual ~JSNode() {
    for (auto child : children) {
      delete child;
    }
    for (auto comment : comments) {
      delete comment;
    }
    if (scope) {
      delete scope;
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
  JSTokenNode() { type = JS_NODE_TYPE::TOKEN; }
};

struct JSErrorNode : public JSNode {
  std::wstring message;
  JSErrorNode() { type = JS_NODE_TYPE::ERROR; }
};

struct JSStringLiteralNode : public JSNode {
  JSStringLiteralNode() { type = JS_NODE_TYPE::LITERAL_STRING; }
};

struct JSNumberLiteralNode : public JSNode {
  JSNumberLiteralNode() { type = JS_NODE_TYPE::LITERAL_NUMBER; }
};

struct JSBigIntLiteralNode : public JSNode {
  JSBigIntLiteralNode() { type = JS_NODE_TYPE::LITERAL_BIGINT; }
};

struct JSRegexpLiteralNode : public JSNode {
  JSRegexpLiteralNode() { type = JS_NODE_TYPE::LITERAL_REGEX; }
};

struct JSIdentityLiteralNode : public JSNode {
  JSIdentityLiteralNode() { type = JS_NODE_TYPE::LITERAL_IDENTITY; }
};

struct JSNullLiteralNode : public JSNode {
  JSNullLiteralNode() { type = JS_NODE_TYPE::LITERAL_NULL; }
};

struct JSUndefinedLiteralNode : public JSNode {
  JSUndefinedLiteralNode() { type = JS_NODE_TYPE::LITERAL_UNDEFINED; }
};

struct JSBooleanLiteralNode : public JSNode {
  JSBooleanLiteralNode() { type = JS_NODE_TYPE::LITERAL_BOOLEAN; }
};

struct JSTemplateLiteralNode : public JSNode {
  JSNode *tag{};
  std::vector<JSNode *> quasis;
  std::vector<JSNode *> expressions;
  JSTemplateLiteralNode() { type = JS_NODE_TYPE::LITERAL_TEMPLATE; }
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
  JSThisLiteralNode() { type = JS_NODE_TYPE::LITERAL_THIS; }
};

struct JSSuperLiteralNode : public JSNode {
  JSSuperLiteralNode() { type = JS_NODE_TYPE::LITERAL_SUPER; }
};

struct JSCommentLiteralNode : public JSNode {
  JSCommentLiteralNode() { type = JS_NODE_TYPE::LITERAL_COMMENT; }
};

struct JSMultilineCommentLiteralNode : public JSNode {
  JSMultilineCommentLiteralNode() {
    type = JS_NODE_TYPE::LITERAL_MULTILINE_COMMENT;
  }
};

struct JSFunctionBaseNode : public JSNode {
  JSNode *body{};
  bool async{};
  bool generator{};
  std::vector<JSNode *> arguments;
  std::set<std::wstring> closure;
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
  JSProgramNode() { type = JS_NODE_TYPE::PROGRAM; }
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
  JSInterpreterDirectiveNode() {
    this->type = JS_NODE_TYPE::INTERPRETER_DIRECTIVE;
  }
};

struct JSDirectiveNode : public JSNode {
  JSDirectiveNode() { this->type = JS_NODE_TYPE::DIRECTIVE; }
};

struct JSEmptyStatementNode : public JSNode {
  JSEmptyStatementNode() { type = JS_NODE_TYPE::STATEMENT_EMPTY; }
};

struct JSBlockStatement : public JSNode {
  std::vector<JSNode *> statements;
  JSBlockStatement() { type = JS_NODE_TYPE::STATEMENT_BLOCK; }
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
struct JSDebuggerStatement : public JSNode {
  JSDebuggerStatement() { type = JS_NODE_TYPE::STATEMENT_DEBUGGER; }
};
struct JSReturnStatement : public JSNode {
  JSNode *value{};
  JSReturnStatement() { type = JS_NODE_TYPE::STATEMENT_RETURN; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};
struct JSLabelStatement : public JSNode {
  JSNode *label{};
  JSNode *statement{};
  JSLabelStatement() { type = JS_NODE_TYPE::STATEMENT_LABEL; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"label", label->toJSON(filename, source));
    json.setField(L"statement", statement->toJSON(filename, source));
    return json;
  }
};
struct JSBreakStatement : public JSNode {
  JSNode *label{};
  JSBreakStatement() { type = JS_NODE_TYPE::STATEMENT_BREAK; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (label) {
      json.setField(L"label", label->toJSON(filename, source));
    }
    return json;
  }
};
struct JSContinueStatement : public JSNode {
  JSNode *label{};
  JSContinueStatement() { type = JS_NODE_TYPE::STATEMENT_CONTINUE; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (label) {
      json.setField(L"label", label->toJSON(filename, source));
    }
    return json;
  }
};
struct JSIfStatement : public JSNode {
  JSNode *condition{};
  JSNode *alternate{};
  JSNode *consequent{};
  JSIfStatement() { type = JS_NODE_TYPE::STATEMENT_IF; }
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
struct JSSwitchStatement : public JSNode {
  JSNode *condition{};
  std::vector<JSNode *> cases;
  JSSwitchStatement() { type = JS_NODE_TYPE::STATEMENT_SWITCH; }
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
struct JSSwitchCaseStatement : public JSNode {
  JSNode *match{};
  std::vector<JSNode *> statements;
  JSSwitchCaseStatement() { type = JS_NODE_TYPE::STATEMENT_SWITCH_CASE; }
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
struct JSThrowStatement : public JSNode {
  JSNode *value{};
  JSThrowStatement() { type = JS_NODE_TYPE::STATEMENT_THROW; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    if (value) {
      json.setField(L"value", value->toJSON(filename, source));
    }
    return json;
  }
};
struct JSTryStatement : public JSNode {
  JSNode *body{};
  JSNode *onerror{};
  JSNode *onfinish{};
  JSTryStatement() { type = JS_NODE_TYPE::STATEMENT_TRY; }
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
struct JSTryCatchStatement : public JSNode {
  JSNode *identifier{};
  JSNode *body{};
  JSTryCatchStatement() { type = JS_NODE_TYPE::STATEMENT_TRY_CATCH; }
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
struct JSWhileStatement : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSWhileStatement() { type = JS_NODE_TYPE::STATEMENT_WHILE; }
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
struct JSDoWhileStatement : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSDoWhileStatement() { type = JS_NODE_TYPE::STATEMENT_DO_WHILE; }
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

struct JSForStatement : public JSNode {
  JSNode *init{};
  JSNode *condition{};
  JSNode *after{};
  JSNode *body{};
  JSForStatement() { type = JS_NODE_TYPE::STATEMENT_FOR; }
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

struct JSForInStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForInStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_IN; }
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
struct JSForOfStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForOfStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_OF; }
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

struct JSForAwaitOfStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForAwaitOfStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF; }
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
  JSExpressionStatementNode() { type = JS_NODE_TYPE::STATEMENT_EXPRESSION; }
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
  JSBinaryExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_BINARY; }
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
  JSAssigmentExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_ASSIGMENT; }
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
  JSGroupExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_GROUP; }
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
  JSMemberExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_MEMBER; }
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
  JSComputedMemberExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER;
  }
};

struct JSOptionalMemberExpressionNode : public JSMemberExpressionNode {
  JSOptionalMemberExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER;
  }
};

struct JSOptionalComputedMemberExpressionNode : public JSMemberExpressionNode {
  JSOptionalComputedMemberExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER;
  }
};

struct JSCallExpressionNode : public JSNode {
  JSNode *callee{};
  std::vector<JSNode *> arguments;
  JSCallExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_CALL; }
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
  JSOptionalCallExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL;
  }
};

struct JSNewExpressionNode : public JSCallExpressionNode {
  JSNewExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_NEW; }
};

struct JSAwaitExpressionNode : public JSNode {
  JSNode *value{};
  JSAwaitExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_AWAIT; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSYieldExpressionNode : public JSNode {
  JSNode *value{};
  JSYieldExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_YIELD; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSYieldDelegateExpressionNode : public JSYieldExpressionNode {
  JSYieldDelegateExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_YIELD_DELEGATE;
  }
};

struct JSDeleteExpressionNode : public JSNode {
  JSNode *value{};
  JSDeleteExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_DELETE; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};

struct JSVoidExpressionNode : public JSNode {
  JSNode *value{};
  JSVoidExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_VOID; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"value", value->toJSON(filename, source));
    return json;
  }
};
struct JSTypeofExpressioNode : public JSNode {
  JSNode *value{};
  JSTypeofExpressioNode() { type = JS_NODE_TYPE::EXPRESSION_TYPEOF; }
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
  JSConditionExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_CONDITION; }
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
  JSSpreadExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_SPREAD; }
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
  JSObjectPropertyNode() { type = JS_NODE_TYPE::OBJECT_PROPERTY; }
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
  JSObjectMethodNode() { type = JS_NODE_TYPE::OBJECT_METHOD; }
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
  JSObjectAccessorNode() { type = JS_NODE_TYPE::OBJECT_ACCESSOR; }
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
  JSObjectDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_OBJECT; }
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
  JSArrayDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_ARRAY; }
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
  JSFunctionArgumentDeclarationNode() {
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
  JSFunctionBodyDeclarationNode() {
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
  JSFunctionDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_FUNCTION; }
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
  JSArrowDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_ARROW_FUNCTION; }
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
  JSObjectPatternItemNode() { type = JS_NODE_TYPE::PATTERN_OBJECT_ITEM; }
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
  JSObjectPatternNode() { type = JS_NODE_TYPE::PATTERN_OBJECT; }
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
  JSArrayPatternItemNode() { type = JS_NODE_TYPE::PATTERN_ARRAY_ITEM; }
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
  JSArrayPatternNode() { type = JS_NODE_TYPE::PATTERN_ARRAY; }
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
  JSSpreadPatternItemNode() { type = JS_NODE_TYPE::PATTERN_SPREAD_ITEM; }
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
  JSVariableDeclaraionNode() { type = JS_NODE_TYPE::DECLARATION_VARIABLE; }
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
  JSVariableDeclaratorNode() { type = JS_NODE_TYPE::VARIABLE_DECLARATOR; }
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
  JSImportDeclarationNode() { type = JS_NODE_TYPE::IMPORT_DECLARATION; }
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
  JSImportSpecifierNode() { type = JS_NODE_TYPE::IMPORT_SPECIFIER; }
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
  JSImportDefaultNode() { type = JS_NODE_TYPE::IMPORT_DEFAULT; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"identifier", identifier->toJSON(filename, source));
    return json;
  }
};
struct JSImportNamespaceNode : public JSNode {
  JSNode *alias{};
  JSImportNamespaceNode() { type = JS_NODE_TYPE::IMPORT_NAMESPACE; }
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
  JSImportAttributeNode() { type = JS_NODE_TYPE::IMPORT_ATTARTUBE; }
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
  JSExportDeclarationNode() { type = JS_NODE_TYPE::EXPORT_DECLARATION; }
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
  JSExportDefaultNode() { type = JS_NODE_TYPE::EXPORT_DEFAULT; }
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
  JSExportSpecifierNode() { type = JS_NODE_TYPE::EXPORT_SPECIFIER; }
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
  JSExportNamedNode() { type = JS_NODE_TYPE::EXPORT_NAMED; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"declaration", declaration->toJSON(filename, source));
    return json;
  }
};
struct JSExportNamespaceNode : public JSNode {
  JSNode *alias{};
  JSExportNamespaceNode() { type = JS_NODE_TYPE::EXPORT_NAMED; }
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
  JSClassMethodNode() { type = JS_NODE_TYPE::CLASS_METHOD; }
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
  JSClassPropertyNode() { type = JS_NODE_TYPE::CLASS_PROPERTY; }
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
  JSClassAccessorNode() { type = JS_NODE_TYPE::CLASS_ACCESSOR; }
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
  JSStaticBlockNode() { type = JS_NODE_TYPE::CLASS_STATIC_BLOCK; }
  JSON toJSON(const std::wstring &filename,
              const std::wstring &source) const override {
    auto json = JSNode::toJSON(filename, source);
    json.setField(L"statement", statement->toJSON(filename, source));
    return json;
  }
};
struct JSClassDeclaration : public JSNode {
  JSNode *extends{};
  JSNode *identifier{};
  std::set<std::wstring> closure{};
  std::vector<JSNode *> properties;
  JSClassDeclaration() { type = JS_NODE_TYPE::DECLARATION_CLASS; }
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

enum class JS_OPERATOR {
  BEGIN = 0,
  END,
  PUSH,
  POP,
  PUSH_VALUE,
  NIL,
  UNDEFINED,
  TRUE,
  FALSE,
  REGEX,
  CLASS,
  LOAD,
  STORE,
  REF,
  STR,
  BIGINT,
  VAR,
  CONST,
  LET,
  THIS,
  SUPER,
  OBJECT,
  ARRAY,
  SUPER_CALL,
  FUNCTION,
  ASYNCFUNCTION,
  GENERATOR,
  ASYNCGENERATOR,
  ENABLE,
  DISABLE,
  GET_FIELD,
  SET_FIELD,
  SET_GETTER,
  SET_SETTER,
  GET_KEYS,
  SET_SUPER_FIELD,
  GET_SUPER_FIELD,
  SET_INITIALIZER,
  CALL,
  MEMBER_CALL,
  VOID,
  TYPEOF,
  NEW,
  DELETE,
  RET,
  YIELD,
  AWAIT,
  THROW,
  YIELD_DELEGATE,
  TRY_BEGIN,
  TRY_END,
  DEFER,
  JMP,
  JTRUE,
  JFALSE,
  JNULL,
  JNOT_NULL,
  ADD,
  SUB,
  DIV,
  MUL,
  MOD,
  POW,
  AND,
  OR,
  NOT,
  XOR,
  SHR,
  SHL,
  USHR,
  EQ,
  SEQ,
  NE,
  SNE,
  GT,
  LT,
  GE,
  LE,
  INC,
  DEC,
  UPDATE_INC,
  UPDATE_DEC,
  NEXT,
  AWAIT_NEXT,
  SPREAD,
  MERGE,
  OBJECT_SPREAD,
  ARRAY_SPREAD,
  EMPTY_CHECK,
  ARGUMENT_SPREAD,
  HLT,
  DEBUGGER,
  WITH
};

struct JSEvalContext {
  size_t pc;
  std::vector<JSValue *> stack;
  JSValue *self;
};

struct JSProgram {
  std::wstring filename;
  std::vector<std::wstring> constants;
  std::vector<uint16_t> codes;
  std::vector<JSPosition> stacks;
  JSNode *error;
  JSProgram(const std::wstring &filename)
      : filename(filename), error(nullptr) {}
  virtual ~JSProgram() {
    if (error) {
      delete error;
      error = nullptr;
    }
  }
  std::wstring toString() {
    std::wstringstream ss;
    ss << L"[.section text]" << std::endl;
    for (size_t index = 0; index < constants.size(); index++) {
      ss << L"." << index << ": \"";
      for (auto &ch : constants[index]) {
        if (ch == '\n') {
          ss << L"\\n";
        } else if (ch == L'\r') {
          ss << L"\\r";
        } else if (ch == '\t') {
          ss << L"\\t";
        } else if (ch == '\"') {
          ss << L"\\\"";
        } else if (ch == '\\') {
          ss << L"\\\\";
        } else {
          ss << ch;
        }
      }
      ss << L"\"" << std::endl;
    }
    size_t offset = 0;
    ss << L"[.section code]" << std::endl;
    while (offset < codes.size()) {
      ss << offset << ": ";
      auto opt = (JS_OPERATOR) * (codes.data() + offset);
      offset++;
      switch (opt) {
      case JS_OPERATOR::BEGIN: {
        ss << L"BEGIN";
        break;
      }
      case JS_OPERATOR::END: {
        ss << L"END";
        break;
      }
      case JS_OPERATOR::PUSH: {
        ss << L"PUSH " << *(double *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::POP: {
        ss << L"POP";
        break;
      }
      case JS_OPERATOR::PUSH_VALUE: {
        ss << L"PUSH_VALUE " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::NIL: {
        ss << L"NIL";
        break;
      }
      case JS_OPERATOR::UNDEFINED: {
        ss << L"UNDEFINED";
        break;
      }
      case JS_OPERATOR::TRUE: {
        ss << L"TRUE";
        break;
      }
      case JS_OPERATOR::FALSE: {
        ss << L"FALSE";
        break;
      }
      case JS_OPERATOR::REGEX: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"REGEX \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::LOAD: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"LOAD \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::STORE: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"STORE \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::STR: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"STR \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::REF: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"REF \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::ENABLE: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"ENABLE \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::DISABLE: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"DISABLE \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case neo::JS_OPERATOR::SUPER: {
        ss << L"SUPER";
        break;
      }
      case neo::JS_OPERATOR::THIS: {
        ss << L"THIS";
        break;
      }
      case neo::JS_OPERATOR::SUPER_CALL: {
        ss << L"SUPER_CALL " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::GET_FIELD: {
        ss << L"GET_FIELD";
        break;
      }
      case JS_OPERATOR::SET_FIELD: {
        ss << L"SET_FIELD";
        break;
      }
      case JS_OPERATOR::SET_GETTER: {
        ss << L"SET_GETTER";
        break;
      }
      case JS_OPERATOR::SET_SETTER: {
        ss << L"SET_SETTER";
        break;
      }
      case JS_OPERATOR::GET_SUPER_FIELD: {
        ss << L"GET_SUPER_FIELD";
        break;
      }
      case JS_OPERATOR::SET_SUPER_FIELD: {
        ss << L"SET_SUPER_FIELD";
        break;
      }
      case JS_OPERATOR::CALL: {
        ss << L"CALL " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::MEMBER_CALL: {
        ss << L"MEMBER_CALL " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::VOID: {
        ss << L"VOID";
        break;
      }
      case JS_OPERATOR::TYPEOF: {
        ss << L"TYPEOF";
        break;
      }
      case JS_OPERATOR::NEW: {
        ss << L"NEW " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::DELETE: {
        ss << L"DELETE";
        break;
      }
      case JS_OPERATOR::RET: {
        ss << L"RET";
        break;
      }
      case JS_OPERATOR::YIELD: {
        ss << L"YIELD";
        break;
      }
      case JS_OPERATOR::AWAIT: {
        ss << L"AWAIT";
        break;
      }
      case JS_OPERATOR::YIELD_DELEGATE: {
        ss << L"YIELD_DELEGATE";
        break;
      }
      case JS_OPERATOR::JMP: {
        ss << L"JMP " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::JTRUE: {
        ss << L"JTRUE " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::JFALSE: {
        ss << L"JFALSE " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::JNULL: {
        ss << L"JNULL " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::JNOT_NULL: {
        ss << L"JNOT_NULL " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::ADD: {
        ss << L"ADD";
        break;
      }
      case JS_OPERATOR::SUB: {
        ss << L"SUB";
        break;
      }
      case JS_OPERATOR::DIV: {
        ss << L"DIV";
        break;
      }
      case JS_OPERATOR::MUL: {
        ss << L"MUL";
        break;
      }
      case JS_OPERATOR::MOD: {
        ss << L"MOD";
        break;
      }
      case JS_OPERATOR::POW: {
        ss << L"POW";
        break;
      }
      case JS_OPERATOR::AND: {
        ss << L"AND";
        break;
      }
      case JS_OPERATOR::OR: {
        ss << L"OR";
        break;
      }
      case JS_OPERATOR::NOT: {
        ss << L"NOT";
        break;
      }
      case JS_OPERATOR::XOR: {
        ss << L"XOR";
        break;
      }
      case JS_OPERATOR::SHR: {
        ss << L"SHR";
        break;
      }
      case JS_OPERATOR::SHL: {
        ss << L"SHL";
        break;
      }
      case JS_OPERATOR::USHR: {
        ss << L"USHR";
        break;
      }
      case JS_OPERATOR::EQ: {
        ss << L"EQ";
        break;
      }
      case JS_OPERATOR::SEQ: {
        ss << L"SEQ";
        break;
      }
      case JS_OPERATOR::NE: {
        ss << L"NE";
        break;
      }
      case JS_OPERATOR::SNE: {
        ss << L"SNE";
        break;
      }
      case JS_OPERATOR::GT: {
        ss << L"GT";
        break;
      }
      case JS_OPERATOR::LT: {
        ss << L"LT";
        break;
      }
      case JS_OPERATOR::GE: {
        ss << L"GE";
        break;
      }
      case JS_OPERATOR::LE: {
        ss << L"LE";
        break;
      }
      case JS_OPERATOR::INC: {
        ss << L"INC";
        break;
      }
      case JS_OPERATOR::DEC: {
        ss << L"DEC";
        break;
      }
      case JS_OPERATOR::UPDATE_INC: {
        ss << L"UPDATE_INC";
        break;
      }
      case JS_OPERATOR::UPDATE_DEC: {
        ss << L"UPDATE_DEC";
        break;
      }
      case JS_OPERATOR::NEXT: {
        ss << L"NEXT";
        break;
      }
      case JS_OPERATOR::AWAIT_NEXT: {
        ss << L"AWAIT_NEXT";
        break;
      }
      case JS_OPERATOR::OBJECT_SPREAD: {
        ss << L"OBJECT_SPREAD " << *(uint32_t *)(codes.data() + offset);
        offset += 2;
        break;
      }
      case JS_OPERATOR::ARRAY_SPREAD: {
        ss << L"ARRAY_SPREAD";
        break;
      }
      case neo::JS_OPERATOR::ARGUMENT_SPREAD: {
        ss << L"ARGUMENT_SPREAD";
        break;
      }
      case JS_OPERATOR::VAR: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"VAR \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::CONST: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"CONST \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::LET: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"LET \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::FUNCTION: {
        ss << L"FUNCTION " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::ASYNCFUNCTION: {
        ss << L"ASYNCFUNCTION " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::GENERATOR: {
        ss << L"GENERATOR " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::ASYNCGENERATOR: {
        ss << L"ASYNCGENERATOR " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::HLT: {
        ss << L"HLT";
        break;
      }
      case neo::JS_OPERATOR::DEBUGGER: {
        ss << L"DEBUGGER";
        break;
      }
        // default:
        //   ss << L"UNKNOWN_" << (uint16_t)opt;
      case JS_OPERATOR::SPREAD: {
        ss << L"SPREAD";
        break;
      }
      case JS_OPERATOR::MERGE: {
        ss << L"MERGE";
        break;
      }
      case JS_OPERATOR::GET_KEYS: {
        ss << L"GET_KEYS";
        break;
      }
      case JS_OPERATOR::TRY_BEGIN: {
        ss << L"TRY_BEGIN " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::TRY_END: {
        ss << L"TRY_END";
        break;
      }
      case JS_OPERATOR::DEFER: {
        ss << L"DEFER " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      case JS_OPERATOR::THROW: {
        ss << L"THROW";
        break;
      }
      case JS_OPERATOR::BIGINT: {
        auto idx = *(uint32_t *)(codes.data() + offset);
        ss << L"BIGINT \"" << constants[idx] << L"\"";
        offset += 2;
        break;
      }
      case JS_OPERATOR::OBJECT: {
        ss << L"OBJECT";
        break;
      }
      case JS_OPERATOR::ARRAY: {
        ss << L"ARRAY";
        break;
      }
      case JS_OPERATOR::EMPTY_CHECK: {
        ss << L"EMPTY_CHECK";
        break;
      }
      case JS_OPERATOR::CLASS: {
        ss << L"CLASS";
        break;
      }
      case neo::JS_OPERATOR::WITH: {
        ss << L"WITH";
        break;
      }
      case JS_OPERATOR::SET_INITIALIZER: {
        ss << L"SET_INITIALIZER " << *(uint64_t *)(codes.data() + offset);
        offset += 4;
        break;
      }
      }
      ss << std::endl;
    }
    return ss.str();
  }
};

class JSParser {
private:
  JSCompileScope *_scope = nullptr;

private:
  JSCompileScope *pushScope(const JS_COMPILE_SCOPE_TYPE &type, JSNode *node) {
    auto scope = new JSCompileScope{type, _scope, node};
    _scope = scope;
    return scope;
  }

  void popScope() {
    if (!_scope) {
      ;
    }
    _scope = _scope->parent;
  }

  JSNode *resolveDeclarator(const JS_DECLARATION_TYPE &type,
                            const std::wstring &source, JSNode *identifier,
                            JSNode *declaration) {
    if (identifier->type == JS_NODE_TYPE::LITERAL_IDENTITY) {
      auto name = identifier->location.get(source);
      if (type == JS_DECLARATION_TYPE::VAR) {
        auto scope = _scope;
        while (scope->type == JS_COMPILE_SCOPE_TYPE::BLOCK) {
          scope = scope->parent;
        }
        scope->declarations.push_back({type, declaration, name});
      } else {
        _scope->declarations.push_back({type, declaration, name});
      }
    } else if (identifier->type == JS_NODE_TYPE::PATTERN_SPREAD_ITEM) {
      auto node = dynamic_cast<JSSpreadPatternItemNode *>(identifier);
      auto err = resolveDeclarator(type, source, node->value, declaration);
      if (err) {
        return err;
      }
    } else if (identifier->type == JS_NODE_TYPE::PATTERN_ARRAY) {
      auto node = dynamic_cast<JSArrayPatternNode *>(identifier);
      for (auto item : node->items) {
        auto node = dynamic_cast<JSArrayPatternItemNode *>(item);
        auto err = resolveDeclarator(type, source, node->alias, declaration);
        if (err) {
          return err;
        }
      }
    } else if (identifier->type == JS_NODE_TYPE::PATTERN_OBJECT) {
      auto node = dynamic_cast<JSObjectPatternNode *>(identifier);
      for (auto item : node->items) {
        auto node = dynamic_cast<JSObjectPatternItemNode *>(item);
        if (node->alias) {
          auto err = resolveDeclarator(type, source, node->alias, declaration);
          if (err) {
            return err;
          }
        } else {
          auto err = resolveDeclarator(type, source, node->key, declaration);
          if (err) {
            return err;
          }
        }
      }
    } else {
      return createError(std::wstring(L"Invalid declaration identifier '") +
                             identifier->location.get(source) + L"'",
                         source, identifier->location.start);
    }
    return nullptr;
  }

  JSNode *declarVariable(const JS_DECLARATION_TYPE &type,
                         const std::wstring &source, JSNode *declaration) {
    if (declaration->type == JS_NODE_TYPE::VARIABLE_DECLARATOR) {
      auto declarator = dynamic_cast<JSVariableDeclaratorNode *>(declaration);
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
      if (err) {
        return err;
      }
    } else if (declaration->type ==
               JS_NODE_TYPE::DECLARATION_FUNCTION_ARGUMENT) {
      auto declarator =
          dynamic_cast<JSFunctionArgumentDeclarationNode *>(declaration);
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
      if (err) {
        return err;
      }
    } else if (declaration->type == JS_NODE_TYPE::DECLARATION_FUNCTION) {
      auto declarator = dynamic_cast<JSFunctionDeclarationNode *>(declaration);
      if (declarator->identifier) {
        auto err = resolveDeclarator(type, source, declarator->identifier,
                                     declaration);
        if (err) {
          return err;
        }
      }
    } else if (declaration->type == JS_NODE_TYPE::DECLARATION_CLASS) {
      auto declarator = dynamic_cast<JSClassDeclaration *>(declaration);
      if (declarator->identifier) {
        auto err = resolveDeclarator(type, source, declarator->identifier,
                                     declaration);
        if (err) {
          return err;
        }
      }
    } else if (declaration->type == JS_NODE_TYPE::IMPORT_DEFAULT) {
      auto declarator = dynamic_cast<JSImportDefaultNode *>(declaration);
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
      if (err) {
        return err;
      }
    } else if (declaration->type == JS_NODE_TYPE::IMPORT_NAMESPACE) {
      auto declarator = dynamic_cast<JSImportNamespaceNode *>(declaration);
      auto err =
          resolveDeclarator(type, source, declarator->alias, declaration);
      if (err) {
        return err;
      }
    } else if (declaration->type == JS_NODE_TYPE::IMPORT_SPECIFIER) {
      auto declarator = dynamic_cast<JSImportSpecifierNode *>(declaration);
      if (declarator->alias) {
        auto err =
            resolveDeclarator(type, source, declarator->alias, declaration);
        if (err) {
          return err;
        }
      } else {
        auto err = resolveDeclarator(type, source, declarator->identifier,
                                     declaration);
        if (err) {
          return err;
        }
      }
    }
    return nullptr;
  }

  void resolveBinding(const std::wstring &source, JSNode *node) {
    for (auto child : node->children) {
      resolveBinding(source, child);
    }
    if (node->scope) {
      auto &refs = node->scope->refs;
      for (auto &name : refs) {
        auto scope = node->scope;
        std::vector<JSCompileScope *> scopes;
        JSNode *declaration = nullptr;
        while (scope) {
          auto it = scope->declarations.begin();
          while (it != scope->declarations.end()) {
            if (it->name == name) {
              break;
            }
            it++;
          }
          if (it != scope->declarations.end()) {
            declaration = it->declaration;
            break;
          } else if (scope->type == JS_COMPILE_SCOPE_TYPE::LEX) {
            scopes.push_back(scope);
          }
          scope = scope->parent;
        }
        if (declaration) {
          if (node->type == JS_NODE_TYPE::DECLARATION_FUNCTION_BODY) {
            if (declaration == node->parent) {
              continue;
            }
          } else if (node == declaration) {
            continue;
          }
          for (auto scope : scopes) {
            auto func = dynamic_cast<JSFunctionBaseNode *>(scope->node);
            if (func) {
              func->closure.insert(name);
              func->scope->refs.insert(name);
            }
            auto clazz = dynamic_cast<JSClassDeclaration *>(scope->node);
            if (clazz) {
              clazz->closure.insert(name);
              clazz->scope->refs.insert(name);
            }
          }
        }
      }
    }
  }

private:
  JSNode *createError(const std::wstring &message, const std::wstring &source,
                      const JSPosition &end) {

    auto *node = new JSErrorNode();
    node->message = message;
    node->location = getLocation(source, {}, end);
    return node;
  }

private:
  bool isLineTerminatior(wchar_t c) {
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
      if (isLineTerminatior(source[loc.start.offset])) {
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
      if (isLineTerminatior(source[loc.end.offset])) {
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
    if (isLineTerminatior(source[position.offset])) {
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
          delete token;
        }
        return true;
      }
    }
    delete token;
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
          delete token;
        }
        return true;
      }
    }
    delete token;
    return false;
  }
  JSNode *readSymbolToken(const std::wstring &source, JSPosition &position) {
    static const std::vector<std::wstring> operators = {
        L">>>=", L"...",    L"<<=", L">>>", L"===", L"!==", L"**=", L">>=",
        L"&&=",  LR"(??=)", L"**",  L"==",  L"!=",  L"<<",  L">>",  L"<=",
        L">=",   L"&&",     L"||",  L"??",  L"++",  L"--",  L"+=",  L"-=",
        L"*=",   L"/=",     L"%=",  L"||=", L"&=",  L"^=",  L"|=",  L"=>",
        L"?.",   L"=",      L"*",   L"/",   L"%",   L"+",   L"-",   L"<",
        L">",    L"&",      L"^",   L"|",   L",",   L"!",   L"~",   L"(",
        L")",    L"[",      L"]",   L"{",   L"}",   L"@",   L"#",   L".",
        L"?",    L":",      L";",
    };
    auto current = position;
    for (auto &opt : operators) {
      bool found = true;
      auto offset = current.offset;
      for (auto &ch : opt) {
        auto &chr = source[offset];
        if (chr != ch) {
          found = false;
          break;
        }
        offset++;
      }
      if (found) {
        current.offset = offset;
        auto token = new JSTokenNode{};
        token->location = getLocation(source, position, current);
        position = current;
        return token;
      }
    }
    return nullptr;
  }

  JSNode *readComments(const std::wstring &source, JSPosition &position,
                       std::vector<JSNode *> &comments) {
    auto comment = readComment(source, position);
    while (comment != nullptr) {
      if (comment->type == JS_NODE_TYPE::ERROR) {
        return comment;
      }
      comments.push_back(comment);
      while (skipInvisible(source, position)) {
      }
      comment = readComment(source, position);
    }
    return nullptr;
  }

private:
  JSNode *readTemplateLiteral(const std::wstring &source, JSPosition &position,
                              JSNode *tag = nullptr) {
    auto current = position;
    if (source[current.offset] != '`') {
      return nullptr;
    }
    auto node = new JSTemplateLiteralNode{};
    current.offset++;
    auto start = current;
    JSNode *quasi = nullptr;
    for (;;) {
      if (!source[current.offset]) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (source[current.offset] == '`') {
        auto end = current;
        quasi = new JSStringLiteralNode{};
        quasi->location = getLocation(source, start, end);
        node->quasis.push_back(quasi);
        quasi->addParent(node);
        current.offset++;
        break;
      }
      if (source[current.offset] == '$' && source[current.offset + 1] == '{') {
        auto end = current;
        quasi = new JSStringLiteralNode{};
        quasi->location = getLocation(source, start, end);
        node->quasis.push_back(quasi);
        quasi->addParent(node);
        current.offset++;
        current.offset++;
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        auto exp = readExpression(source, current);
        if (!exp) {
          delete node;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (exp->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return exp;
        }
        node->expressions.push_back(exp);
        exp->addParent(node);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        if (source[current.offset] != '}') {
          delete node;
          return createError(L"Invalid or unexpected token", source, current);
        } else {
          start.offset = current.offset + 1;
        }
      }
      if (source[current.offset] == '\\') {
        current.offset++;
      }
      current.offset++;
    }
    node->location = getLocation(source, position, current);
    node->tag = tag;
    if (tag) {
      tag->addParent(node);
    }
    position = current;
    return node;
  }

  JSNode *readIdentifyLiteral(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (source[current.offset] == '$' || source[current.offset] == '_' ||
        (source[current.offset] >= 'a' && source[current.offset] <= 'z') ||
        (source[current.offset] >= 'A' && source[current.offset] <= 'Z')) {
      current.offset++;
      while ((source[current.offset] >= '0' && source[current.offset] <= '9') ||
             (source[current.offset] >= 'A' && source[current.offset] <= 'Z') ||
             (source[current.offset] >= 'a' && source[current.offset] <= 'z') ||
             source[current.offset] == '$' || source[current.offset] == '_') {
        current.offset++;
      }
      auto node = new JSIdentityLiteralNode();
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readStringLiteral(const std::wstring &source, JSPosition &position) {

    if (source[position.offset] == '\'' || source[position.offset] == '\"') {
      auto current = position;
      current.offset++;
      for (;;) {
        if (source[current.offset] == source[position.offset]) {
          break;
        }
        if (!source[current.offset]) {
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (source[current.offset] == '\\') {
          current.offset++;
        }
        current.offset++;
      }
      current.offset++;
      auto token = new JSStringLiteralNode();
      token->location = getLocation(source, position, current);
      position = current;
      return token;
    }
    return nullptr;
  }

  JSNode *readNullLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identify = readIdentifyLiteral(source, current);
    if (!identify) {
      return identify;
    }
    if (identify->location.is(source, L"null")) {
      auto node = new JSNullLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      delete identify;
      return node;
    }
    delete identify;
    return nullptr;
  }

  JSNode *readUndefinedLiteral(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    auto identify = readIdentifyLiteral(source, current);
    if (!identify) {
      return identify;
    }
    if (identify->location.is(source, L"undefined")) {
      auto node = new JSUndefinedLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      delete identify;
      return node;
    }
    delete identify;
    return nullptr;
  }

  JSNode *readThisLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identify = readIdentifyLiteral(source, current);
    if (!identify) {
      return identify;
    }
    if (identify->location.is(source, L"this")) {
      auto node = new JSThisLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      delete identify;
      return node;
    }
    delete identify;
    return nullptr;
  }

  JSNode *readSuperLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identify = readIdentifyLiteral(source, current);
    if (!identify) {
      return identify;
    }
    if (identify->location.is(source, L"super")) {
      auto node = new JSSuperLiteralNode{};
      node->location = getLocation(source, position, current);
      delete identify;
      position = current;
      return node;
    }
    delete identify;
    return nullptr;
  }

  JSNode *readBooleanLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identify = readIdentifyLiteral(source, current);
    if (!identify) {
      return identify;
    }
    if (identify && (identify->location.is(source, L"true") ||
                     identify->location.is(source, L"false"))) {
      auto node = new JSBooleanLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      delete identify;
      return node;
    }
    delete identify;
    return nullptr;
  }

  JSNode *readNumberLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (source[current.offset] >= '0' && source[current.offset] <= '9') {
      if (source[current.offset] == '0') {
        if (source[current.offset + 1] == 'x' ||
            source[current.offset + 1] == 'X') {
          current.offset += 2;
          while ((source[current.offset] >= '0' &&
                  source[current.offset] <= '9') ||
                 (source[current.offset] >= 'a' &&
                  source[current.offset] <= 'f') ||
                 (source[current.offset] >= 'A' &&
                  source[current.offset] <= 'F')) {
            current.offset++;
          }
          auto node = new JSNumberLiteralNode();
          node->location = getLocation(source, position, current);
          position = current;
          return node;
        }
        if (source[current.offset + 1] == 'o' ||
            source[current.offset + 1] == 'O') {
          current.offset += 2;
          while ((source[current.offset] >= '0' &&
                  source[current.offset] <= '7')) {
            current.offset++;
          }
          auto node = new JSNumberLiteralNode();
          node->location = getLocation(source, position, current);
          position = current;
          return node;
        }
        if (source[current.offset + 1] == 'b' ||
            source[current.offset + 1] == 'B') {
          current.offset += 2;
          while ((source[current.offset] >= '0' &&
                  source[current.offset] <= '1')) {
            current.offset++;
          }
          auto node = new JSNumberLiteralNode();
          node->location = getLocation(source, position, current);
          position = current;
          return node;
        }
      }
      while (source[current.offset] >= '0' && source[current.offset] <= '9') {
        current.offset++;
      }
      if (source[current.offset] == 'n') {
        current.offset++;
        auto node = new JSBigIntLiteralNode();
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
      if (source[current.offset] == '.') {
        current.offset++;
      }
      while (source[current.offset] >= '0' && source[current.offset] <= '9') {
        current.offset++;
      }
      if (source[current.offset] == 'e' || source[current.offset] == 'E') {
        current.offset++;
      }
      while (source[current.offset] >= '0' && source[current.offset] <= '9') {
        current.offset++;
      }
      auto node = new JSNumberLiteralNode();
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    } else if (source[current.offset] == '.' &&
               source[current.offset + 1] >= '0' &&
               source[current.offset + 1] <= '9') {
      current.offset++;
      while (source[current.offset + 1] >= '0' &&
             source[current.offset + 1] <= '9') {
        current.offset++;
      }
      if (source[current.offset] == 'e' || source[current.offset] == 'E') {
        current.offset++;
      }
      if (source[current.offset] < '0' || source[current.offset] > '9') {
        return createError(L"Invalid or unexpected token", source, current);
      }
      while (source[current.offset] >= '0' && source[current.offset] <= '9') {
        current.offset++;
      }
      auto node = new JSNumberLiteralNode();
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readRegexpLiteral(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (source[current.offset] == '/') {
      current.offset++;
      for (;;) {
        if (source[current.offset] == '/') {
          break;
        }
        if (!source[current.offset]) {

          return createError(L"Invalid or unexpected token", source, current);
        }
        if (source[current.offset] == '[') {
          for (;;) {
            if (source[current.offset == ']']) {
              break;
            }
            if (!source[current.offset]) {

              return createError(L"Invalid or unexpected token", source,
                                 current);
            }
            if (source[current.offset] == '\\') {
              current.offset++;
            }
            current.offset++;
          }
        }
        if (source[current.offset] == '\\') {
          current.offset++;
        }
        current.offset++;
      }
      while (source[current.offset] == 'g' || source[current.offset] == 'i' ||
             source[current.offset] == 'm') {
        current.offset++;
      }
      auto node = new JSRegexpLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readProgram(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto *node = new JSProgramNode();
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    auto interpreter = readInterpreterDirective(source, current);
    if (interpreter) {
      if (interpreter->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return interpreter;
      } else {
        node->interpreter = interpreter;
        interpreter->addParent(node);
      }
    }
    for (;;) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto directive = readDirective(source, current);
      if (!directive) {
        break;
      }
      if (directive->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return directive;
      }
      node->directives.push_back(directive);
      directive->addParent(node);
    }
    for (;;) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto statement = readStatement(source, current);
      if (!statement) {
        break;
      }
      if (statement->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return statement;
      }
      node->statements.push_back(statement);
      statement->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (source[current.offset]) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    popScope();
    return node;
  }

  JSNode *readInterpreterDirective(const std::wstring &source,
                                   JSPosition &position) {
    auto current = position;
    if (source[position.offset] == '#' && source[position.offset + 1] == '!') {
      auto *node = new JSInterpreterDirectiveNode();
      current.offset += 2;
      while (source[current.offset] &&
             !isLineTerminatior(source[current.offset])) {
        current.offset++;
      }
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readDirective(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto node = readStringLiteral(source, current);
    if (!node || node->type == JS_NODE_TYPE::ERROR) {
      return node;
    }
    delete node;
    node = new JSDirectiveNode();
    node->location = getLocation(source, position, current);
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      delete node;
      return nullptr;
    }
    position = current;
    return node;
  }

  JSNode *readEmptyStatement(const std::wstring &source, JSPosition &position) {
    if (checkSymbol({L";"}, source, position)) {
      auto current = position;
      current.offset++;
      auto node = new JSEmptyStatementNode{};
      node->location = getLocation(source, position, current);
      return node;
    }
    return nullptr;
  }

  JSNode *readBlockStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"{"}, source, current)) {
      return nullptr;
    }
    auto node = new JSBlockStatement{};
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto statement = readStatement(source, current);
    while (statement) {
      if (statement->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return statement;
      }
      node->statements.push_back(statement);
      statement->addParent(node);
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      statement = readStatement(source, current);
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return createError(L"Invalid o runexcepted token", source, current);
    }
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readDebuggerStatement(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"debugger"}, source, current)) {
      return nullptr;
    }
    auto node = new JSDebuggerStatement{};
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readReturnStatement(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"return"}, source, current)) {
      return nullptr;
    }
    auto node = new JSReturnStatement{};

    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        auto val = readExpression(source, current);
        if (val) {
          if (val->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return val;
          }
          node->value = val;
          val->addParent(node);
        } else {
          delete node;
          return createError(L"Invalid or nexcepted token", source, current);
        }
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readLabelStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identifier = readIdentifyLiteral(source, current);
    if (identifier && isKeyword(source, identifier->location)) {
      delete identifier;
      identifier = nullptr;
    }
    if (!identifier || identifier->type == JS_NODE_TYPE::ERROR) {
      return identifier;
    }
    auto node = new JSLabelStatement{};
    node->label = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L":"}, source, current)) {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto statement = readStatement(source, current);
    if (!statement) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (statement->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return statement;
    }
    node->statement = statement;
    statement->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readBreakStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"break"}, source, current)) {
      return nullptr;
    }
    auto node = new JSBreakStatement{};
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        auto val = readIdentifyLiteral(source, current);
        if (val) {
          if (val->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return val;
          }
          node->label = val;
          val->addParent(node);
        } else {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readContinueStatement(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"continue"}, source, current)) {
      return nullptr;
    }
    auto node = new JSContinueStatement{};
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        auto val = readIdentifyLiteral(source, current);
        if (val) {
          if (val->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return val;
          }
          node->label = val;
          val->addParent(node);
        } else {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readIfStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"if"}, source, current)) {
      return nullptr;
    }
    auto node = new JSIfStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto condition = readExpression(source, current);
    if (!condition) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (condition->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return condition;
    }
    node->condition = condition;
    condition->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto consequent = readStatement(source, current);
    if (!consequent) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (consequent->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return consequent;
    }
    node->consequent = consequent;
    consequent->addParent(node);
    auto backup = current;
    std::vector<JSNode *> comments;
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"else"}, source, current)) {
      for (auto &comment : comments) {
        node->comments.push_back(comment);
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto alternate = readStatement(source, current);
      if (!alternate) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (alternate->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return alternate;
      }
      node->alternate = alternate;
      alternate->addParent(node);
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readSwitchStatement(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"switch"}, source, current)) {
      return nullptr;
    }
    auto node = new JSSwitchStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto condition = readExpression(source, current);
    if (!condition) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (condition->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return condition;
    }
    node->condition = condition;
    condition->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    if (!checkSymbol({L"{"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto cas = readSwitchCaseStatement(source, current);
    while (cas) {
      if (cas->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return cas;
      }
      node->cases.push_back(cas);
      cas->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      cas = readSwitchCaseStatement(source, current);
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readSwitchCaseStatement(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto node = new JSSwitchCaseStatement{};
    if (checkIdentifier({L"default"}, source, current)) {
    } else if (checkIdentifier({L"case"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto expr = readExpression(source, current);
      if (!expr) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (expr->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return expr;
      }
      node->match = expr;
      expr->addParent(node);
    } else {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L":"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto statement = readStatement(source, current);
    while (statement) {
      if (statement->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return statement;
      }
      node->statements.push_back(statement);
      statement->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      statement = readStatement(source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readThrowStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"throw"}, source, current)) {
      return nullptr;
    }
    auto node = new JSThrowStatement{};

    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        auto val = readExpression(source, current);
        if (val) {
          if (val->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return val;
          }
          node->value = val;
          val->addParent(node);
        } else {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readTryStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"try"}, source, current)) {
      return nullptr;
    }
    auto node = new JSTryStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readBlockStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"catch"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto onerror = readTryCatchStatement(source, current);
      if (!onerror) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (onerror->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return onerror;
      }
      node->onerror = onerror;
      onerror->addParent(node);
    }
    if (checkIdentifier({L"finally"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto onfinish = readBlockStatement(source, current);
      if (!onfinish) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (onfinish->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return onfinish;
      }
      node->onfinish = onfinish;
      onfinish->addParent(node);
    }
    if (!node->onfinish && !node->onerror) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readTryCatchStatement(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    auto node = new JSTryCatchStatement{};
    if (checkSymbol({L"("}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readObjectPattern(source, current);
      }
      if (!identifier) {
        identifier = readArrayPattern(source, current);
      }
      if (!identifier) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->identifier = identifier;
      identifier->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L")"}, source, current)) {
        delete node;
        return err;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
    }
    auto body = readBlockStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readWhileStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"while"}, source, current)) {
      return nullptr;
    }
    auto node = new JSWhileStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    auto condition = readExpression(source, current);
    if (!condition) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (condition->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return condition;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }

    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readDoWhileStatement(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"do"}, source, current)) {
      return nullptr;
    }
    auto node = new JSDoWhileStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"while"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto condition = readExpression(source, current);
    if (!condition) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (condition->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return condition;
    }
    node->condition = condition;
    condition->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readForStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"for"}, source, current)) {
      return nullptr;
    }
    auto node = new JSForStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto init = readExpression(source, current);
    if (!init) {
      init = readVariableDeclaration(source, current);
    }
    if (init) {
      if (init->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return init;
      }
      node->init = init;
      init->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L";"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto condition = readExpression(source, current);
    if (condition) {
      if (condition->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return condition;
      }
      node->condition = condition;
      condition->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L";"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto after = readExpression(source, current);
    if (after) {
      if (after->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return after;
      }
      node->after = after;
      after->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, current, position);
    popScope();
    position = current;
    return node;
  }

  JSNode *readForInStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"for"}, source, current)) {
      return nullptr;
    }
    auto node = new JSForInStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto init = readExpression17(source, current);
    if (!init) {
      init = readVariableDeclaration(source, current);
    }
    if (!init) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (init->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return init;
    }
    node->left = init;
    init->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"in"}, source, current)) {
      popScope();
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto expr = readExpression(source, current);
    if (!expr) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (expr->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return expr;
    }
    node->right = expr;
    expr->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    popScope();
    position = current;
    return node;
  }

  JSNode *readForOfStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"for"}, source, current)) {
      return nullptr;
    }
    auto node = new JSForOfStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto init = readExpression17(source, current);
    if (!init) {
      init = readVariableDeclaration(source, current);
    }
    if (!init) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (init->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return init;
    }
    node->left = init;
    init->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"of"}, source, current)) {
      popScope();
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto expr = readExpression(source, current);
    if (!expr) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (expr->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return expr;
    }
    node->right = expr;
    expr->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readForAwaitOfStatement(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"for"}, source, current)) {
      return nullptr;
    }
    auto node = new JSForAwaitOfStatement{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"await"}, source, current)) {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto init = readExpression17(source, current);
    if (!init) {
      init = readVariableDeclaration(source, current);
    }
    if (!init) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (init->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return init;
    }
    node->left = init;
    init->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"of"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto expr = readExpression(source, current);
    if (!expr) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (expr->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return expr;
    }
    node->right = expr;
    expr->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readStatement(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readVariableDeclarationStatement(const std::wstring &source,
                                           JSPosition &position) {
    auto current = position;
    auto node = readVariableDeclaration(source, current);
    if (!node || node->type == JS_NODE_TYPE::ERROR) {
      return node;
    }
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readVariableDeclaration(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto node = new JSVariableDeclaraionNode{};
    if (checkIdentifier({L"const"}, source, current)) {
      node->kind = JS_DECLARATION_TYPE::CONST;
    } else if (checkIdentifier({L"let"}, source, current)) {
      node->kind = JS_DECLARATION_TYPE::LET;
    } else if (checkIdentifier({L"var"}, source, current)) {
      node->kind = JS_DECLARATION_TYPE::VAR;
    } else {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto declaration = readVariableDeclarator(source, current);
    if (!declaration) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    for (;;) {
      if (declaration->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return declaration;
      }
      node->declarations.push_back(declaration);
      declaration->addParent(node);
      auto err = declarVariable(node->kind, source, declaration);
      if (err) {
        delete node;
        return err;
      }
      auto backup = current;
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L","}, source, current)) {
        current = backup;
        break;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      declaration = readVariableDeclarator(source, current);
      if (!declaration) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readVariableDeclarator(const std::wstring &source,
                                 JSPosition &position) {
    auto current = position;
    auto node = new JSVariableDeclaratorNode{};
    auto identifier = readObjectPattern(source, current);
    if (!identifier) {
      identifier = readArrayPattern(source, position);
    }
    if (!identifier) {
      identifier = readIdentifyLiteral(source, current);
    }
    if (!identifier) {
      delete node;
      return nullptr;
    }
    node->identifier = identifier;
    identifier->addParent(node);
    auto backup = current;
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkSymbol({L"="}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto value = readExpression2(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->initialize = value;
      value->addParent(node);
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readImportNamespace(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"*"}, source, current)) {
      return nullptr;
    }
    auto node = new JSImportNamespaceNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"as"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto alias = readIdentifyLiteral(source, current);
    if (!alias) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->alias = alias;
    alias->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readImportDefault(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      return nullptr;
    }
    if (isKeyword(source, identifier->location)) {
      delete identifier;
      return nullptr;
    }
    auto node = new JSImportDefaultNode{};
    node->identifier = identifier;
    identifier->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readImportSpecifier(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      return nullptr;
    }
    if (identifier->type == JS_NODE_TYPE::ERROR) {
      return identifier;
    }
    if (isKeyword(source, identifier->location)) {
      delete identifier;
      return nullptr;
    }
    auto node = new JSImportSpecifierNode{};
    node->identifier = identifier;
    identifier->addParent(node);
    auto backup = current;
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"as"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto alias = readIdentifyLiteral(source, current);
      if (!alias) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      node->alias = alias;
      alias->addParent(node);
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readImportAttribute(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      return nullptr;
    }
    auto node = new JSImportAttributeNode{};
    node->key = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L":"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto value = readStringLiteral(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readImportDeclaration(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"import"}, source, current)) {
      return nullptr;
    }
    auto node = new JSImportDeclarationNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto src = readStringLiteral(source, current);
    if (src) {
      node->source = src;
      src->addParent(node);
    } else {
      auto specifier = readImportNamespace(source, current);
      if (specifier) {
        if (specifier->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return specifier;
        }
        node->specifiers.push_back(specifier);
        specifier->addParent(node);
      } else {
        specifier = readImportDefault(source, current);
        if (specifier) {
          if (specifier->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return specifier;
          }
          node->specifiers.push_back(specifier);
          specifier->addParent(node);
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (checkSymbol({L","}, source, current)) {
            auto backup = current;
            while (skipInvisible(source, current)) {
            }
            err = readComments(source, current, node->comments);
            if (err) {
              delete node;
              return err;
            }
            if (!checkSymbol({L"{"}, source, current)) {
              delete node;
              return createError(L"Invalid or unexpected token", source,
                                 current);
            } else {
              current = backup;
            }
          }
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          delete node;
          return err;
        }
        if (checkSymbol({L"{"}, source, current)) {
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          specifier = readImportSpecifier(source, current);
          if (specifier) {
            for (;;) {
              if (specifier->type == JS_NODE_TYPE::ERROR) {
                delete node;
                return specifier;
              }
              node->specifiers.push_back(specifier);
              specifier->addParent(node);
              while (skipInvisible(source, current)) {
              }
              err = readComments(source, current, node->comments);
              if (err) {
                delete node;
                return err;
              }
              if (!checkSymbol({L","}, source, current)) {
                break;
              }
              while (skipInvisible(source, current)) {
              }
              err = readComments(source, current, node->comments);
              if (err) {
                delete node;
                return err;
              }
              specifier = readImportSpecifier(source, current);
              if (!specifier) {
                delete node;
                return createError(L"Invalid or unexpected token", source,
                                   current);
              }
            }
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (!checkSymbol({L"}"}, source, current)) {
            delete node;
            return createError(L"Invalid or unexpected token", source, current);
          }
        }
      }

      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!node->specifiers.empty()) {
        if (!checkIdentifier({L"from"}, source, current)) {
          delete node;
          return createError(L"Invalid or unexpected token", source, current);
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          delete node;
          return err;
        }
      }
      auto src = readStringLiteral(source, current);
      if (!src) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (src->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return src;
      }
      node->source = src;
      src->addParent(node);
    }
    auto backup = current;
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"assert"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"{"}, source, current)) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto attribute = readImportAttribute(source, current);
      if (attribute) {
        for (;;) {
          if (attribute->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return attribute;
          }
          node->attributes.push_back(attribute);
          attribute->addParent(node);
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (!checkSymbol({L","}, source, current)) {
            break;
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          attribute = readImportAttribute(source, current);
          if (!attribute) {
            delete node;
            return createError(L"Invalid or unexpected token", source, current);
          }
        }
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"}"}, source, current)) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
    } else {
      current = backup;
    }
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    backup = current;
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        backup = current;
      } else {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
    }
    for (auto item : node->specifiers) {
      auto err = declarVariable(JS_DECLARATION_TYPE::CONST, source, item);
      if (err) {
        delete node;
        return err;
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExportSpecifier(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    auto node = new JSExportSpecifierNode{};
    auto left = readIdentifyLiteral(source, current);
    if (!left) {
      left = readStringLiteral(source, current);
    }
    if (!left) {
      delete node;
      return nullptr;
    }
    if (left->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return left;
    }
    node->identifier = left;
    left->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"as"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto identifier = readIdentifyLiteral(source, current);
      identifier = readStringLiteral(source, current);
      if (!identifier) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (identifier->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return identifier;
      }
      node->alias = identifier;
      identifier->addParent(node);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExportNamespaceSpecifier(const std::wstring &source,
                                       JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"*"}, source, current)) {
      return nullptr;
    }
    auto node = new JSExportNamespaceNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"as"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto alias = readIdentifyLiteral(source, current);
      if (!alias) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (alias->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return alias;
      }
      node->alias = alias;
      alias->addParent(node);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExportNamedSpecifier(const std::wstring &source,
                                   JSPosition &position) {
    auto current = position;
    auto node = new JSExportNamedNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      delete node;
      return nullptr;
    }
    node->declaration = identifier;
    identifier->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExportDefaultSpecifier(const std::wstring &source,
                                     JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"default"}, source, current)) {
      return nullptr;
    }
    auto node = new JSExportDefaultNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->expression = readExpression(source, current);
    if (!node->expression) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (node->expression->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return node->expression;
    }
    node->expression->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExportDeclaration(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"export"}, source, current)) {
      return nullptr;
    }
    auto node = new JSExportDeclarationNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto item = readFunctionDeclaration(source, current);
    if (!item) {
      item = readClassDeclaration(source, current);
    }
    if (!item) {
      item = readVariableDeclaration(source, current);
    }
    if (!item) {
      item = readExportDefaultSpecifier(source, current);
    }
    if (item) {
      node->specifiers.push_back(item);
      item->addParent(node);
    } else {
      auto specifier = readExportNamespaceSpecifier(source, current);
      if (specifier) {
        if (specifier->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return specifier;
        }
        node->specifiers.push_back(specifier);
        specifier->addParent(node);
      } else {
        specifier = readExportNamedSpecifier(source, current);
        if (specifier) {
          if (specifier->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return specifier;
          }
          node->specifiers.push_back(specifier);
          specifier->addParent(node);
          while (skipInvisible(source, current)) {
          }
          auto err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (checkSymbol({L","}, source, current)) {
            while (skipInvisible(source, current)) {
            }
            auto err = readComments(source, current, node->comments);
            if (err) {
              delete node;
              return err;
            }
            auto backup = current;
            if (!checkSymbol({L"{"}, source, current)) {
              delete node;
              return createError(L"Invalid or unexcepted token", source,
                                 current);
            } else {
              current = backup;
            }
          }
        }
        if (checkSymbol({L"{"}, source, current)) {
          auto specifier = readExportSpecifier(source, current);
          if (specifier) {
            for (;;) {
              if (specifier->type == JS_NODE_TYPE::ERROR) {
                delete node;
                return specifier;
              }
              while (skipInvisible(source, current)) {
              }
              auto err = readComments(source, current, node->comments);
              if (err) {
                delete node;
                return err;
              }
              if (!checkSymbol({L","}, source, current)) {
                break;
              }
              specifier = readExportSpecifier(source, current);
              if (!specifier) {
                delete node;
                return createError(L"Invalid or unexcepted token", source,
                                   current);
              }
            }
          }
          while (skipInvisible(source, current)) {
          }
          auto err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (!checkSymbol({L"}"}, source, current)) {
            delete node;
            return createError(L"Invalid or unexcepted token", source, current);
          }
        } else {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (checkIdentifier({L"from"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err) {
          delete node;
          return err;
        }
        auto src = readStringLiteral(source, current);
        if (!src) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
        if (src->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return src;
        }
        node->source = src;
        src->addParent(node);
        auto backup = current;
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          delete node;
          return err;
        }
        if (checkIdentifier({L"assert"}, source, current)) {
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (!checkSymbol({L"{"}, source, current)) {
            delete node;
            return createError(L"Invalid or unexpected token", source, current);
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          auto attribute = readImportAttribute(source, current);
          if (attribute) {
            for (;;) {
              if (attribute->type == JS_NODE_TYPE::ERROR) {
                delete node;
                return attribute;
              }
              node->attributes.push_back(attribute);
              attribute->addParent(node);
              while (skipInvisible(source, current)) {
              }
              err = readComments(source, current, node->comments);
              if (err) {
                delete node;
                return err;
              }
              if (!checkSymbol({L","}, source, current)) {
                break;
              }
              while (skipInvisible(source, current)) {
              }
              err = readComments(source, current, node->comments);
              if (err) {
                delete node;
                return err;
              }
              attribute = readImportAttribute(source, current);
              if (!attribute) {
                delete node;
                return createError(L"Invalid or unexpected token", source,
                                   current);
              }
            }
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          if (!checkSymbol({L"}"}, source, current)) {
            delete node;
            return createError(L"Invalid or unexpected token", source, current);
          }
        } else {
          current = backup;
        }
      }
    }
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        backup = current;
      } else {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    JSNode *node = readEmptyStatement(source, current);
    if (!node) {
      node = readVariableDeclarationStatement(source, current);
    }
    if (!node) {
      node = readBlockStatement(source, current);
    }
    if (!node) {
      node = readDebuggerStatement(source, current);
    }
    if (!node) {
      node = readReturnStatement(source, current);
    }
    if (!node) {
      node = readLabelStatement(source, current);
    }
    if (!node) {
      node = readBreakStatement(source, current);
    }
    if (!node) {
      node = readContinueStatement(source, current);
    }
    if (!node) {
      node = readIfStatement(source, current);
    }
    if (!node) {
      node = readSwitchStatement(source, current);
    }
    if (!node) {
      node = readThrowStatement(source, current);
    }
    if (!node) {
      node = readTryStatement(source, current);
    }
    if (!node) {
      node = readWhileStatement(source, current);
    }
    if (!node) {
      node = readDoWhileStatement(source, current);
    }
    if (!node) {
      node = readForAwaitOfStatement(source, current);
    }
    if (!node) {
      node = readForInStatement(source, current);
    }
    if (!node) {
      node = readForOfStatement(source, current);
    }
    if (!node) {
      node = readForStatement(source, current);
    }
    if (!node) {
      node = readImportDeclaration(source, current);
    }
    if (!node) {
      node = readFunctionDeclaration(source, current);
    }
    if (!node) {
      node = readClassDeclaration(source, current);
    }
    if (!node) {
      node = readExpressionStatement(source, current);
    }
    if (node) {
      position = current;
    }
    return node;
  }

  JSNode *readExpressionStatement(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto expression = readExpression(source, current);
    if (!expression || expression->type == JS_NODE_TYPE::ERROR) {
      return expression;
    }
    auto node = new JSExpressionStatementNode{};
    node->expression = expression;
    expression->addParent(node);
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return comment;
        }
        node->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      if (checkSymbol({L"}"}, source, current)) {
        backup = current;
      } else {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readObjectProperty(const std::wstring &source, JSPosition &position) {
    {
      auto node = readSpreadExpression(source, position);
      if (node) {
        return node;
      }
      node = readObjectMethod(source, position);
      if (node) {
        return node;
      }
      node = readObjectAccessor(source, position);
      if (node) {
        return node;
      }
    }
    auto current = position;
    auto node = new JSObjectPropertyNode{};
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->key = value;
      value->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      node->computed = true;
    } else {
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (!identifier) {
        delete node;
        return nullptr;
      }
      node->key = identifier;
      identifier->addParent(node);
    }
    if (checkSymbol({L":"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto val = readExpression2(source, current);
      if (!val) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      if (val->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return val;
      }
      node->value = val;
      val->addParent(node);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readObjectMethod(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto backup = current;
    auto node = new JSObjectMethodNode{};
    if (checkIdentifier({L"async"}, source, current)) {
      node->async = true;
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      if (checkSymbol({L"("}, source, current)) {
        current = backup;
        node->async = false;
      }
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"*"}, source, current)) {
      node->generator = true;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->key = value;
      value->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      node->computed = true;
    } else {
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (!identifier) {
        delete node;
        return nullptr;
      }
      node->key = identifier;
      identifier->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    auto argument = readFunctionArgument(source, current);
    while (argument) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
      if (err) {
        return err;
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L")"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      argument = readFunctionArgument(source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readObjectAccessor(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto node = new JSObjectAccessorNode{};
    if (checkIdentifier({L"get"}, source, current)) {
      node->kind = JS_ACCESSOR_TYPE::GET;
    } else if (checkIdentifier({L"set"}, source, current)) {
      node->kind = JS_ACCESSOR_TYPE::SET;
    } else {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }

    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->key = value;
      value->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        delete node;
        return createError(L"Invalid or nexcepted token", source, current);
      }
      node->computed = true;
    } else {
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (!identifier) {
        delete node;
        return nullptr;
      }
      node->key = identifier;
      identifier->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }

    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return nullptr;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto argument = readFunctionArgument(source, current);
    while (argument) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
      if (err) {
        return err;
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L")"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      argument = readFunctionArgument(source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readObjectDeclaration(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"{"}, source, current)) {
      return nullptr;
    }
    auto node = new JSObjectDeclarationNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto item = readObjectProperty(source, current);
    if (item) {
      for (;;) {
        if (item->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return item;
        }
        node->properties.push_back(item);
        item->addParent(node);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        auto backup = current;
        if (checkSymbol({L"}"}, source, current)) {
          current = backup;
          break;
        }
        if (!checkSymbol({L","}, source, current)) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        } else {
          auto backup = current;
          if (checkSymbol({L"}"}, source, current)) {
            current = backup;
            break;
          }
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        item = readObjectProperty(source, current);
        if (!item) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readClassProperty(const std::wstring &source, JSPosition &position) {
    {
      auto node = readEmptyStatement(source, position);
      if (!node) {
        node = readClassAccessor(source, position);
      }
      if (!node) {
        node = readClassMethod(source, position);
      }
      if (!node) {
        node = readStaticBlock(source, position);
      }
      if (node) {
        return node;
      }
    }
    auto current = position;
    JSNode *res = readSpreadExpression(source, current);
    if (!res) {
      auto backup = current;
      auto node = new JSClassPropertyNode{};
      if (checkIdentifier({L"static"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        auto backup2 = current;
        auto identifier = readIdentifyLiteral(source, current);
        if (!identifier) {
          current = backup;
        } else {
          delete identifier;
          node->static_ = true;
          current = backup2;
          while (skipInvisible(source, current)) {
          }
          auto err = readComments(source, current, node->comments);
          if (err != nullptr) {
            delete node;
            return err;
          }
        }
      }
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (!identifier) {
        if (checkSymbol({L"["}, source, current)) {
          while (skipInvisible(source, current)) {
          }
          auto err = readComments(source, current, node->comments);
          if (err != nullptr) {
            delete node;
            return err;
          }
          identifier = readExpression(source, current);
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err != nullptr) {
            delete node;
            return err;
          }
          if (!checkSymbol({L"]"}, source, current)) {
            delete node;
            return createError(L"Invalid or unexcepted token", source, current);
          }
          node->computed = true;
        }
      }
      if (!identifier) {
        delete node;
        return nullptr;
      }
      if (identifier->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return identifier;
      }
      node->identifier = identifier;
      identifier->addParent(node);
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      if (checkSymbol({L"="}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        auto value = readExpression(source, current);
        if (!value) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
        if (value->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return value;
        }
        node->value = value;
        value->addParent(node);
      }
      res = node;
    }
    auto newline = false;
    for (;;) {
      if (skipWhiteSpace(source, current)) {
        continue;
      }
      if (skipLineTerminator(source, current)) {
        newline = true;
        break;
      }
      bool isCommentNewline = false;
      auto comment = readComment(source, current, &isCommentNewline);
      if (comment) {
        if (comment->type == JS_NODE_TYPE::ERROR) {
          delete res;
          return comment;
        }
        res->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    auto backup = current;
    if (!checkSymbol({L";"}, source, current) && !newline) {
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
      } else {
        delete res;
        return createError(L"Invalid or unexpected token", source, current);
      }
    }
    res->location = getLocation(source, position, current);
    position = current;
    return res;
  }
  JSNode *readClassMethod(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto backup = current;
    auto node = new JSClassMethodNode{};
    if (checkIdentifier({L"static"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup2 = current;
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (identifier && identifier->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return identifier;
      }
      if (identifier || checkSymbol({L"*"}, source, current) ||
          checkSymbol({L"["}, source, current)) {
        current = backup2;
        node->static_ = true;
        delete identifier;
      } else {
        current = backup;
      }
    }
    backup = current;
    if (checkIdentifier({L"async"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup2 = current;
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
      if (identifier && identifier->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return identifier;
      }
      if (identifier || checkSymbol({L"*"}, source, current) ||
          checkSymbol({L"["}, source, current)) {
        current = backup2;
        node->async = true;
        delete identifier;
      } else {
        current = backup;
      }
    }
    if (checkSymbol({L"*"}, source, current)) {
      node->generator = true;
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
    }

    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      if (checkSymbol({L"["}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        identifier = readExpression(source, current);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        if (!checkSymbol({L"]"}, source, current)) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
        node->computed = true;
      }
    }
    if (!identifier) {
      delete node;
      return nullptr;
    }
    if (identifier->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return identifier;
    }
    node->identifier = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return nullptr;
    }
    auto argument = readFunctionArgument(source, current);
    if (argument) {
      for (;;) {
        if (argument->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return argument;
        }
        node->arguments.push_back(argument);
        argument->addParent(node);
        auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
        if (err) {
          return err;
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        if (!checkSymbol({L","}, source, current)) {
          break;
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        argument = readFunctionArgument(source, current);
        if (!argument) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readClassAccessor(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto node = new JSClassAccessorNode{};
    auto backup = current;
    if (checkIdentifier({L"static"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup2 = current;
      if (checkIdentifier({L"get", L"set"}, source, current)) {
        current = backup2;
        node->static_ = true;
      } else {
        current = backup;
      }
    }
    backup = current;
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"get"}, source, current)) {
      node->kind = JS_ACCESSOR_TYPE::GET;
    } else if (checkIdentifier({L"set"}, source, current)) {
      node->kind = JS_ACCESSOR_TYPE::SET;
    } else {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      if (checkSymbol({L"["}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        identifier = readExpression(source, current);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        if (!checkSymbol({L"]"}, source, current)) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
        node->computed = true;
      }
    }
    if (!identifier) {
      delete node;
      return nullptr;
    }
    if (identifier->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return identifier;
    }
    node->identifier = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return nullptr;
    }
    auto argument = readFunctionArgument(source, current);
    if (argument) {
      for (;;) {
        if (argument->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return argument;
        }
        node->arguments.push_back(argument);
        argument->addParent(node);
        auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
        if (err) {
          return err;
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        if (!checkSymbol({L","}, source, current)) {
          break;
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        argument = readFunctionArgument(source, current);
        if (!argument) {
          delete node;
          return createError(L"Invalid or unexcepted token", source, current);
        }
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readStaticBlock(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"static"}, source, current)) {
      return nullptr;
    }
    auto node = new JSStaticBlockNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    auto block = readBlockStatement(source, current);
    if (!block) {
      popScope();
      delete node;
      return nullptr;
    }
    if (block->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return block;
    }
    popScope();
    node->statement = block;
    block->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readClassDeclaration(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"class"}, source, current)) {
      return nullptr;
    }
    auto node = new JSClassDeclaration{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto backup = current;
    auto identifier = readIdentifyLiteral(source, current);
    if (identifier) {
      if (identifier->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return identifier;
      }
      if (isKeyword(source, identifier->location)) {
        delete identifier;
        current = backup;
        identifier = nullptr;
      }
    }
    if (identifier) {
      node->identifier = identifier;
      identifier->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkIdentifier({L"extends"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto extends = readExpression17(source, current);
      if (!extends) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->extends = extends;
      extends->addParent(node);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    if (!checkSymbol({L"{"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto property = readClassProperty(source, current);
    if (property) {
      for (;;) {
        if (property->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return property;
        }
        if (property->type == JS_NODE_TYPE::STATEMENT_EMPTY) {
          delete property;
        } else {
          node->properties.push_back(property);
          property->addParent(node);
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        property = readClassProperty(source, current);
        if (!property) {
          break;
        }
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    err = declarVariable(JS_DECLARATION_TYPE::CONST, source, node);
    if (err) {
      delete node;
      return err;
    }
    return node;
  }

  JSNode *readArrayDeclaration(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"["}, source, current)) {
      return nullptr;
    }
    auto node = new JSArrayDeclarationNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"]"}, source, current)) {
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    auto item = readExpression2(source, current);
    if (!item) {
      item = readSpreadExpression(source, current);
    }
    for (;;) {
      if (item) {
        if (item->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return item;
        }
        item->addParent(node);
      }
      node->items.push_back(item);
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L"]"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      item = readExpression2(source, current);
      if (!item) {
        item = readSpreadExpression(source, current);
      }
    }
    if (!checkSymbol({L"]"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readSpreadPattern(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"..."}, source, current)) {
      return nullptr;
    }
    auto node = new JSSpreadPatternItemNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    auto value = readPattern(source, current);
    if (!value) {
      delete node;
      return nullptr;
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readObjectPatternItem(const std::wstring &source,
                                JSPosition &position) {
    auto current = position;
    auto node = new JSObjectPatternItemNode{};
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto field = readExpression(source, current);
      if (!field) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (field->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return field;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->key = field;
      field->addParent(node);
      node->computed = true;
    } else {
      auto identifier = readPartPattern(source, current);
      if (identifier) {
        node->key = identifier;
        identifier->addParent(node);
      }
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (node->key) {
      if (checkSymbol({L":"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err != nullptr) {
          delete node;
          return err;
        }
        auto pattern = readPattern(source, current);
        if (!pattern) {
          delete node;
          return nullptr;
        }
        if (pattern->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return pattern;
        }
        node->alias = pattern;
        pattern->addParent(node);
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"="}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression2(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->value = value;
      value->addParent(node);
    }

    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readObjectPattern(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"{"}, source, current)) {
      return nullptr;
    }
    auto node = new JSObjectPatternNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"}"}, source, current)) {
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    auto item = readObjectPatternItem(source, current);

    for (;;) {
      if (!item) {
        delete node;
        return nullptr;
      }
      if (item->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return item;
      }
      node->items.push_back(item);
      item->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return nullptr;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      item = readObjectPatternItem(source, current);
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return nullptr;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readArrayPatternItem(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    auto node = new JSArrayPatternItemNode{};
    auto identifier = readPartPattern(source, current);
    if (!identifier || identifier->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return identifier;
    }
    node->alias = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"="}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression2(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->value = value;
      value->addParent(node);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readArrayPattern(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"["}, source, current)) {
      return nullptr;
    }
    auto node = new JSArrayPatternNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"]"}, source, current)) {
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    auto item = readArrayPatternItem(source, current);
    for (;;) {
      if (item) {
        if (item->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return item;
        }
        item->addParent(node);
      }
      node->items.push_back(item);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L"]"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return nullptr;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      item = readArrayPatternItem(source, current);
    }
    if (!checkSymbol({L"]"}, source, current)) {
      delete node;
      return nullptr;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readPattern(const std::wstring &source, JSPosition &position) {
    auto node = readObjectPattern(source, position);
    if (!node) {
      node = readArrayPattern(source, position);
    }
    if (!node) {
      node = readExpression17(source, position);
    }
    return node;
  }

  JSNode *readPartPattern(const std::wstring &source, JSPosition &position) {
    auto node = readPattern(source, position);
    if (!node) {
      node = readSpreadPattern(source, position);
    }
    return node;
  }

  JSNode *readFunctionArgument(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    auto node = new JSFunctionArgumentDeclarationNode{};
    auto identifier = readPartPattern(source, current);
    if (!identifier) {
      delete node;
      return nullptr;
    }
    node->identifier = identifier;
    identifier->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      delete node;
      return err;
    }
    if (checkSymbol({L"="}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto value = readExpression3(source, current);
      if (!value) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->value = value;
      value->addParent(node);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readFunctionBodyDeclaration(const std::wstring &source,
                                      JSPosition &position) {
    auto current = position;
    auto node = new JSFunctionBodyDeclarationNode{};
    if (!checkSymbol({L"{"}, source, current)) {
      delete node;
      return nullptr;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
    for (;;) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto directive = readDirective(source, current);
      if (!directive) {
        break;
      }
      if (directive->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return directive;
      }
      node->directives.push_back(directive);
      directive->addParent(node);
    }
    for (;;) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto statement = readStatement(source, current);
      if (!statement) {
        break;
      }
      if (statement->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return statement;
      }
      node->statements.push_back(statement);
      statement->addParent(node);
    }
    if (!checkSymbol({L"}"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    popScope();
    return node;
  }

  JSNode *readFunctionDeclaration(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto node = new JSFunctionDeclarationNode{};
    if (checkIdentifier({L"async"}, source, current)) {
      node->async = true;
    }
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkIdentifier({L"function"}, source, current)) {
      delete node;
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkSymbol({L"*"}, source, current)) {
      node->generator = true;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (identifier) {
      node->identifier = identifier;
      identifier->addParent(node);
      if (isKeyword(source, identifier->location)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }

    if (!checkSymbol({L"("}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    auto argument = readFunctionArgument(source, current);
    while (argument) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
      if (err) {
        return err;
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L")"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        delete node;
        return createError(L"Invalid or unexcepted token", source, current);
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      argument = readFunctionArgument(source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (body->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return body;
    }
    node->body = body;
    body->addParent(node);
    popScope();
    node->location = getLocation(source, position, current);
    position = current;
    err = declarVariable(JS_DECLARATION_TYPE::FUNCTION, source, node);
    if (err) {
      delete node;
      return err;
    }
    return node;
  }

  JSNode *readArrowFunctionDeclaration(const std::wstring &source,
                                       JSPosition &position) {
    auto current = position;
    auto node = new JSArrowDeclarationNode;
    node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
    auto backup = current;
    if (checkIdentifier({L"async"}, source, current)) {
      node->async = true;
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (checkSymbol({L"=>"}, source, current)) {
        node->async = false;
        current = backup;
      }
    }
    if (checkSymbol({L"("}, source, current)) {
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto arg = readFunctionArgument(source, current);
      if (arg) {
        for (;;) {
          if (arg->type == JS_NODE_TYPE::ERROR) {
            delete node;
            return arg;
          }
          node->arguments.push_back(arg);
          arg->addParent(node);
          auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, arg);
          if (err) {
            return err;
          }
          while (skipInvisible(source, current)) {
          };
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          auto backup = current;
          if (checkSymbol({L")"}, source, current)) {
            current = backup;
            break;
          }
          if (!checkSymbol({L","}, source, current)) {
            popScope();
            delete node;
            return nullptr;
          }
          while (skipInvisible(source, current)) {
          };
          err = readComments(source, current, node->comments);
          if (err) {
            delete node;
            return err;
          }
          arg = readFunctionArgument(source, current);
          if (!arg) {
            popScope();
            delete node;
            return nullptr;
          }
        }
      }

      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L")"}, source, current)) {
        popScope();
        delete node;
        return nullptr;
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"=>"}, source, current)) {
        popScope();
        delete node;
        return nullptr;
      }
    } else {
      auto identifier = readIdentifyLiteral(source, current);
      if (identifier == nullptr) {
        popScope();
        delete node;
        return nullptr;
      }
      auto arg = new JSFunctionArgumentDeclarationNode{};
      arg->identifier = identifier;
      arg->location = identifier->location;
      identifier->addParent(arg);
      node->arguments.push_back(arg);
      arg->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, arg);
      if (err) {
        return err;
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"=>"}, source, current)) {
        popScope();
        delete node;
        return nullptr;
      }
    }
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto body = readFunctionBodyDeclaration(source, current);
    if (!body) {
      body = readExpression2(source, current);
    }
    if (!body) {
      delete node;
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->body = body;
    body->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    popScope();
    return node;
  }

  JSNode *readExpression19(const std::wstring &source, JSPosition &position) {
    auto node = readStringLiteral(source, position);
    if (!node) {
      node = readNumberLiteral(source, position);
    }
    if (!node) {
      node = readRegexpLiteral(source, position);
    }
    if (!node) {
      node = readBooleanLiteral(source, position);
    }
    if (!node) {
      node = readNullLiteral(source, position);
    }
    if (!node) {
      node = readUndefinedLiteral(source, position);
    }
    if (!node) {
      node = readThisLiteral(source, position);
    }
    if (!node) {
      node = readSuperLiteral(source, position);
    }
    if (!node) {
      node = readObjectDeclaration(source, position);
    }
    if (!node) {
      node = readArrayDeclaration(source, position);
    }
    if (!node) {
      node = readFunctionDeclaration(source, position);
    }
    if (!node) {
      node = readClassDeclaration(source, position);
    }
    if (!node) {
      node = readTemplateLiteral(source, position);
    }
    if (!node) {
      auto current = position;
      node = readIdentifyLiteral(source, current);
      if (node) {
        if (isKeyword(source, node->location)) {
          delete node;
          node = nullptr;
        } else {
          position = current;
        }
      }
    }
    return node;
  }

  JSNode *readGroupExpression(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (checkSymbol({L"("}, source, current)) {
      auto node = new JSGroupExpressionNode{};
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto expr = readExpression(source, current);
      if (!expr) {
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (expr->type == JS_NODE_TYPE::ERROR) {
        return expr;
      }
      node->expression = expr;
      expr->addParent(node);
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L")"}, source, current)) {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readExpression18(const std::wstring &source, JSPosition &position) {
    auto node = readGroupExpression(source, position);
    if (!node) {
      node = readExpression19(source, position);
    }
    return node;
  }

  JSNode *readNewCallExpression(const std::wstring &source,
                                JSPosition &position) {

    auto current = position;
    if (!checkIdentifier({L"new"}, source, current)) {
      return nullptr;
    }
    auto node = new JSNewExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto callee = readIdentifyLiteral(source, current);
    if (!callee) {
      callee = readGroupExpression(source, current);
    }
    if (!callee) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (callee->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return callee;
    }
    for (;;) {
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        delete callee;
        return err;
      }
      auto next = readCallExpression(source, current, callee);
      if (next) {
        auto call = dynamic_cast<JSCallExpressionNode *>(next);
        node->callee = call->callee;
        node->arguments = call->arguments;
        node->comments = call->comments;
        node->children = call->children;
        call->children.clear();
        call->comments.clear();
        delete call;
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
      next = readOptionalMemberExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid optional chain from new expression",
                           source, current);
      }
      next = readOptionalComputedMemberExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid optional chain from new expression",
                           source, current);
      }
      next = readOptionalCallExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid optional chain from new expression",
                           source, current);
      }
      next = readMemberExpression(source, current, callee);
      if (!next) {
        next = readComputedMemberExpression(source, current, callee);
      }
      if (next) {
        callee = next;
      }
      if (!next) {
        delete callee;
        delete node;
        return nullptr;
      }
    }
    return nullptr;
  }

  JSNode *readMemberExpression(const std::wstring &source, JSPosition &position,
                               JSNode *left) {
    auto current = position;
    if (!checkSymbol({L"."}, source, current)) {
      return nullptr;
    }
    auto node = new JSMemberExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      delete node;
      return createError(L"Unexpected end of input", source, current);
    }
    if (isKeyword(source, identifier->location)) {
      delete identifier;
      delete node;
      return createError(L"Invalid or unexpected token", source,
                         identifier->location.start);
    }
    node->host = left;
    left->addParent(node);
    node->field = identifier;
    identifier->addParent(node);
    node->location = getLocation(source, left->location.start, current);
    position = current;
    return node;
  }

  JSNode *readComputedMemberExpression(const std::wstring &source,
                                       JSPosition &position, JSNode *left) {
    auto current = position;
    if (!checkSymbol({L"["}, source, current)) {
      return nullptr;
    }
    auto node = new JSComputedMemberExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto field = readExpression(source, current);
    if (!field) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (field->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return field;
    }
    node->field = field;
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L"]"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->host = left;
    left->addParent(node);
    field->addParent(node);
    node->location = getLocation(source, left->location.start, current);
    position = current;
    return node;
  }

  JSNode *readSpreadExpression(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    if (!checkSymbol({L"..."}, source, current)) {
      return nullptr;
    }
    auto value = readExpression2(source, current);
    if (!value) {
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      return value;
    }
    auto node = new JSSpreadExpressionNode{};
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readCallExpression(const std::wstring &source, JSPosition &position,
                             JSNode *left) {
    auto current = position;
    if (!checkSymbol({L"("}, source, current)) {
      return nullptr;
    }
    auto node = new JSCallExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto arg = readExpression2(source, current);
    if (!arg) {
      arg = readSpreadExpression(source, current);
    }
    if (arg && arg->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return arg;
    }
    while (arg) {
      node->arguments.push_back(arg);
      arg->addParent(node);
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      auto backup = current;
      if (checkSymbol({L","}, source, current)) {
      } else if (checkSymbol({L")"}, source, current)) {
        current = backup;
        break;
      } else {
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      while (skipInvisible(source, current)) {
      };
      err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      arg = readExpression2(source, current);
      if (!arg) {
        arg = readSpreadExpression(source, current);
      }
      if (arg && arg->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return arg;
      }
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->callee = left;
    left->addParent(node);
    node->location = getLocation(source, left->location.start, current);
    position = current;
    return node;
  }

  JSNode *readOptionalMemberExpression(const std::wstring &source,
                                       JSPosition &position, JSNode *left) {
    auto current = position;

    if (!checkSymbol({L"?."}, source, current)) {
      return nullptr;
    }
    auto node = new JSOptionalMemberExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      delete node;
      return identifier;
    }
    node->field = identifier;
    identifier->addParent(node);
    node->host = left;
    left->addParent(node);
    node->location = getLocation(source, left->location.start, current);
    position = current;
    return node;
  }

  JSNode *readOptionalComputedMemberExpression(const std::wstring &source,
                                               JSPosition &position,
                                               JSNode *left) {
    auto current = position;

    if (!checkSymbol({L"?."}, source, current)) {

      return nullptr;
    }
    auto computed = readComputedMemberExpression(source, current, left);
    if (!computed || computed->type == JS_NODE_TYPE::ERROR) {
      return computed;
    }
    auto node = new JSOptionalMemberExpressionNode;
    auto computedNode =
        dynamic_cast<JSComputedMemberExpressionNode *>(computed);
    node->host = computedNode->host;
    node->field = computedNode->field;
    node->children = computedNode->children;
    node->comments = computedNode->comments;
    node->location = computedNode->location;
    computedNode->children.clear();
    computedNode->comments.clear();
    delete computedNode;
    position = current;
    return node;
  }

  JSNode *readOptionalCallExpression(const std::wstring &source,
                                     JSPosition &position, JSNode *left) {
    auto current = position;

    if (!checkSymbol({L"?."}, source, current)) {
      return nullptr;
    }
    auto call = readCallExpression(source, current, left);
    if (!call || call->type == JS_NODE_TYPE::ERROR) {
      return call;
    }
    auto node = new JSCallExpressionNode;
    auto callNode = dynamic_cast<JSCallExpressionNode *>(call);
    node->callee = callNode->callee;
    node->arguments = callNode->arguments;
    node->children = callNode->children;
    node->comments = callNode->comments;
    node->location = callNode->location;
    callNode->children.clear();
    callNode->comments.clear();
    delete callNode;
    position = current;
    return node;
  }

  JSNode *readExpression17(const std::wstring &source, JSPosition &position) {
    auto node = readNewCallExpression(source, position);
    if (!node) {
      node = readExpression18(source, position);
    }
    if (node) {
      if (node->type == JS_NODE_TYPE::LITERAL_IDENTITY) {
        auto name = node->location.get(source);
        _scope->refs.insert(name);
      }
      auto current = position;
      bool optional = false;
      for (;;) {
        while (skipInvisible(source, current)) {
        };
        auto err = readComments(source, current, node->comments);
        if (err) {
          delete node;
          return err;
        }
        JSNode *next = readCallExpression(source, current, node);
        if (!next) {
          next = readMemberExpression(source, current, node);
        }
        if (!next) {
          next = readComputedMemberExpression(source, current, node);
        }
        if (!next) {
          next = readOptionalMemberExpression(source, current, node);
          if (next) {
            optional = true;
          }
        }
        if (!next) {
          next = readOptionalComputedMemberExpression(source, current, node);
          if (next) {
            optional = true;
          }
        }
        if (!next) {
          next = readOptionalCallExpression(source, current, node);
          if (next) {
            optional = true;
          }
        }
        if (!next) {
          next = readTemplateLiteral(source, current, node);
          if (next && next->type != JS_NODE_TYPE::ERROR) {
            if (optional) {
              current = next->location.start;
              delete next;
              return createError(L"Invalid tagged template on optional chain",
                                 source, current);
            }
          }
        }
        if (!next) {
          break;
        }
        if (next->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return next;
        }
        node = next;
        position = current;
      }
    }
    return node;
  }
  JSNode *readNewExpression(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"new"}, source, current)) {

      return nullptr;
    }

    auto node = new JSNewExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto callee = readIdentifyLiteral(source, current);
    if (callee && isKeyword(source, callee->location)) {
      current = callee->location.start;
      delete callee;
      callee = nullptr;
    }
    if (!callee) {
      callee = readGroupExpression(source, current);
    }
    if (!callee) {
      callee = readNewCallExpression(source, current);
    }
    if (!callee) {
      callee = readNewExpression(source, current);
    }
    if (!callee) {
      delete node;
      return createError(L"Invalid optional chain from new expression", source,
                         current);
    }
    if (callee->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return callee;
    }
    for (;;) {
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        delete callee;
        return err;
      }
      auto next = readCallExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return nullptr;
      }
      next = readOptionalMemberExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      next = readOptionalComputedMemberExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      next = readOptionalCallExpression(source, current, callee);
      if (next) {
        delete next;
        delete node;
        return createError(L"Invalid or unexpected token", source, current);
      }
      next = readMemberExpression(source, current, callee);
      if (!next) {
        next = readComputedMemberExpression(source, current, callee);
      }
      if (next) {
        callee = next;
      }
      if (!next) {
        break;
      }
    }
    node->callee = callee;
    callee->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExpression16(const std::wstring &source, JSPosition &position) {
    auto node = readNewExpression(source, position);
    if (!node) {
      node = readExpression17(source, position);
    }
    return node;
  }

  JSNode *readExpression15(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression16(source, position);
    if (node && node->type == JS_NODE_TYPE::ERROR) {
      return node;
    }
    if (node) {
      auto current = position;
      auto newline = false;
      auto n = new JSBinaryExpressionNode{};
      for (;;) {
        if (skipWhiteSpace(source, current)) {
          continue;
        }
        if (skipLineTerminator(source, current)) {
          newline = true;
          break;
        }
        auto comment = readComment(source, current);
        if (comment) {
          if (comment->type == JS_NODE_TYPE::ERROR) {
            delete n;
            delete node;
            return comment;
          }
          n->comments.push_back(comment);
          continue;
        }
        break;
      }
      JSNode *token = nullptr;
      if (!newline && checkSymbol({L"++", L"--"}, source, current, &token)) {
        n->opt = token;
        token->addParent(n);
        n->left = node;
        node->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readTypeofExpression(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    JSNode *token = nullptr;
    if (!checkIdentifier({L"typeof"}, source, current, &token)) {
      return nullptr;
    }
    auto node = new JSTypeofExpressioNode{};
    token->addParent(node);
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto value = readExpression14(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readVoidExpression(const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"void"}, source, current)) {
      return nullptr;
    }
    auto node = new JSVoidExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto value = readExpression14(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readDeleteExpression(const std::wstring &source,
                               JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"delete"}, source, current)) {
      return nullptr;
    }
    auto node = new JSDeleteExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto value = readExpression14(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readAwaitExpression(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"await"}, source, current)) {
      return nullptr;
    }
    auto node = new JSAwaitExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto value = readExpression14(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExpression14(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readTypeofExpression(source, position);
    if (!node) {
      node = readVoidExpression(source, position);
    }
    if (!node) {
      node = readDeleteExpression(source, position);
    }
    if (!node) {
      node = readAwaitExpression(source, position);
    }
    if (!node) {
      auto current = position;
      JSNode *token = nullptr;
      if (checkSymbol({L"~", L"!", L"+", L"-", L"++", L"--"}, source, current,
                      &token)) {
        auto n = new JSBinaryExpressionNode{};
        n->opt = token;
        token->addParent(n);
        auto value = readExpression14(source, current);
        if (!value) {
          delete node;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (value->type == JS_NODE_TYPE::ERROR) {
          delete node;
          return value;
        }
        n->right = value;
        value->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete node;
        node = nullptr;
      }
    }
    if (!node) {
      node = readExpression15(source, position);
    }
    return node;
  }

  JSNode *readExpression13(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression14(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"**"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression13(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression12(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression13(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"*", L"/", L"%"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression12(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression11(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression12(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"+", L"-"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression11(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression10(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression11(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"<<", L">>", L">>>"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression10(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression9(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression10(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"<", L"<=", L">", L">="}, source, current, &token) ||
          checkIdentifier({L"in", L"instanceof"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression9(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression8(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression9(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"==", L"!=", L"===", L"!=="}, source, current,
                      &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression8(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression7(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression8(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"&"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression7(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression6(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression7(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"^"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression6(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression5(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression6(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"|"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression5(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression4(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression5(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"&&"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression4(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression3(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression4(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L"||", L"??"}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression3(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readYieldExpression(const std::wstring &source,
                              JSPosition &position) {
    auto current = position;
    if (!checkIdentifier({L"yield"}, source, current)) {
      return nullptr;
    }
    auto node = new JSYieldExpressionNode{};
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (checkSymbol({L"*"}, source, current)) {
      auto n = new JSYieldDelegateExpressionNode;
      n->comments = node->comments;
      node->comments.clear();
      delete node;
      node = n;
    }
    auto value = readExpression2(source, current);
    if (!value) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return value;
    }
    node->value = value;
    value->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readConditionExpression(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto condition = readExpression3(source, current);
    if (!condition || condition->type == JS_NODE_TYPE::ERROR) {
      return condition;
    }
    auto backup = current;
    auto node = new JSConditionExpressionNode{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      delete condition;
      return err;
    }
    if (!checkSymbol({L"?"}, source, current)) {
      delete node;
      position = backup;
      return condition;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto consequent = readExpression2(source, current);
    if (!consequent) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (consequent->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return consequent;
    }
    node->consequent = consequent;
    consequent->addParent(node);

    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    if (!checkSymbol({L":"}, source, current)) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }

    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto alternate = readExpression2(source, current);
    if (!alternate) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (alternate->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return alternate;
    }
    node->alternate = alternate;
    alternate->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readAssigmentExpression(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto left = readPattern(source, current);
    if (!left || left->type == JS_NODE_TYPE::ERROR) {
      return left;
    }
    auto node = new JSAssigmentExpressionNode{};
    node->left = left;
    left->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    JSNode *token = nullptr;
    if (!checkSymbol({L"=", L"+=", L"-=", L"**=", L"*=", L"/=", L"%=", L"<<=",
                      L">>=", L">>>=", L"&=", L"|=", L"^=", L"&&=", L"||=",
                      LR"(??=)"},
                     source, current, &token)) {
      delete node;
      return nullptr;
    }
    node->opt = token;
    token->addParent(node);
    err = checkAssigment(left, source);
    if (err) {
      delete node;
      return err;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
    auto right = readExpression2(source, current);
    if (!right) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (right->type == JS_NODE_TYPE::ERROR) {
      delete node;
      return right;
    }
    node->right = right;
    right->addParent(node);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readExpression2(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readYieldExpression(source, position);
    if (!node) {
      node = readArrowFunctionDeclaration(source, position);
    }
    if (!node) {
      node = readAssigmentExpression(source, position);
    }
    if (!node) {
      node = readConditionExpression(source, position);
    }
    return node;
  }

  JSNode *readExpression1(const std::wstring &source, JSPosition &position) {
    auto base = position;
    auto node = readExpression2(source, position);
    if (node && node->type != JS_NODE_TYPE::ERROR) {
      auto current = position;
      auto n = new JSBinaryExpressionNode{};
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        delete n;
        delete node;
        return err;
      }
      JSNode *token = nullptr;
      if (checkSymbol({L","}, source, current, &token)) {
        n->left = node;
        node->addParent(n);
        n->opt = token;
        token->addParent(n);
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, n->comments);
        if (err) {
          delete n;
          delete node;
          return err;
        }
        auto right = readExpression1(source, current);
        if (!right) {
          delete n;
          return createError(L"Invalid or unexpected token", source, current);
        }
        if (right->type == JS_NODE_TYPE::ERROR) {
          delete n;
          return right;
        }
        n->right = right;
        right->addParent(n);
        n->location = getLocation(source, base, current);
        position = current;
        node = n;
      } else {
        delete n;
      }
    }
    return node;
  }

  JSNode *readExpression(const std::wstring &source, JSPosition &position) {
    auto node = readExpression1(source, position);
    return node;
  }

  JSNode *readComment(const std::wstring &source, JSPosition &position,
                      bool *newline = nullptr) {
    if (source[position.offset] == '/') {
      if (source[position.offset + 1] == '/') {
        auto current = position;
        while (!isLineTerminatior(source[current.offset])) {
          current.offset++;
        }
        auto node = new JSCommentLiteralNode();
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      } else if (source[position.offset + 1] == '*') {
        auto current = position;
        current.offset += 2;
        for (;;) {
          if (source[current.offset] == '*' &&
              source[current.offset + 1] == '/') {
            current.offset += 2;
            break;
          }
          if (isLineTerminatior(source[current.offset])) {
            if (newline) {
              *newline = true;
            }
          }
          if (!source[current.offset]) {
            return createError(L"Invalid or unexpected token", source, current);
          }
          current.offset++;
        }
        auto node = new JSMultilineCommentLiteralNode();
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
    return nullptr;
  }

public:
  virtual JSNode *parse(const std::wstring &source) {
    JSPosition pos = {};
    auto node = readProgram(source, pos);
    resolveBinding(source, node);
    return node;
  }
  virtual ~JSParser() {}
};

class JSScope;

class JSAtom;

class JSBase {
public:
  JSBase() {}
  virtual ~JSBase() {}

  virtual inline JSValue *toString(JSContext *ctx) = 0;
};

class JSUninitialize : public JSBase {
public:
  JSValue *toString(JSContext *ctx) { return nullptr; }
};

class JSAtom {
private:
  static inline std::vector<JSAtom *> _destroyed = {};

private:
  std::vector<JSAtom *> _parents;
  std::vector<JSAtom *> _children;
  JS_TYPE _type;
  JSBase *_data;

public:
  JSAtom(JSAtom *parent, const JS_TYPE &type, JSBase *data)
      : _type(type), _data(data) {
    if (parent) {
      parent->addChild(this);
    }
  }

  JSAtom() : _type(JS_TYPE::INTERNAL), _data(nullptr) {}

  ~JSAtom() {
    if (_data != nullptr) {
      delete _data;
      _data = nullptr;
    }
    while (!_children.empty()) {
      removeChild(*_children.begin());
    }
  }

  void addChild(JSAtom *child) {
    _children.push_back(child);
    child->_parents.push_back(this);
  }

  void removeChild(JSAtom *child) {
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
      _children.erase(it);
    }
    it = std::find(child->_parents.begin(), child->_parents.end(), this);
    if (it != child->_parents.end()) {
      child->_parents.erase(it);
    }
    JSAtom::_destroyed.push_back(child);
  }

  const JSBase *getData() const { return _data; }

  JSBase *getData() { return _data; }

  void setData(JSBase *data) {
    if (_data && _data != data) {
      delete _data;
    }
    _data = data;
  }

  const JS_TYPE &getType() const { return _type; }

  std::wstring getTypeName() const {
    switch (_type) {
    case JS_TYPE::UNDEFINED:
      return L"undefined";
    case JS_TYPE::NUMBER:
      return L"number";
    case JS_TYPE::STRING:
      return L"string";
    case JS_TYPE::BOOLEAN:
      return L"boolean";
    case JS_TYPE::BIGINT:
      return L"bigint";
    case JS_TYPE::OBJECT:
      return L"object";
    case JS_TYPE::FUNCTION:
      return L"function";
    case JS_TYPE::SYMBOL:
      return L"symbol";
    case JS_TYPE::CLASS:
      return L"function";
    default:
      break;
    }
    return L"unknown";
  }

public:
  static void gc() {
    std::vector<JSAtom *> alived = {};
    std::vector<JSAtom *> disposed = {};
    while (!_destroyed.empty()) {
      auto item = *_destroyed.rbegin();
      _destroyed.pop_back();
      if (std::find(alived.begin(), alived.end(), item) != alived.end()) {
        continue;
      }
      if (std::find(disposed.begin(), disposed.end(), item) != disposed.end()) {
        continue;
      }
      std::vector<JSAtom *> workflow = {item};
      std::vector<JSAtom *> cache = {};
      for (;;) {
        if (workflow.empty()) {
          disposed.push_back(item);
          while (!item->_children.empty()) {
            auto child = *item->_children.begin();
            item->removeChild(child);
          }
          while (!item->_parents.empty()) {
            auto parent = *item->_parents.begin();
            std::erase(parent->_children, item);
            item->_parents.erase(item->_parents.begin());
          }
          break;
        }
        auto it = *workflow.begin();
        workflow.erase(workflow.begin());
        if (std::find(cache.begin(), cache.end(), it) != cache.end()) {
          continue;
        }
        cache.push_back(it);
        if (std::find(disposed.begin(), disposed.end(), it) != disposed.end()) {
          continue;
        }
        if (std::find(alived.begin(), alived.end(), it) != alived.end()) {
          continue;
        }
        auto alive = false;
        for (auto parent : it->_parents) {
          if (parent->_data == nullptr) {
            alive = true;
            break;
          }
          workflow.push_back(parent);
        }
        if (alive) {
          alived.push_back(it);
          break;
        }
      }
    }
    for (auto dis : disposed) {
      delete dis;
    }
  }
};

class JSValue {
private:
  JSAtom *_atom;
  bool _const;

public:
  JSValue(JSAtom *atom) : _atom(atom), _const(false) {}

  ~JSValue() {}

  JSAtom *getAtom() { return _atom; }

  const JSAtom *getAtom() const { return _atom; }

  void setAtom(JSAtom *atom) { _atom = atom; }

  JSBase *getData() { return _atom->getData(); }

  const JSBase *getData() const { return _atom->getData(); }

  const JS_TYPE &getType() const { return _atom->getType(); }

  bool isConst() const { return _const; }

  void setConst(bool value) { _const = value; }
};

class JSScope {
private:
  JSScope *_parent;
  JSAtom *_root;
  std::vector<JSScope *> _children;
  std::vector<JSValue *> _variables;
  std::unordered_map<std::wstring, JSValue *> _namedVariables;

public:
  JSScope(JSScope *parent = nullptr) : _parent(parent) {
    if (_parent) {
      _parent->_children.push_back(this);
    }
    _root = new JSAtom();
  }

  ~JSScope() {
    _parent = nullptr;
    for (auto child : _children) {
      delete child;
    }
    _children.clear();
    for (auto variable : _variables) {
      delete variable;
    }
    _variables.clear();
    delete _root;
    _root = nullptr;
    JSAtom::gc();
  }

  JSScope *getParent() { return _parent; }

  void removeChild(JSScope *scope) {
    auto it = std::find(_children.begin(), _children.end(), scope);
    if (it != _children.end()) {
      _children.erase(it);
    }
    scope->_parent = nullptr;
  }

  JSAtom *createAtom(const JS_TYPE &type, JSBase *data) {
    return new JSAtom(_root, type, data);
  }

  JSValue *createValue(JSAtom *atom) {
    _root->addChild(atom);
    JSValue *value = nullptr;
    value = new JSValue(atom);
    _variables.push_back(value);
    return value;
  }

  JSValue *createValue(const JS_TYPE &type, JSBase *val) {
    auto atom = createAtom(type, val);
    JSValue *value = nullptr;
    value = new JSValue(atom);
    _variables.push_back(value);
    return value;
  }

  JSValue *queryValue(const std::wstring &name) {
    if (_namedVariables.contains(name)) {
      return _namedVariables.at(name);
    }
    return nullptr;
  }

  void storeValue(const std::wstring &name, JSValue *value) {
    _namedVariables[name] = value;
  }
};

using JS_NATIVE =
    std::function<JSValue *(JSContext *, JSValue *, std::vector<JSValue *>)>;

class JSNumber : public JSBase {
private:
  double _value;

public:
  JSNumber(double value) : _value(value) {}
  double getValue() const { return _value; }
  void setValue(double value) { _value = value; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSString : public JSBase {
private:
  std::wstring _value;

public:
  JSString(const std::wstring &value) : _value(value) {}
  const std::wstring &getValue() const { return _value; }
  void setValue(const std::wstring &value) { _value = value; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSBoolean : public JSBase {
private:
  bool _value;

public:
  JSBoolean(bool value) : _value(value) {}
  bool getValue() const { return _value; }
  void setValue(bool value) { _value = value; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSUndefined : public JSBase {
public:
  inline JSValue *toString(JSContext *ctx) override;
};

struct JSField {
  bool configurable{};
  bool enumable{};

  JSAtom *value{};
  bool writable{};

  JSAtom *getter{};
  JSAtom *setter{};
};

class JSObject : public JSBase {
private:
  std::unordered_map<std::wstring, JSField> _fields;
  std::unordered_map<JSAtom *, JSField> _symboledFields;
  bool _sealed;
  bool _frozen;
  bool _extensible;
  JSAtom *_prototype;

public:
  JSObject(JSAtom *prototype)
      : _sealed(false), _frozen(false), _extensible(true),
        _prototype(prototype) {}

  JSAtom *getPrototype() { return _prototype; }

  const JSAtom *getPrototype() const { return _prototype; }

  const std::unordered_map<std::wstring, JSField> &getFields() const {
    return _fields;
  }

  std::unordered_map<std::wstring, JSField> &getFields() { return _fields; }

  bool isSealed() const { return _sealed; }

  bool isFrozen() const { return _frozen; }

  bool isExtensible() const { return _extensible; }

  void setSealed(bool value) { _sealed = value; }

  void setFrozen(bool value) { _frozen = value; }

  void setExtensible(bool value) { _extensible = value; }

public:
  inline JSValue *toString(JSContext *ctx) override;

  virtual std::vector<std::wstring> getKeys(JSContext *ctx) {
    std::vector<std::wstring> keys;
    for (auto &[key, field] : _fields) {
      if (field.enumable) {
        keys.push_back(key);
      }
    }
    return keys;
  };

  virtual JSValue *getField(JSContext *ctx, JSValue *self,
                            const std::wstring &name);

  virtual JSValue *setField(JSContext *ctx, JSValue *self,
                            const std::wstring &name, JSValue *value);

  virtual JSValue *getIndex(JSContext *ctx, JSValue *self, size_t index);

  virtual JSValue *setIndex(JSContext *ctx, JSValue *self, size_t index,
                            JSValue *value);

  virtual JSValue *getField(JSContext *ctx, JSValue *self, JSValue *name);

  virtual JSValue *setField(JSContext *ctx, JSValue *self, JSValue *name,
                            JSValue *value);
};

class JSArray : public JSObject {
private:
  std::unordered_map<size_t, JSAtom *> _items;

  size_t _length{};

public:
  JSArray(JSAtom *prototype) : JSObject(prototype) {}

  std::unordered_map<size_t, JSAtom *> &getItems() { return _items; }

  const std::unordered_map<size_t, JSAtom *> &getItems() const {
    return _items;
  }

  size_t &getLength() { return _length; }

  size_t getLength() const { return _length; }

public:
  inline JSValue *toString(JSContext *ctx) override;

  std::vector<std::wstring> getKeys(JSContext *ctx) override {
    std::vector<std::wstring> keys;
    for (auto &[key, _] : _items) {
      keys.push_back(std::to_wstring(key));
    }
    return keys;
  }

  JSValue *getIndex(JSContext *ctx, JSValue *self, size_t index) override;

  JSValue *setIndex(JSContext *ctx, JSValue *self, size_t index,
                    JSValue *value) override;
};

class JSNull : public JSObject {
public:
  JSNull() : JSObject(nullptr) {}
  inline JSValue *toString(JSContext *ctx) override;
};

class JSCallable : public JSObject {
private:
  std::unordered_map<std::wstring, JSAtom *> _closure;
  std::wstring _name;

public:
  JSCallable(JSAtom *prototype, const std::wstring &name,
             const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSObject(prototype), _closure(closure), _name(name){};
  const std::unordered_map<std::wstring, JSAtom *> &getClosure() const {
    return _closure;
  }

  virtual JSValue *call(JSContext *ctx, JSValue *self,
                        const std::vector<JSValue *> args) = 0;

  const std::wstring &getName() const { return _name; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSNativeFunction : public JSCallable {
private:
  JS_NATIVE _native;

public:
  JSNativeFunction(JSAtom *prototype, const std::wstring &name,
                   const JS_NATIVE &native,
                   const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSCallable(prototype, name, closure), _native(native) {}
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) {
    return _native(ctx, self, args);
  }
};

class JSException : public JSBase {
private:
  std::wstring _message;
  std::vector<JSStackFrame> _stack;

public:
  JSException(const std::wstring &message,
              const std::vector<JSStackFrame> &stack)
      : _message(message), _stack(stack){};

  const std::wstring &getMessage() const { return _message; };

  const std::vector<JSStackFrame> &getStack() const { return _stack; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

enum class JS_INTERRUPT_KIND {
  RETURN,
  BREAK,
  CONTINUE,
  YIELD,
  YIELD_DELEGATE,
  AWAIT
};
template <JS_INTERRUPT_KIND kind> class JSInterrupt : public JSBase {

public:
  JSInterrupt() {}

  constexpr JS_INTERRUPT_KIND getKind() const { return kind; }

  JSValue *toString(JSContext *ctx) override { return nullptr; }
};

class JSReturnInterrupt : public JSInterrupt<JS_INTERRUPT_KIND::RETURN> {
private:
  JSValue *_value;

public:
  JSReturnInterrupt(JSValue *value = nullptr) : _value(value) {}
  JSValue *getValue() { return _value; }
};

class JSFunction : public JSCallable {
private:
  JSProgram *_program;
  size_t _address;

public:
  JSFunction(JSAtom *prototype, const std::wstring &name, JSProgram *program,
             size_t address,
             const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSCallable(prototype, name, closure), _program(program),
        _address(address) {}
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) override;
};

class JSCodeGenerator;
class JSVirtualMachine;

class JSRuntime {
private:
  JSParser *_parser;
  JSCodeGenerator *_generator;
  JSVirtualMachine *_vm;

public:
  JSRuntime();
  ~JSRuntime();
  JSParser *getParser() { return _parser; }
  JSCodeGenerator *getGenerator() { return _generator; }
  JSVirtualMachine *getVirtualMachine() { return _vm; }

  void setDirective(const std::wstring &directive) {}
};

class JSContext {
private:
  JSRuntime *_runtime;
  JSScope *_root;
  JSScope *_current;

private:
  JSValue *_null;
  JSValue *_undefined;
  JSValue *_global;

public:
  JSContext(JSRuntime *runtime) : _runtime(runtime) {
    _root = new JSScope();
    _current = _root;
    _null = _current->createValue(JS_TYPE::OBJECT, new JSNull{});
    _global =
        _current->createValue(JS_TYPE::OBJECT, new JSObject{_null->getAtom()});
    _null->getAtom()->addChild(_global->getAtom());

    _undefined = _current->createValue(JS_TYPE::UNDEFINED, new JSUndefined{});
  }

  ~JSContext() {
    _current = nullptr;
    delete _root;
    _root = nullptr;
    _runtime = nullptr;
  }

  JSRuntime *getRuntime() { return _runtime; }

  JSValue *eval(const std::wstring &filename, const std::wstring &source);

  void pushScope() { _current = new JSScope(_current); }

  void popScope() {
    auto parent = _current->getParent();
    parent->removeChild(_current);
    delete _current;
    _current = parent;
  }

  JSScope *getScope() { return _current; }

  void setScope(JSScope *scope) { _current = scope; }

  JSValue *createUndefined() {
    return _current->createValue(JS_TYPE::UNDEFINED, new JSUndefined{});
  }
  JSValue *createUninitialized() {
    return _current->createValue(JS_TYPE::UNINITIALIZED, new JSUninitialize{});
  }
  JSValue *createNull() {
    return _current->createValue(JS_TYPE::OBJECT, new JSNull{});
  }

  JSValue *createNumber(double value) {
    return _current->createValue(JS_TYPE::NUMBER, new JSNumber{value});
  }

  JSValue *createString(const std::wstring &value) {
    return _current->createValue(JS_TYPE::STRING, new JSString{value});
  }

  JSValue *createBoolean(bool value) {
    return _current->createValue(JS_TYPE::BOOLEAN, new JSBoolean{value});
  }

  JSValue *createObject() {
    return _current->createValue(JS_TYPE::OBJECT, new JSObject{nullptr});
  }

  JSValue *createArray() {
    return _current->createValue(JS_TYPE::OBJECT, new JSArray{nullptr});
  }

  JSValue *createNativeFunction(
      const JS_NATIVE &func, const std::wstring &name,
      const std::unordered_map<std::wstring, JSValue *> &closure = {}) {
    std::unordered_map<std::wstring, JSAtom *> clo;
    for (auto &[n, val] : closure) {
      clo[n] = val->getAtom();
    }
    auto val = _current->createValue(
        JS_TYPE::FUNCTION, new JSNativeFunction{nullptr, name, func, clo});
    for (auto &[n, atom] : clo) {
      val->getAtom()->addChild(atom);
    }
    return val;
  }

  JSValue *createException(const std::wstring &message,
                           const std::wstring &filename = L"",
                           size_t column = 0, size_t line = 0) {
    return _current->createValue(JS_TYPE::EXCEPTION,
                                 new JSException{message, {}});
  }

  JSValue *getGlobal() { return _global; }

  JSValue *queryValue(const std::wstring &name) {
    auto scope = _current;
    while (scope) {
      auto val = scope->queryValue(name);
      if (val) {
        return val;
      }
      scope = scope->getParent();
    }
    auto keys = getKeys(_global);
    if (std::find(keys.begin(), keys.end(), name) == keys.end()) {
      return createException(L"ReferenceError: " + name + L" is not defined");
    }
    return getField(_global, name);
  }

  JSValue *assigmentValue(JSValue *variable, JSValue *value) {
    if (variable->isConst() && variable->getType() != JS_TYPE::UNINITIALIZED) {
      return createException(L"TypeError: Assignment to constant variable.");
    }
    if (value->getType() >= JS_TYPE::OBJECT) {
      variable->setAtom(value->getAtom());
    } else {
      if (value->getType() == JS_TYPE::UNDEFINED) {
        variable->setAtom(
            _current->createAtom(JS_TYPE::UNDEFINED, new JSUndefined{}));
      }
      // TODO:
    }
    return value;
  }

  JSValue *call(JSValue *func, JSValue *self,
                const std::vector<JSValue *> args) {
    auto fn = const_cast<JSCallable *>(
        dynamic_cast<const JSCallable *>(func->getAtom()->getData()));
    for (auto &[key, val] : fn->getClosure()) {
      auto value = _current->createValue(val);
      _current->storeValue(key, value);
    }
    for (auto &arg : args) {
      _current->createValue(arg->getAtom());
    }
    return fn->call(this, self, args);
  }

  JSValue *construct(JSValue *constructor, const std::vector<JSValue *> args) {
    auto obj = createObject();
    call(constructor, obj, args);
    return obj;
  }

  JSValue *setField(JSValue *obj, const std::wstring &name, JSValue *value) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->setField(this, obj, name, value);
  }

  JSValue *getField(JSValue *obj, const std::wstring &name) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->getField(this, obj, name);
  }

  JSValue *setField(JSValue *obj, JSValue *name, JSValue *value) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->setField(this, obj, name, value);
  }

  JSValue *getField(JSValue *obj, JSValue *name) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->getField(this, obj, name);
  }

  JSValue *setIndex(JSValue *obj, size_t index, JSValue *value) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->setIndex(this, obj, index, value);
  }

  JSValue *getIndex(JSValue *obj, size_t index) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->getIndex(this, obj, index);
  }

  std::vector<std::wstring> getKeys(JSValue *obj) {
    auto object = dynamic_cast<JSObject *>(obj->getData());
    return object->getKeys(this);
  }

  std::wstring toString(JSValue *value) {
    auto str = value->getAtom()->getData()->toString(this);
    auto string = (JSString *)str->getAtom()->getData();
    return string->getValue();
  }
};

class JSCodeGenerator {
public:
private:
  JSNode *unwrap(JSNode *node) {
    JSNode *result = node;
    while (result->type == JS_NODE_TYPE::EXPRESSION_GROUP) {
      result = result->cast<JSGroupExpressionNode>()->expression;
    }
    return result;
  }
  bool is(JSNode *node, const JS_NODE_TYPE &type) {
    return unwrap(node)->type == type;
  }
  uint32_t resolveConstant(JSProgram &program, const std::wstring &constant) {
    for (size_t index = 0; index < program.constants.size(); index++) {
      if (program.constants[index] == constant) {
        return (uint32_t)index;
      }
    }
    program.constants.push_back(constant);
    return program.constants.size() - 1;
  }

  void pushUint32(JSProgram &program, uint32_t value) {
    auto offset = program.codes.size();
    program.codes.push_back(0);
    program.codes.push_back(0);
    *(uint32_t *)(program.codes.data() + offset) = value;
  }

  void pushAddress(JSProgram &program, uint64_t value) {
    auto offset = program.codes.size();
    program.codes.push_back(0);
    program.codes.push_back(0);
    program.codes.push_back(0);
    program.codes.push_back(0);
    *(uint64_t *)(program.codes.data() + offset) = value;
  }

  void pushString(JSProgram &program, const std::wstring &value) {
    pushUint32(program, resolveConstant(program, value));
  }

  void pushUint16(JSProgram &program, uint16_t value) {
    program.codes.push_back(value);
  }

  void pushNumber(JSProgram &program, double value) {
    auto offset = program.codes.size();
    program.codes.push_back(0);
    program.codes.push_back(0);
    program.codes.push_back(0);
    program.codes.push_back(0);
    *(double *)(program.codes.data() + offset) = value;
  }

  void pushOperator(JSProgram &program, const JS_OPERATOR &opt) {
    pushUint16(program, (uint16_t)opt);
  }

  JSNode *createError(const std::wstring &message, const JSLocation &loc) {
    auto err = new JSErrorNode{};
    err->message = message;
    err->location = loc;
    return err;
  }

  JSNode *resolveStore(const std::wstring &source, JSNode *node,
                       JSProgram &program) {
    if (is(node, JS_NODE_TYPE::LITERAL_IDENTITY)) {
      pushOperator(program, JS_OPERATOR::STORE);
      pushString(program, node->location.get(source));
    } else if (is(node, JS_NODE_TYPE::LITERAL_THIS)) {
      pushOperator(program, JS_OPERATOR::THIS);
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_MEMBER)) {
      auto expression = node->cast<JSMemberExpressionNode>();
      pushOperator(program, JS_OPERATOR::STR);
      pushString(program, expression->field->location.get(source));
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        auto err = resolve(source, expression->host, program);
        if (err) {
          return err;
        }
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        pushOperator(program, JS_OPERATOR::SET_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::SET_SUPER_FIELD);
      }
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER)) {
      auto expression = node->cast<JSComputedMemberExpressionNode>();
      auto err = resolve(source, expression->field, program);
      if (err) {
        return err;
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        auto err = resolve(source, expression->host, program);
        if (err) {
          return err;
        }
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        pushOperator(program, JS_OPERATOR::SET_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::SET_SUPER_FIELD);
      }
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_GROUP)) {
      auto expression = node->cast<JSGroupExpressionNode>();
      return resolveStore(source, expression->expression, program);
    } else if (is(node, JS_NODE_TYPE::PATTERN_OBJECT)) {
      auto expression = node->cast<JSObjectPatternNode>();
      size_t index = 0;
      for (auto item : expression->items) {
        auto field = item->cast<JSObjectPatternItemNode>();
        if (is(field->key, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
          auto spread = field->key->cast<JSSpreadPatternItemNode>();
          pushOperator(program, JS_OPERATOR::OBJECT_SPREAD);
          pushUint32(program, index);
          auto err = resolveStore(source, spread->value, program);
          if (err) {
            return err;
          }
          pushOperator(program, JS_OPERATOR::POP);
        } else {
          if (field->computed) {
            auto err = resolve(source, field->key, program);
            if (err) {
              return err;
            }
          } else {
            pushOperator(program, JS_OPERATOR::STR);
            pushString(program, field->key->location.get(source));
          }
          pushOperator(program, JS_OPERATOR::PUSH_VALUE);
          pushUint32(program, index + 1);
          pushOperator(program, JS_OPERATOR::GET_FIELD);
          if (field->value) {
            pushOperator(program, JS_OPERATOR::JNOT_NULL);
            auto address = program.codes.size();
            pushAddress(program, 0);
            auto err = resolve(source, field->value, program);
            if (err) {
              return err;
            }
            *(size_t *)(program.codes.data() + address) = program.codes.size();
          }
          if (field->alias) {
            auto err = resolveStore(source, field->alias, program);
            if (err) {
              return err;
            }
          } else {
            auto err = resolveStore(source, field->key, program);
            if (err) {
              return err;
            }
          }
          pushOperator(program, JS_OPERATOR::POP);
          index++;
        }
      }
    } else if (is(node, JS_NODE_TYPE::PATTERN_ARRAY)) {
      auto expression = node->cast<JSArrayPatternNode>();
      for (auto &item : expression->items) {
        if (item && is(item, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
          auto field = item->cast<JSSpreadPatternItemNode>();
          pushOperator(program, JS_OPERATOR::ARRAY_SPREAD);
          auto err = resolveStore(source, field->value, program);
          if (err) {
            return err;
          }
          pushOperator(program, JS_OPERATOR::POP);
        } else {
          pushOperator(program, JS_OPERATOR::POP);
          if (item) {
            auto field = item->cast<JSArrayPatternItemNode>();
            if (field->value) {
              pushOperator(program, JS_OPERATOR::JNOT_NULL);
              auto address = program.codes.size();
              pushAddress(program, 0);
              auto err = resolve(source, field->value, program);
              if (err) {
                return err;
              }
              *(size_t *)(program.codes.data() + address) =
                  program.codes.size();
            }
            auto err = resolveStore(source, field->alias, program);
            if (err) {
              return err;
            }
            pushOperator(program, JS_OPERATOR::POP);
          } else {
            pushOperator(program, JS_OPERATOR::POP);
          }
          pushOperator(program, JS_OPERATOR::NEXT);
        }
      }
      pushOperator(program, JS_OPERATOR::POP);
    } else {
      return createError(
          L"Invalid left-handle assigment node:" +
              JSON::stringify(node->toJSON(program.filename, source)),
          node->location);
    }
    return nullptr;
  }

  JSNode *resolveMemberChain(const std::wstring &source, JSNode *node,
                             JSProgram &program,
                             std::vector<size_t> &addresses) {
    node = unwrap(node);
    if (is(node, JS_NODE_TYPE::EXPRESSION_MEMBER) ||
        is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER)) {
      auto expression = node->cast<JSMemberExpressionNode>();
      pushOperator(program, JS_OPERATOR::STR);
      pushString(program, expression->field->location.get(source));
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        auto err =
            resolveMemberChain(source, expression->host, program, addresses);
        if (err) {
          return err;
        }
      }
      if (is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER)) {
        pushOperator(program, JS_OPERATOR::JNULL);
        addresses.push_back(program.codes.size());
        pushAddress(program, 0);
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        pushOperator(program, JS_OPERATOR::GET_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::GET_SUPER_FIELD);
      }
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) ||
               is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
      auto expression = node->cast<JSComputedMemberExpressionNode>();
      auto err = resolve(source, expression->field, program);
      if (err) {
        return err;
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        auto err =
            resolveMemberChain(source, expression->host, program, addresses);
        if (err) {
          return err;
        }
      }
      if (is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
        pushOperator(program, JS_OPERATOR::JNULL);
        addresses.push_back(program.codes.size());
        pushAddress(program, 0);
      }
      if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
        pushOperator(program, JS_OPERATOR::GET_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::GET_SUPER_FIELD);
      }
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_CALL) ||
               is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL)) {
      auto expression = node->cast<JSCallExpressionNode>();
      auto callee = unwrap(expression->callee);
      if (callee->type == JS_NODE_TYPE::EXPRESSION_MEMBER ||
          callee->type == JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER) {
        auto expression = callee->cast<JSMemberExpressionNode>();
        auto err =
            resolveMemberChain(source, expression->host, program, addresses);
        if (err) {
          return err;
        }
        if (is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER)) {
          pushOperator(program, JS_OPERATOR::JNULL);
          addresses.push_back(program.codes.size());
          pushAddress(program, 0);
        }
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, expression->field->location.get(source));
      } else if (is(callee, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) ||
                 is(callee,
                    JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
        auto expression = callee->cast<JSComputedMemberExpressionNode>();
        auto err =
            resolveMemberChain(source, expression->host, program, addresses);
        if (err) {
          return err;
        }
        if (is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
          pushOperator(program, JS_OPERATOR::JNULL);
          addresses.push_back(program.codes.size());
          pushAddress(program, 0);
        }
        err = resolve(source, expression->field, program);
        if (err) {
          return err;
        }
      } else if (!is(callee, JS_NODE_TYPE::LITERAL_SUPER)) {
        auto err = resolveMemberChain(source, callee, program, addresses);
        if (err) {
          return err;
        }
      }
      if (is(node, JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL)) {
        pushOperator(program, JS_OPERATOR::JNULL);
        addresses.push_back(program.codes.size());
        pushAddress(program, 0);
      }
      for (auto it = expression->arguments.rbegin();
           it != expression->arguments.rend(); it++) {
        auto arg = *it;
        if (is(arg, JS_NODE_TYPE::EXPRESSION_SPREAD)) {
          auto expr = arg->cast<JSSpreadExpressionNode>();
          auto err = resolve(source, expr->value, program);
          if (err) {
            return err;
          }
          pushOperator(program, JS_OPERATOR::SPREAD);
        } else {
          auto err = resolve(source, arg, program);
          if (err) {
            return err;
          }
        }
      }
      if (is(callee, JS_NODE_TYPE::EXPRESSION_MEMBER) ||
          is(callee, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) ||
          is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER) ||
          is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
        pushOperator(program, JS_OPERATOR::MEMBER_CALL);
      } else if (is(callee, JS_NODE_TYPE::LITERAL_SUPER)) {
        pushOperator(program, JS_OPERATOR::SUPER_CALL);
      } else {
        pushOperator(program, JS_OPERATOR::CALL);
      }
      pushUint32(program, (uint32_t)expression->arguments.size());
    } else if (is(node, JS_NODE_TYPE::EXPRESSION_GROUP)) {
      auto expression = node->cast<JSGroupExpressionNode>();
      return resolveMemberChain(source, expression->expression, program,
                                addresses);
    } else {
      return resolve(source, node, program);
    }
    return nullptr;
  }

public:
  JSCodeGenerator() {}

  ~JSCodeGenerator() {}

  JSNode *resolveNode(const std::wstring &source, JSNode *node,
                      JSProgram &program) {
    switch (node->type) {
    case JS_NODE_TYPE::PRIVATE_NAME:
      break;
    case JS_NODE_TYPE::LITERAL_REGEX:
      return resolveRegexLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_NULL:
      return resolveNullLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_STRING:
      return resolveStringLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_BOOLEAN:
      return resolveBooleanLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_NUMBER:
      return resolveNumberLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_UNDEFINED:
      return resolveUndefinedLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_IDENTITY:
      return resolveIdentityLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_TEMPLATE:
      return resolveTemplateLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_BIGINT:
      return resolveBigintLiteral(source, node, program);
    case JS_NODE_TYPE::LITERAL_THIS:
      return resolveThis(source, node, program);
    case JS_NODE_TYPE::PROGRAM:
      return resolveProgram(source, node, program);
    case JS_NODE_TYPE::STATEMENT_BLOCK:
      return resolveBlockStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_DEBUGGER:
      return resolveDebuggerStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_RETURN:
      return resolveReturnStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_LABEL:
      break;
    case JS_NODE_TYPE::STATEMENT_BREAK:
      break;
    case JS_NODE_TYPE::STATEMENT_CONTINUE:
      break;
    case JS_NODE_TYPE::STATEMENT_IF:
      return resolveIfStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_SWITCH:
      return resolveSwitchStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_THROW:
      return resolveThrowStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_TRY:
      return resolveTryStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_TRY_CATCH:
      return resolveTryCatchStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_WHILE:
      return resolveWhileStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_DO_WHILE:
      return resolveDoWhileStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_FOR:
      return resolveForStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_FOR_IN:
      return resolveForInStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_FOR_OF:
      return resolveForOfStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF:
      return resolveForAwaitOfStatement(source, node, program);
    case JS_NODE_TYPE::STATEMENT_EXPRESSION:
      return resolveExpressionStatement(source, node, program);
    case JS_NODE_TYPE::DECORATOR:
      break;
    case JS_NODE_TYPE::EXPRESSION_BINARY:
      return resolveBinaryExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_MEMBER:
      return resolveMemberExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER:
      return resolveOptionalMemberExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER:
      return resolveComputedMemberExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER:
      return resolveOptionalComputedMemberExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_CONDITION:
      return resolveConditionExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_CALL:
      return resolveCallExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_OPTIONAL_CALL:
      return resolveOptionalCallExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_NEW:
      return resolveNewExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_DELETE:
      return resolveDeleteExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_AWAIT:
      return resolveAwaitExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_YIELD:
      return resolveYieldExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_YIELD_DELEGATE:
      return resolveYieldDelegateExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_VOID:
      return resolveVoidExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_TYPEOF:
      return resolveTypeofExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_GROUP:
      return resolveGroupExpression(source, node, program);
    case JS_NODE_TYPE::EXPRESSION_ASSIGMENT:
      return resolveAssigmentExpression(source, node, program);
    case JS_NODE_TYPE::IMPORT_DECLARATION:
      break;
    case JS_NODE_TYPE::IMPORT_SPECIFIER:
      break;
    case JS_NODE_TYPE::IMPORT_DEFAULT:
      break;
    case JS_NODE_TYPE::IMPORT_NAMESPACE:
      break;
    case JS_NODE_TYPE::IMPORT_ATTARTUBE:
      break;
    case JS_NODE_TYPE::EXPORT_DECLARATION:
      break;
    case JS_NODE_TYPE::EXPORT_DEFAULT:
      break;
    case JS_NODE_TYPE::EXPORT_SPECIFIER:
      break;
    case JS_NODE_TYPE::EXPORT_NAMED:
      break;
    case JS_NODE_TYPE::EXPORT_NAMESPACE:
      break;
    case JS_NODE_TYPE::DECLARATION_ARROW_FUNCTION:
      return resolveArrowFunctionDeclaration(source, node, program);
    case JS_NODE_TYPE::DECLARATION_FUNCTION:
      return resolveFunctionDeclarator(source, node, program);
    case JS_NODE_TYPE::DECLARATION_FUNCTION_BODY:
      return resolveFunctionBodyDeclaration(source, node, program);
    case JS_NODE_TYPE::DECLARATION_OBJECT:
      return resolveObjectDeclaration(source, node, program);
    case JS_NODE_TYPE::DECLARATION_ARRAY:
      return resolveArrayDeclaration(source, node, program);
    case JS_NODE_TYPE::DECLARATION_CLASS:
      return resolveClassDeclaration(source, node, program);
    case JS_NODE_TYPE::DECLARATION_VARIABLE:
      return resolveVariableDeclaration(source, node, program);
    default:
      break;
    }
    return nullptr;
  }

  std::unordered_map<JSNode *, size_t>
  beginScope(const std::wstring &source, JSNode *node, JSProgram &program) {
    std::unordered_map<JSNode *, size_t> ctx;
    pushOperator(program, JS_OPERATOR::BEGIN);
    for (auto declar : node->scope->declarations) {
      switch (declar.type) {
      case JS_DECLARATION_TYPE::VAR:
        pushOperator(program, JS_OPERATOR::VAR);
        pushString(program, declar.name);
        break;
      case JS_DECLARATION_TYPE::CONST:
        pushOperator(program, JS_OPERATOR::CONST);
        pushString(program, declar.name);
        break;
      case JS_DECLARATION_TYPE::LET:
        pushOperator(program, JS_OPERATOR::LET);
        pushString(program, declar.name);
        break;
      case JS_DECLARATION_TYPE::FUNCTION: {
        auto func = declar.declaration->cast<JSFunctionBaseNode>();
        if (func->async) {
          if (func->generator) {
            pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
          }
        } else {
          if (func->generator) {
            pushOperator(program, JS_OPERATOR::GENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::FUNCTION);
          }
        }
        ctx[declar.declaration] = program.codes.size();
        pushAddress(program, 0);
        pushOperator(program, JS_OPERATOR::STORE);
        pushString(program, declar.name);
        pushOperator(program, JS_OPERATOR::POP);
      } break;
      }
    }
    return ctx;
  }

  JSNode *endScope(const std::wstring &source, JSNode *node, JSProgram &program,
                   std::unordered_map<JSNode *, size_t> &ctx) {
    if (!ctx.empty()) {
      pushOperator(program, JS_OPERATOR::JMP);
      auto address = program.codes.size();
      pushAddress(program, 0);
      for (auto [declar, addr] : ctx) {
        *(size_t *)(program.codes.data() + addr) = program.codes.size();
        auto err =
            resolveFunctionDeclaration(source, (JSNode *)declar, program);
        if (err) {
          return err;
        }
      }
      *(size_t *)(program.codes.data() + address) = program.codes.size();
    }
    pushOperator(program, JS_OPERATOR::END);
    return nullptr;
  }

  JSNode *resolveFunctionDeclaration(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto func = node->cast<JSFunctionBaseNode>();
    auto ctx = beginScope(source, node, program);
    for (auto arg : func->arguments) {
      auto argument = arg->cast<JSFunctionArgumentDeclarationNode>();
      if (is(argument->identifier, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
        pushOperator(program, JS_OPERATOR::ARGUMENT_SPREAD);
        auto spread = argument->identifier->cast<JSSpreadPatternItemNode>();
        auto err = resolveStore(source, spread->value, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::EMPTY_CHECK);
        if (argument->value) {
          pushOperator(program, JS_OPERATOR::JNOT_NULL);
          auto address = program.codes.size();
          pushAddress(program, 0);
          pushOperator(program, JS_OPERATOR::POP);
          auto err = resolve(source, argument->value, program);
          if (err) {
            return err;
          }
          *(size_t *)(program.codes.data() + address) = program.codes.size();
        }
        auto err = resolveStore(source, argument->identifier, program);
        if (err) {
          return err;
        }
      }
    }
    auto err = resolve(source, func->body, program);
    if (err) {
      return err;
    }
    if (func->body->type != JS_NODE_TYPE::DECLARATION_FUNCTION_BODY) {
      pushOperator(program, JS_OPERATOR::RET);
    }
    return endScope(source, node, program, ctx);
  }

  JSNode *resolveFunctionBodyDeclaration(const std::wstring &source,
                                         JSNode *node, JSProgram &program) {
    auto body = node->cast<JSFunctionBodyDeclarationNode>();
    std::vector<std::wstring> directives;
    for (auto directive : body->directives) {
      auto str = directive->location.get(source);
      str = str.substr(1, str.size() - 2);
      directives.push_back(str);
      pushOperator(program, JS_OPERATOR::ENABLE);
      pushString(program, str);
    }
    for (auto &statement : body->statements) {
      auto err = resolve(source, statement, program);
      if (err) {
        return err;
      }
      if (is(statement, JS_NODE_TYPE::STATEMENT_EXPRESSION)) {
        pushOperator(program, JS_OPERATOR::POP);
      }
    }
    pushOperator(program, JS_OPERATOR::UNDEFINED);
    pushOperator(program, JS_OPERATOR::RET);

    return nullptr;
  }
  JSNode *resolveArrowFunctionDeclaration(const std::wstring &source,
                                          JSNode *node, JSProgram &program) {
    auto func = node->cast<JSArrowDeclarationNode>();
    if (func->async) {
      pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
    } else {
      pushOperator(program, JS_OPERATOR::FUNCTION);
    }
    auto address = program.codes.size();
    pushAddress(program, 0);
    pushOperator(program, JS_OPERATOR::JMP);
    auto end = program.codes.size();
    pushAddress(program, 0);
    *(size_t *)(program.codes.data() + address) = program.codes.size();
    auto err = resolveFunctionDeclaration(source, node, program);
    if (err) {
      return err;
    }
    *(size_t *)(program.codes.data() + end) = program.codes.size();
    pushOperator(program, JS_OPERATOR::STR);
    pushString(program, L"bind");
    pushOperator(program, JS_OPERATOR::THIS);
    pushOperator(program, JS_OPERATOR::MEMBER_CALL);
    pushUint32(program, 1);
    return nullptr;
  }

  JSNode *resolveFunctionDeclarator(const std::wstring &source, JSNode *node,
                                    JSProgram &program) {
    auto func = node->cast<JSFunctionDeclarationNode>();
    if (func->identifier) {
      pushOperator(program, JS_OPERATOR::LOAD);
      pushString(program, func->identifier->location.get(source));
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
    } else {
      if (func->async) {
        if (func->generator) {
          pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
        } else {
          pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
        }
      } else {
        if (func->generator) {
          pushOperator(program, JS_OPERATOR::GENERATOR);
        } else {
          pushOperator(program, JS_OPERATOR::FUNCTION);
        }
      }
      auto address = program.codes.size();
      pushAddress(program, 0);
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
      pushOperator(program, JS_OPERATOR::JMP);
      auto end = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + address) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, node, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + end) = program.codes.size();
    }
    return nullptr;
  }

  JSNode *resolve(const std::wstring &source, JSNode *node,
                  JSProgram &program) {
    std::unordered_map<JSNode *, size_t> ctx;
    auto funcion = node->cast<JSFunctionBaseNode>();
    auto clazz = node->cast<JSClassDeclaration>();
    if (node->scope && !funcion && !clazz) {
      ctx = beginScope(source, node, program);
    }
    auto err = resolveNode(source, node, program);
    if (err) {
      return err;
    }
    if (node->scope && !funcion && !clazz) {
      auto err = endScope(source, node, program, ctx);
      if (err) {
        return err;
      }
    }
    return nullptr;
  }
  JSNode *resolveRegexLiteral(const std::wstring &source, JSNode *node,
                              JSProgram &program) {
    pushOperator(program, JS_OPERATOR::REGEX);
    pushString(program, node->location.get(source));
    return nullptr;
  }
  JSNode *resolveNullLiteral(const std::wstring &source, JSNode *node,
                             JSProgram &program) {
    pushOperator(program, JS_OPERATOR::NIL);
    return nullptr;
  }

  JSNode *resolveUndefinedLiteral(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    pushOperator(program, JS_OPERATOR::UNDEFINED);
    return nullptr;
  }
  JSNode *resolveIdentityLiteral(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    pushOperator(program, JS_OPERATOR::LOAD);
    pushString(program, node->location.get(source));
    return nullptr;
  }
  JSNode *resolveBigintLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program) {
    pushOperator(program, JS_OPERATOR::BIGINT);
    pushString(program, node->location.get(source));
    return nullptr;
  }
  JSNode *resolveTemplateLiteral(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto temp = node->cast<JSTemplateLiteralNode>();
    if (temp->tag) {
      auto tag = unwrap(temp->tag);
      std::vector<size_t> addresses;
      JS_OPERATOR opt = JS_OPERATOR::CALL;
      if (is(tag, JS_NODE_TYPE::EXPRESSION_MEMBER)) {
        auto expr = (tag)->cast<JSMemberExpressionNode>();
        auto err = resolveMemberChain(source, expr->host, program, addresses);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, expr->field->location.get(source));
        opt = JS_OPERATOR::MEMBER_CALL;
      } else if (is(tag, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER)) {
        auto expr = (tag)->cast<JSComputedMemberExpressionNode>();
        auto err = resolveMemberChain(source, expr->host, program, addresses);
        if (err) {
          return err;
        }
        err = resolve(source, expr->field, program);
        if (err) {
          return err;
        }
        opt = JS_OPERATOR::MEMBER_CALL;
      } else if (is(tag, JS_NODE_TYPE::LITERAL_SUPER)) {
        opt = JS_OPERATOR::SUPER_CALL;
      } else {
        auto err = resolveMemberChain(source, tag, program, addresses);
        if (err) {
          return err;
        }
      }
      if (!addresses.empty()) {
        return createError(L"Invalid tagged template on optional chain",
                           temp->location);
      }
      for (auto it = temp->expressions.rbegin(); it != temp->expressions.rend();
           it++) {
        auto err = resolve(source, *it, program);
        if (err) {
          return err;
        }
      }
      pushOperator(program, JS_OPERATOR::ARRAY);
      for (size_t index = 0; index < temp->quasis.size(); index++) {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, temp->quasis[index]->location.get(source));
        pushOperator(program, JS_OPERATOR::PUSH);
        pushNumber(program, index);
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        pushOperator(program, JS_OPERATOR::SET_FIELD);
        pushOperator(program, JS_OPERATOR::POP);
      }
      pushOperator(program, opt);
      pushUint32(program, temp->expressions.size() + 1);
    } else {
      auto str = temp->quasis[0]->location.get(source);
      pushOperator(program, JS_OPERATOR::STR);
      pushString(program, str);
      size_t index = 0;
      while (index < temp->expressions.size()) {
        auto err = resolve(source, temp->expressions[index], program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::ADD);
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, temp->quasis[index + 1]->location.get(source));
        pushOperator(program, JS_OPERATOR::ADD);
        index++;
      }
    }
    return nullptr;
  }

  JSNode *resolveStringLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program) {
    auto string = unwrap(node)->cast<JSStringLiteralNode>();
    pushOperator(program, JS_OPERATOR::STR);
    auto str = string->location.get(source);
    str = str.substr(1, str.length() - 2);
    pushString(program, str);
    return nullptr;
  }
  JSNode *resolveBooleanLiteral(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto boolean = unwrap(node)->cast<JSBooleanLiteralNode>();
    auto str = boolean->location.get(source);
    if (str == L"true") {
      pushOperator(program, JS_OPERATOR::TRUE);
    } else {
      pushOperator(program, JS_OPERATOR::FALSE);
    }
    return nullptr;
  }
  JSNode *resolveNumberLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program) {
    auto number = unwrap(node)->cast<JSNumberLiteralNode>();
    auto str = number->location.get(source);
    double val = std::stold(str);
    pushOperator(program, JS_OPERATOR::PUSH);
    pushNumber(program, val);
    return nullptr;
  }

  JSNode *resolveExpressionStatement(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto statement = unwrap(node)->cast<JSExpressionStatementNode>();
    return resolve(source, statement->expression, program);
  }
  JSNode *resolveWhileStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto statement = unwrap(node)->cast<JSWhileStatement>();
    auto start = program.codes.size();
    auto err = resolve(source, statement->condition, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JFALSE);
    auto end_address = program.codes.size();
    pushAddress(program, 0);
    err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    pushAddress(program, start);
    *(size_t *)(program.codes.data() + end_address) = program.codes.size();
    return nullptr;
  }
  JSNode *resolveDoWhileStatement(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto statement = unwrap(node)->cast<JSDoWhileStatement>();
    auto start = program.codes.size();
    auto err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    err = resolve(source, statement->condition, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JTRUE);
    pushAddress(program, start);
    return nullptr;
  }
  JSNode *resolveForStatement(const std::wstring &source, JSNode *node,
                              JSProgram &program) {
    auto statement = unwrap(node)->cast<JSForStatement>();
    if (statement->init) {
      auto err = resolve(source, statement->init, program);
      if (err) {
        return err;
      }
    }
    size_t end_address = 0;
    auto start = program.codes.size();
    if (statement->condition) {
      auto err = resolve(source, statement->init, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::JFALSE);
      end_address = program.codes.size();
      pushAddress(program, 0);
    }
    auto err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    pushAddress(program, start);
    *(size_t *)(program.codes.data() + end_address) = program.codes.size();
    return nullptr;
  }
  JSNode *resolveForOfStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto statement = node->cast<JSForOfStatement>();
    auto err = resolve(source, statement->right, program);
    if (err) {
      return err;
    }
    auto start = program.codes.size();
    pushOperator(program, JS_OPERATOR::NEXT);
    pushOperator(program, JS_OPERATOR::JTRUE);
    auto end_address = program.codes.size();
    pushAddress(program, 0);
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
    err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    pushAddress(program, start);
    *(size_t *)(program.codes.data() + end_address) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    return nullptr;
  }
  JSNode *resolveForInStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto statement = node->cast<JSForInStatement>();
    auto err = resolve(source, statement->right, program);
    if (err) {
      return err;
    }
    auto start = program.codes.size();
    pushOperator(program, JS_OPERATOR::GET_KEYS);
    pushOperator(program, JS_OPERATOR::NEXT);
    pushOperator(program, JS_OPERATOR::JTRUE);
    auto end_address = program.codes.size();
    pushAddress(program, 0);
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
    err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    pushAddress(program, start);
    *(size_t *)(program.codes.data() + end_address) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    pushOperator(program, JS_OPERATOR::POP);
    return nullptr;
  }
  JSNode *resolveForAwaitOfStatement(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto statement = node->cast<JSForAwaitOfStatement>();
    auto err = resolve(source, statement->right, program);
    if (err) {
      return err;
    }
    auto start = program.codes.size();
    pushOperator(program, JS_OPERATOR::AWAIT_NEXT);
    pushOperator(program, JS_OPERATOR::JTRUE);
    auto end_address = program.codes.size();
    pushAddress(program, 0);
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
    err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    pushAddress(program, start);
    *(size_t *)(program.codes.data() + end_address) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    return nullptr;
  }
  JSNode *resolveObjectDeclaration(const std::wstring &source, JSNode *node,
                                   JSProgram &program) {
    auto declaration = node->cast<JSObjectDeclarationNode>();
    pushOperator(program, JS_OPERATOR::OBJECT);
    for (auto prop : declaration->properties) {
      if (prop->type == JS_NODE_TYPE::OBJECT_PROPERTY) {
        auto p = prop->cast<JSObjectPropertyNode>();
        if (p->value) {
          auto err = resolve(source, p->value, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::LOAD);
          pushString(program, p->key->location.get(source));
        }
        if (p->computed) {
          auto err = resolve(source, p->key, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, p->key->location.get(source));
        }
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        pushOperator(program, JS_OPERATOR::SET_FIELD);
        pushOperator(program, JS_OPERATOR::POP);
      } else if (prop->type == JS_NODE_TYPE::OBJECT_METHOD) {
        auto p = prop->cast<JSObjectMethodNode>();
        if (p->async) {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
          }
        } else {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::GENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::FUNCTION);
          }
        }
        auto address = program.codes.size();
        pushAddress(program, 0);
        pushOperator(program, JS_OPERATOR::JMP);
        auto end_address = program.codes.size();
        pushAddress(program, 0);
        *(size_t *)(program.codes.data() + address) = program.codes.size();
        auto err = resolveFunctionDeclaration(source, p, program);
        if (err) {
          return err;
        }
        *(size_t *)(program.codes.data() + end_address) = program.codes.size();
        if (p->computed) {
          auto err = resolve(source, p->key, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, p->key->location.get(source));
        }
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        pushOperator(program, JS_OPERATOR::SET_FIELD);
        pushOperator(program, JS_OPERATOR::POP);
      } else if (prop->type == JS_NODE_TYPE::OBJECT_ACCESSOR) {
        auto p = prop->cast<JSObjectAccessorNode>();
        if (p->async) {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
          }
        } else {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::GENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::FUNCTION);
          }
        }
        auto address = program.codes.size();
        pushAddress(program, 0);
        pushOperator(program, JS_OPERATOR::JMP);
        auto end_address = program.codes.size();
        pushAddress(program, 0);
        *(size_t *)(program.codes.data() + address) = program.codes.size();
        auto err = resolveFunctionDeclaration(source, p, program);
        if (err) {
          return err;
        }
        *(size_t *)(program.codes.data() + end_address) = program.codes.size();
        if (p->computed) {
          auto err = resolve(source, p->key, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, p->key->location.get(source));
        }
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        if (p->kind == JS_ACCESSOR_TYPE::GET) {
          pushOperator(program, JS_OPERATOR::SET_GETTER);
        } else {
          pushOperator(program, JS_OPERATOR::SET_SETTER);
        }
        pushOperator(program, JS_OPERATOR::POP);
      } else if (prop->type == JS_NODE_TYPE::EXPRESSION_SPREAD) {
        auto p = prop->cast<JSSpreadExpressionNode>();
        auto err = resolve(source, p->value, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::MERGE);
      }
    }
    return nullptr;
  }
  JSNode *resolveArrayDeclaration(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto declaration = node->cast<JSArrayDeclarationNode>();
    pushOperator(program, JS_OPERATOR::ARRAY);
    size_t index = 0;
    for (auto item : declaration->items) {
      if (item->type == JS_NODE_TYPE::EXPRESSION_SPREAD) {
        auto p = item->cast<JSSpreadExpressionNode>();
        auto err = resolve(source, p->value, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::MERGE);
      } else {
        auto err = resolve(source, item, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::PUSH);
        pushNumber(program, index);
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        pushOperator(program, JS_OPERATOR::SET_FIELD);
        pushOperator(program, JS_OPERATOR::POP);
        index++;
      }
    }
    return nullptr;
  }
  JSNode *resolveClassDeclaration(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto declaration = node->cast<JSClassDeclaration>();
    if (declaration->extends) {
      auto err = resolve(source, declaration->extends, program);
      if (err) {
        return err;
      }
    } else {
      pushOperator(program, JS_OPERATOR::UNDEFINED);
    }
    pushOperator(program, JS_OPERATOR::CLASS);
    if (declaration->identifier) {
      pushOperator(program, JS_OPERATOR::STORE);
      pushString(program, declaration->identifier->location.get(source));
    }
    auto ctx = beginScope(source, node, program);
    pushOperator(program, JS_OPERATOR::PUSH_VALUE);
    pushUint32(program, 0);
    pushOperator(program, JS_OPERATOR::WITH);
    for (auto prop : declaration->properties) {
      if (prop->type == JS_NODE_TYPE::CLASS_METHOD) {
        auto p = prop->cast<JSClassMethodNode>();
        if (p->async) {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
          }
        } else {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::GENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::FUNCTION);
          }
        }
        auto address = program.codes.size();
        pushAddress(program, 0);
        pushOperator(program, JS_OPERATOR::JMP);
        auto end_address = program.codes.size();
        pushAddress(program, 0);
        *(size_t *)(program.codes.data() + address) = program.codes.size();
        auto err = resolveFunctionDeclaration(source, p, program);
        if (err) {
          return err;
        }
        *(size_t *)(program.codes.data() + end_address) = program.codes.size();
        if (p->computed) {
          auto err = resolve(source, p->identifier, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, p->identifier->location.get(source));
        }
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        if (!p->static_) {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, L"prototype");
          pushOperator(program, JS_OPERATOR::GET_FIELD);
        }
        pushOperator(program, JS_OPERATOR::SET_FIELD);
        pushOperator(program, JS_OPERATOR::POP);
      } else if (prop->type == JS_NODE_TYPE::CLASS_ACCESSOR) {
        auto p = prop->cast<JSClassAccessorNode>();
        if (p->async) {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::ASYNCGENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::ASYNCFUNCTION);
          }
        } else {
          if (p->generator) {
            pushOperator(program, JS_OPERATOR::GENERATOR);
          } else {
            pushOperator(program, JS_OPERATOR::FUNCTION);
          }
        }
        auto address = program.codes.size();
        pushAddress(program, 0);
        pushOperator(program, JS_OPERATOR::JMP);
        auto end_address = program.codes.size();
        pushAddress(program, 0);
        *(size_t *)(program.codes.data() + address) = program.codes.size();
        auto err = resolveFunctionDeclaration(source, p, program);
        if (err) {
          return err;
        }
        *(size_t *)(program.codes.data() + end_address) = program.codes.size();
        if (p->computed) {
          auto err = resolve(source, p->identifier, program);
          if (err) {
            return err;
          }
        } else {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, p->identifier->location.get(source));
        }
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 2);
        if (!p->static_) {
          pushOperator(program, JS_OPERATOR::STR);
          pushString(program, L"prototype");
          pushOperator(program, JS_OPERATOR::GET_FIELD);
        }
        if (p->kind == JS_ACCESSOR_TYPE::GET) {
          pushOperator(program, JS_OPERATOR::SET_GETTER);
        } else {
          pushOperator(program, JS_OPERATOR::SET_SETTER);
        }
        pushOperator(program, JS_OPERATOR::POP);
      } else if (prop->type == JS_NODE_TYPE::CLASS_PROPERTY) {
        auto p = prop->cast<JSClassPropertyNode>();
        if (p->static_) {
          if (p->value) {
            auto err = resolve(source, p->value, program);
            if (err) {
              return err;
            }
          } else {
            pushOperator(program, JS_OPERATOR::UNDEFINED);
          }
          if (p->computed) {
            auto err = resolve(source, p->identifier, program);
            if (err) {
              return err;
            }
          } else {
            pushOperator(program, JS_OPERATOR::STR);
            pushString(program, p->identifier->location.get(source));
          }
          pushOperator(program, JS_OPERATOR::PUSH_VALUE);
          pushUint32(program, 2);
          pushOperator(program, JS_OPERATOR::SET_FIELD);
          pushOperator(program, JS_OPERATOR::POP);
        } else {
          if (p->computed) {
            auto err = resolve(source, p->identifier, program);
            if (err) {
              return err;
            }
          } else {
            pushOperator(program, JS_OPERATOR::STR);
            pushString(program, p->identifier->location.get(source));
          }
          pushOperator(program, JS_OPERATOR::PUSH_VALUE);
          pushUint32(program, 2);
          pushOperator(program, JS_OPERATOR::SET_INITIALIZER);
          auto address = program.codes.size();
          pushAddress(program, 0);
          pushOperator(program, JS_OPERATOR::JMP);
          auto end_address = program.codes.size();
          pushAddress(program, 0);
          *(size_t *)(program.codes.data() + address) = program.codes.size();
          if (p->value) {
            auto err = resolve(source, p->value, program);
            if (err) {
              return err;
            }
          } else {
            pushOperator(program, JS_OPERATOR::UNDEFINED);
          }
          pushOperator(program, JS_OPERATOR::RET);
          *(size_t *)(program.codes.data() + end_address) =
              program.codes.size();
        }
      } else if (prop->type == JS_NODE_TYPE::CLASS_STATIC_BLOCK) {
        auto p = prop->cast<JSStaticBlockNode>();
        auto err = resolve(source, p->statement, program);
        if (err) {
          return err;
        }
      }
    }
    auto err = endScope(source, node, program, ctx);
    if (err) {
      return err;
    }
    return nullptr;
  }
  JSNode *resolveVariableDeclaration(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto declarations = unwrap(node)->cast<JSVariableDeclaraionNode>();
    for (auto declaration : declarations->declarations) {
      auto declar = unwrap(declaration)->cast<JSVariableDeclaratorNode>();
      if (declar->initialize) {
        auto err = resolve(source, declar->initialize, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::UNDEFINED);
      }
      auto err = resolveStore(source, declar->identifier, program);
      if (err) {
        return err;
      }
    }
    return nullptr;
  }

  JSNode *resolveBlockStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto statement = unwrap(node)->cast<JSBlockStatement>();
    for (auto statement : statement->statements) {
      auto err = resolve(source, statement, program);
      if (err) {
        return err;
      }
    }
    return nullptr;
  }
  JSNode *resolveDebuggerStatement(const std::wstring &source, JSNode *node,
                                   JSProgram &program) {
    pushOperator(program, JS_OPERATOR::DEBUGGER);
    return nullptr;
  }
  JSNode *resolveReturnStatement(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto statement = unwrap(node)->cast<JSReturnStatement>();
    if (statement->value) {
      auto err = resolve(source, statement->value, program);
      if (err) {
        return err;
      }
    } else {
      pushOperator(program, JS_OPERATOR::UNDEFINED);
    }
    pushOperator(program, JS_OPERATOR::RET);
    return nullptr;
  }
  JSNode *resolveThrowStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto statement = unwrap(node)->cast<JSAwaitExpressionNode>();
    auto err = resolve(source, statement->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::THROW);
    return nullptr;
  }
  JSNode *resolveTryCatchStatement(const std::wstring &source, JSNode *node,
                                   JSProgram &program) {
    auto statement = unwrap(node)->cast<JSTryCatchStatement>();
    if (statement->identifier) {
      auto err = resolveStore(source, statement->identifier, program);
      if (err) {
        return err;
      }
    } else {
      pushOperator(program, JS_OPERATOR::POP);
    }
    auto err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    return nullptr;
  }
  JSNode *resolveTryStatement(const std::wstring &source, JSNode *node,
                              JSProgram &program) {
    auto statement = unwrap(node)->cast<JSTryStatement>();
    pushOperator(program, JS_OPERATOR::TRY_BEGIN);
    size_t onerror = program.codes.size();
    pushAddress(program, 0);
    size_t onfinish = 0;
    if (statement->onfinish) {
      pushOperator(program, JS_OPERATOR::DEFER);
      onfinish = program.codes.size();
      pushAddress(program, 0);
    }
    auto err = resolve(source, statement->body, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::TRY_END);
    if (statement->onerror) {
      pushOperator(program, JS_OPERATOR::JMP);
      auto onerror_end = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + onerror) = program.codes.size();
      auto err = resolve(source, statement->onerror, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + onerror_end) = program.codes.size();
    }
    if (statement->onfinish) {
      pushOperator(program, JS_OPERATOR::JMP);
      auto onfinish_end = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + onfinish) = program.codes.size();
      auto err = resolve(source, statement->onfinish, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + onfinish_end) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveIfStatement(const std::wstring &source, JSNode *node,
                             JSProgram &program) {
    auto statement = unwrap(node)->cast<JSIfStatement>();
    auto err = resolve(source, statement->condition, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JFALSE);
    auto alternate = program.codes.size();
    pushAddress(program, 0);
    pushOperator(program, JS_OPERATOR::POP);
    err = resolve(source, statement->consequent, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    auto end = program.codes.size();
    *(uint64_t *)(program.codes.data() + alternate) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    if (statement->alternate) {
      err = resolve(source, statement->alternate, program);
      if (err) {
        return err;
      }
    }
    *(uint64_t *)(program.codes.data() + end) = program.codes.size();
    return nullptr;
  }
  JSNode *resolveSwitchStatement(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto statement = unwrap(node)->cast<JSSwitchStatement>();
    auto err = resolve(source, statement->condition, program);
    if (err) {
      return err;
    }
    size_t default_address = 0;
    for (auto c : statement->cases) {
      auto cas = unwrap(c)->cast<JSSwitchCaseStatement>();
      if (cas->match) {
        auto err = resolve(source, cas->match, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::EQ);
        pushOperator(program, JS_OPERATOR::JFALSE);
        auto address = program.codes.size();
        pushAddress(program, 0);
        for (auto statement : cas->statements) {
          auto err = resolve(source, statement, program);
          if (err) {
            return err;
          }
        }
        *(size_t *)(program.codes.data() + address) = program.codes.size();
      } else {
        pushOperator(program, JS_OPERATOR::JMP);
        auto address = program.codes.size();
        pushAddress(program, 0);
        default_address = program.codes.size();
        for (auto statement : cas->statements) {
          auto err = resolve(source, statement, program);
          if (err) {
            return err;
          }
        }
        *(size_t *)(program.codes.data() + address) = program.codes.size();
      }
    }
    if (default_address) {
      pushOperator(program, JS_OPERATOR::JMP);
      pushAddress(program, default_address);
    }
    return nullptr;
  }
  JSNode *resolveBinaryExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto expression = unwrap(node)->cast<JSBinaryExpressionNode>();
    auto opt = expression->opt->location.get(source);
    if (opt == L"++" || opt == L"--") {
      if (expression->right) {
        auto err = resolve(source, expression->right, program);
        if (err) {
          return err;
        }
        if (opt == L"++") {
          pushOperator(program, JS_OPERATOR::INC);
        } else {
          pushOperator(program, JS_OPERATOR::DEC);
        }
      } else {
        auto err = resolve(source, expression->left, program);
        if (err) {
          return err;
        }
        if (opt == L"++") {
          pushOperator(program, JS_OPERATOR::UPDATE_INC);
        } else {
          pushOperator(program, JS_OPERATOR::UPDATE_DEC);
        }
      }
    } else if (opt == L"&&" || opt == L"||" || opt == L"??") {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      if (opt == L"&&") {
        pushOperator(program, JS_OPERATOR::JFALSE);
      } else if (opt == L"||") {
        pushOperator(program, JS_OPERATOR::JTRUE);
      } else if (opt == L"??") {
        pushOperator(program, JS_OPERATOR::JNOT_NULL);
      }
      auto address = program.codes.size();
      pushAddress(program, 0);
      pushOperator(program, JS_OPERATOR::POP);
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      *(uint64_t *)(program.codes.data() + address) = program.codes.size();
    } else {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      if (opt == L"+") {
        pushOperator(program, JS_OPERATOR::ADD);
      }
      if (opt == L"-") {
        pushOperator(program, JS_OPERATOR::SUB);
      }
      if (opt == L"**") {
        pushOperator(program, JS_OPERATOR::POW);
      }
      if (opt == L"*") {
        pushOperator(program, JS_OPERATOR::MUL);
      }
      if (opt == L"/") {
        pushOperator(program, JS_OPERATOR::DIV);
      }
      if (opt == L"%") {
        pushOperator(program, JS_OPERATOR::MOD);
      }
      if (opt == L"<<") {
        pushOperator(program, JS_OPERATOR::SHL);
      }
      if (opt == L">>") {
        pushOperator(program, JS_OPERATOR::SHR);
      }
      if (opt == L">>>") {
        pushOperator(program, JS_OPERATOR::USHR);
      }
      if (opt == L"&") {
        pushOperator(program, JS_OPERATOR::AND);
      }
      if (opt == L"|") {
        pushOperator(program, JS_OPERATOR::OR);
      }
      if (opt == L"^") {
        pushOperator(program, JS_OPERATOR::XOR);
      }
    }
    return nullptr;
  }
  JSNode *resolveMemberExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto expression = unwrap(node)->cast<JSMemberExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveOptionalMemberExpression(const std::wstring &source,
                                          JSNode *node, JSProgram &program) {
    auto expression = unwrap(node)->cast<JSOptionalMemberExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }

  JSNode *resolveComputedMemberExpression(const std::wstring &source,
                                          JSNode *node, JSProgram &program) {
    auto expression = unwrap(node)->cast<JSComputedMemberExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveOptionalComputedMemberExpression(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
    auto expression =
        unwrap(node)->cast<JSOptionalComputedMemberExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveConditionExpression(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto expression = unwrap(node)->cast<JSConditionExpressionNode>();
    auto err = resolve(source, expression->condition, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JFALSE);
    auto address = program.codes.size();
    pushAddress(program, 0);
    pushOperator(program, JS_OPERATOR::POP);
    err = resolve(source, expression->consequent, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::JMP);
    auto end = program.codes.size();
    pushAddress(program, 0);
    *(uint64_t *)(program.codes.data() + address) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    err = resolve(source, expression->alternate, program);
    if (err) {
      return err;
    }
    *(uint64_t *)(program.codes.data() + end) = program.codes.size();
    return nullptr;
  }
  JSNode *resolveCallExpression(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto expression = unwrap(node)->cast<JSCallExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveOptionalCallExpression(const std::wstring &source,
                                        JSNode *node, JSProgram &program) {
    auto expression = unwrap(node)->cast<JSOptionalCallExpressionNode>();
    std::vector<size_t> addresses;
    auto err = resolveMemberChain(source, expression, program, addresses);
    if (err) {
      return err;
    }
    for (auto &addr : addresses) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
    }
    return nullptr;
  }
  JSNode *resolveNewExpression(const std::wstring &source, JSNode *node,
                               JSProgram &program) {
    auto expression = unwrap(node)->cast<JSNewExpressionNode>();
    auto err = resolve(source, expression->callee, program);
    if (err) {
      return err;
    }
    for (auto arg : expression->arguments) {
      auto err = resolve(source, arg, program);
      if (err) {
        return err;
      }
    }
    pushOperator(program, JS_OPERATOR::NEW);
    pushUint32(program, (uint32_t)expression->arguments.size());
    return nullptr;
  }
  JSNode *resolveDeleteExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto expression = unwrap(node)->cast<JSDeleteExpressionNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::DELETE);
    return nullptr;
  }
  JSNode *resolveVoidExpression(const std::wstring &source, JSNode *node,
                                JSProgram &program) {
    auto expression = unwrap(node)->cast<JSVoidExpressionNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::VOID);
    return nullptr;
  }
  JSNode *resolveTypeofExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program) {
    auto expression = unwrap(node)->cast<JSTypeofExpressioNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::TYPEOF);
    return nullptr;
  }
  JSNode *resolveYieldExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::YIELD);
    return nullptr;
  }
  JSNode *resolveYieldDelegateExpression(const std::wstring &source,
                                         JSNode *node, JSProgram &program) {
    auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::YIELD_DELEGATE);
    return nullptr;
  }
  JSNode *resolveAwaitExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
    auto err = resolve(source, expression->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::AWAIT);
    return nullptr;
  }
  JSNode *resolveGroupExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
    auto expression = node->cast<JSGroupExpressionNode>();
    return resolve(source, expression->expression, program);
  }
  JSNode *resolveAssigmentExpression(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
    auto expression = unwrap(node)->cast<JSAssigmentExpressionNode>();
    auto opt = expression->opt->location.get(source);
    if (opt == L"&&=") {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::JTRUE);
      auto address = program.codes.size();
      pushAddress(program, 0);
      pushOperator(program, JS_OPERATOR::POP);
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      *(uint64_t *)(program.codes.data() + address) = program.codes.size();
    }
    if (opt == L"||=") {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::JFALSE);
      auto address = program.codes.size();
      pushAddress(program, 0);
      pushOperator(program, JS_OPERATOR::POP);
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      *(uint64_t *)(program.codes.data() + address) = program.codes.size();
    }
    if (opt == LR"(??=)") {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::JNOT_NULL);
      auto address = program.codes.size();
      pushAddress(program, 0);
      pushOperator(program, JS_OPERATOR::POP);
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      *(uint64_t *)(program.codes.data() + address) = program.codes.size();
    }
    if (opt == L"=") {
      auto err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
    } else {
      auto err = resolve(source, expression->left, program);
      if (err) {
        return err;
      }
      err = resolve(source, expression->right, program);
      if (err) {
        return err;
      }
      if (opt == L"+=") {
        pushOperator(program, JS_OPERATOR::ADD);
      }
      if (opt == L"-=") {
        pushOperator(program, JS_OPERATOR::SUB);
      }
      if (opt == L"**=") {
        pushOperator(program, JS_OPERATOR::POW);
      }
      if (opt == L"*=") {
        pushOperator(program, JS_OPERATOR::MUL);
      }
      if (opt == L"/=") {
        pushOperator(program, JS_OPERATOR::DIV);
      }
      if (opt == L"%=") {
        pushOperator(program, JS_OPERATOR::MOD);
      }
      if (opt == L"<<=") {
        pushOperator(program, JS_OPERATOR::SHL);
      }
      if (opt == L">>=") {
        pushOperator(program, JS_OPERATOR::SHR);
      }
      if (opt == L">>>=") {
        pushOperator(program, JS_OPERATOR::USHR);
      }
      if (opt == L"&=") {
        pushOperator(program, JS_OPERATOR::AND);
      }
      if (opt == L"|=") {
        pushOperator(program, JS_OPERATOR::OR);
      }
      if (opt == L"^=") {
        pushOperator(program, JS_OPERATOR::XOR);
      }
    }
    auto err = resolveStore(source, expression->left, program);
    if (err) {
      return err;
    }
    return nullptr;
  }
  JSNode *resolveThis(const std::wstring &source, JSNode *node,
                      JSProgram &program) {
    pushOperator(program, JS_OPERATOR::THIS);
    return nullptr;
  }
  JSNode *resolveProgram(const std::wstring &source, JSNode *node,
                         JSProgram &program) {
    auto prog = unwrap(node)->cast<JSProgramNode>();
    for (auto &directive : prog->directives) {
      auto str = directive->location.get(source);
      str = str.substr(1, str.size() - 2);
      pushOperator(program, JS_OPERATOR::ENABLE);
      pushString(program, str);
    }
    for (auto statement : prog->statements) {
      auto err = resolve(source, statement, program);
      if (err) {
        return err;
      }
    }
    pushOperator(program, JS_OPERATOR::HLT);
    return nullptr;
  }

  JSProgram compile(const std::wstring &filename, const std::wstring &source,
                    JSNode *node) {
    JSProgram program = {filename};
    auto err = resolve(source, node, program);
    if (err) {
      program.error = err;
    }
    return program;
  }
};

class JSVirtualMachine {
private:
  uint32_t getUint32(const JSProgram &program, size_t &address) {
    auto val = *(uint32_t *)(program.codes.data() + address);
    address += 2;
    return val;
  }
  size_t getAddress(const JSProgram &program, size_t &address) {
    auto val = *(size_t *)(program.codes.data() + address);
    address += 4;
    return val;
  }
  double getNumber(const JSProgram &program, size_t &address) {
    auto val = *(double *)(program.codes.data() + address);
    address += 4;
    return val;
  }
  const std::wstring &getString(const JSProgram &program, size_t &address) {
    auto idx = getUint32(program, address);
    return program.constants[idx];
  }

private:
  JSValue *call(JSContext *ctx, JSValue *func, JSValue *self,
                std::vector<JSValue *> args) {
    return ctx->call(func, self, args);
  }
  JSValue *construct(JSContext *ctx, JSValue *constructor,
                     std::vector<JSValue *> args) {
    return ctx->construct(constructor, args);
  }

private:
  void runBegin(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ctx->pushScope();
  }
  void runEnd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ctx->popScope();
  }
  void runPush(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto val = getNumber(program, ectx.pc);
    ectx.stack.push_back(ctx->createNumber(val));
  }

  void runPop(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.pop_back();
  }
  void runPushValue(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    auto idx = getUint32(program, ectx.pc);
    ectx.stack.push_back(*(ectx.stack.rbegin() + idx));
  }
  void runNil(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createNull());
  }

  void runUndefined(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createUndefined());
  }
  void runTrue(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createBoolean(true));
  }
  void runFalse(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createBoolean(false));
  }
  void runRegex(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
  }
  void runBigint(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
  }
  void runLoad(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->queryValue(name);
    ectx.stack.push_back(val);
    if (val->getType() == JS_TYPE::EXCEPTION) {
      ectx.pc = program.codes.size();
    }
  }
  void runStore(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto value = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto variable = ctx->queryValue(name);
    if (variable->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(variable);
      ectx.pc = program.codes.size();
      return;
    }
    ctx->assigmentValue(variable, value);
    ectx.stack.push_back(value);
  }
  void runRef(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
  }
  void runStr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto str = getString(program, ectx.pc);
    ectx.stack.push_back(ctx->createString(str));
  }
  void runVar(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUndefined();
    ctx->getScope()->storeValue(name, val);
  }
  void runConst(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUninitialized();
    val->setConst(true);
    ctx->getScope()->storeValue(name, val);
  }
  void runLet(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto name = getString(program, ectx.pc);
    auto val = ctx->createUninitialized();
    ctx->getScope()->storeValue(name, val);
  }
  void runObject(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createObject());
  }
  void runArray(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ctx->createArray());
  }
  void runFunction(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAsyncfunction(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGenerator(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAsyncgenerator(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runEnable(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDisable(JSContext *ctx, const JSProgram &program,
                  JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGetField(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto result = ctx->getField(obj, field);
    if (result->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runSetField(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto val = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto result = ctx->setField(obj, field, val);
    if (result->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runSetGetter(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {}
  void runSetSetter(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {}
  void runCall(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto func = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto result = call(ctx, func, ctx->createUndefined(), arguments);
    if (result->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runMemberCall(JSContext *ctx, const JSProgram &program,
                     JSEvalContext &ectx) {
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto field = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto obj = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto func = ctx->getField(obj, field);
    if (func->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(func);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(call(ctx, func, obj, arguments));
  }
  void runVoid(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {

    // not implement
    ectx.pc = program.codes.size();
  }
  void runTypeof(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {

    // not implement
    ectx.pc = program.codes.size();
  }
  void runNew(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto size = getUint32(program, ectx.pc);
    std::vector<JSValue *> arguments;
    for (size_t index = 0; index < size; index++) {
      auto val = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      arguments.push_back(val);
    }
    auto constructor = *ectx.stack.rbegin();
    ectx.stack.pop_back();
    auto result = construct(ctx, constructor, arguments);
    if (result->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(result);
  }
  void runDelete(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runRet(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.pc = program.codes.size();
  }
  void runYield(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAwait(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runYieldDelegate(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runJmp(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto address = getAddress(program, ectx.pc);
    ectx.pc = address;
  }
  void runJtrue(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runJfalse(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runJnull(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runJnotNull(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAdd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSub(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDiv(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runMul(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runMod(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runPow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runAnd(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runOr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runNot(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runXor(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runShr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runShl(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runUshr(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runEq(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSeq(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runNe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSne(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runLt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runLe(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runInc(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDec(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runUpdateInc(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runUpdateDec(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runNext(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    auto generator = *ectx.stack.rbegin();
    // TODO: get iterator
    auto next = ctx->getField(generator, L"next");
    if (next->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(next);
      ectx.pc = program.codes.size();
      return;
    }
    auto result = ctx->call(next, generator, {});
    if (result->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(result);
      ectx.pc = program.codes.size();
      return;
    }
    auto value = ctx->getField(result, L"value");
    if (value->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(value);
      ectx.pc = program.codes.size();
      return;
    }
    auto done = ctx->getField(result, L"done");
    if (done->getType() == JS_TYPE::EXCEPTION) {
      ectx.stack.push_back(done);
      ectx.pc = program.codes.size();
      return;
    }
    ectx.stack.push_back(value);
    ectx.stack.push_back(done);
  }
  void runAwaitNext(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runObjectSpread(JSContext *ctx, const JSProgram &program,
                       JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runArraySpread(JSContext *ctx, const JSProgram &program,
                      JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSuper(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSuperCall(JSContext *ctx, const JSProgram &program,
                    JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runThis(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    ectx.stack.push_back(ectx.self);
  }
  void runGetSuperField(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetSuperField(JSContext *ctx, const JSProgram &program,
                        JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runArgumentSpread(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    auto arr = ctx->createArray();
    size_t index = 0;
    while (!ectx.stack.empty()) {
      auto arg = *ectx.stack.rbegin();
      ectx.stack.pop_back();
      auto err = ctx->setIndex(arr, index, arg);
      if (err->getType() == JS_TYPE::EXCEPTION) {
        ectx.stack.push_back(err);
        ectx.pc = program.codes.size();
        return;
      }
    }
    ectx.stack.push_back(arr);
  }
  void runHlt(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    if (ectx.stack.empty()) {
      ectx.stack.push_back(ctx->createUndefined());
    }
    ectx.pc = program.codes.size();
  }
  void runDebugger(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSpread(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runMerge(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runGetKeys(JSContext *ctx, const JSProgram &program,
                  JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runTryBegin(JSContext *ctx, const JSProgram &program,
                   JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runTryEnd(JSContext *ctx, const JSProgram &program,
                 JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runDefer(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runThrow(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runEmptyCheck(JSContext *ctx, const JSProgram &program,
                     JSEvalContext &ectx) {
    if (ectx.stack.empty()) {
      ectx.stack.push_back(ctx->createUndefined());
    }
  }
  void runClass(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runWith(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  void runSetInitializer(JSContext *ctx, const JSProgram &program,
                         JSEvalContext &ectx) {
    // not implement
    ectx.pc = program.codes.size();
  }
  JSValue *run(JSContext *ctx, const JSProgram &program, JSEvalContext &ectx) {
    for (;;) {
      if (ectx.pc >= program.codes.size()) {
        break;
      }
      auto opt = (JS_OPERATOR)(program.codes[ectx.pc]);
      ectx.pc++;
      switch (opt) {
      case JS_OPERATOR::BEGIN:
        runBegin(ctx, program, ectx);
        break;
      case JS_OPERATOR::END:
        runEnd(ctx, program, ectx);
        break;
      case JS_OPERATOR::PUSH:
        runPush(ctx, program, ectx);
        break;
      case JS_OPERATOR::POP:
        runPop(ctx, program, ectx);
        break;
      case JS_OPERATOR::PUSH_VALUE:
        runPushValue(ctx, program, ectx);
        break;
      case JS_OPERATOR::NIL:
        runNil(ctx, program, ectx);
        break;
      case JS_OPERATOR::UNDEFINED:
        runUndefined(ctx, program, ectx);
        break;
      case JS_OPERATOR::TRUE:
        runTrue(ctx, program, ectx);
        break;
      case JS_OPERATOR::FALSE:
        runFalse(ctx, program, ectx);
        break;
      case JS_OPERATOR::REGEX:
        runRegex(ctx, program, ectx);
        break;
      case JS_OPERATOR::CLASS:
        runClass(ctx, program, ectx);
        break;
      case JS_OPERATOR::BIGINT:
        runBigint(ctx, program, ectx);
        break;
      case JS_OPERATOR::LOAD:
        runLoad(ctx, program, ectx);
        break;
      case JS_OPERATOR::STORE:
        runStore(ctx, program, ectx);
        break;
      case JS_OPERATOR::REF:
        runRef(ctx, program, ectx);
        break;
      case JS_OPERATOR::STR:
        runStr(ctx, program, ectx);
        break;
      case JS_OPERATOR::VAR:
        runVar(ctx, program, ectx);
        break;
      case JS_OPERATOR::CONST:
        runConst(ctx, program, ectx);
        break;
      case JS_OPERATOR::LET:
        runLet(ctx, program, ectx);
        break;
      case neo::JS_OPERATOR::OBJECT:
        runObject(ctx, program, ectx);
        break;
      case neo::JS_OPERATOR::ARRAY:
        runArray(ctx, program, ectx);
        break;
      case JS_OPERATOR::FUNCTION:
        runFunction(ctx, program, ectx);
        break;
      case JS_OPERATOR::ASYNCFUNCTION:
        runAsyncfunction(ctx, program, ectx);
        break;
      case JS_OPERATOR::GENERATOR:
        runGenerator(ctx, program, ectx);
        break;
      case JS_OPERATOR::ASYNCGENERATOR:
        runAsyncgenerator(ctx, program, ectx);
        break;
      case JS_OPERATOR::ENABLE:
        runEnable(ctx, program, ectx);
        break;
      case JS_OPERATOR::DISABLE:
        runDisable(ctx, program, ectx);
        break;
      case JS_OPERATOR::GET_FIELD:
        runGetField(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_FIELD:
        runSetField(ctx, program, ectx);
        break;
      case neo::JS_OPERATOR::SET_INITIALIZER:
        runSetInitializer(ctx, program, ectx);
        break;
      case JS_OPERATOR::CALL:
        runCall(ctx, program, ectx);
        break;
      case JS_OPERATOR::MEMBER_CALL:
        runMemberCall(ctx, program, ectx);
        break;
      case JS_OPERATOR::VOID:
        runVoid(ctx, program, ectx);
        break;
      case JS_OPERATOR::TYPEOF:
        runTypeof(ctx, program, ectx);
        break;
      case JS_OPERATOR::NEW:
        runNew(ctx, program, ectx);
        break;
      case JS_OPERATOR::DELETE:
        runDelete(ctx, program, ectx);
        break;
      case JS_OPERATOR::RET:
        runRet(ctx, program, ectx);
        break;
      case JS_OPERATOR::YIELD:
        runYield(ctx, program, ectx);
        break;
      case JS_OPERATOR::AWAIT:
        runAwait(ctx, program, ectx);
        break;
      case JS_OPERATOR::YIELD_DELEGATE:
        runYieldDelegate(ctx, program, ectx);
        break;
      case JS_OPERATOR::JMP:
        runJmp(ctx, program, ectx);
        break;
      case JS_OPERATOR::JTRUE:
        runJtrue(ctx, program, ectx);
        break;
      case JS_OPERATOR::JFALSE:
        runJfalse(ctx, program, ectx);
        break;
      case JS_OPERATOR::JNULL:
        runJnull(ctx, program, ectx);
        break;
      case JS_OPERATOR::JNOT_NULL:
        runJnotNull(ctx, program, ectx);
        break;
      case JS_OPERATOR::ADD:
        runAdd(ctx, program, ectx);
        break;
      case JS_OPERATOR::SUB:
        runSub(ctx, program, ectx);
        break;
      case JS_OPERATOR::DIV:
        runDiv(ctx, program, ectx);
        break;
      case JS_OPERATOR::MUL:
        runMul(ctx, program, ectx);
        break;
      case JS_OPERATOR::MOD:
        runMod(ctx, program, ectx);
        break;
      case JS_OPERATOR::POW:
        runPow(ctx, program, ectx);
        break;
      case JS_OPERATOR::AND:
        runAnd(ctx, program, ectx);
        break;
      case JS_OPERATOR::OR:
        runOr(ctx, program, ectx);
        break;
      case JS_OPERATOR::NOT:
        runNot(ctx, program, ectx);
        break;
      case JS_OPERATOR::XOR:
        runXor(ctx, program, ectx);
        break;
      case JS_OPERATOR::SHR:
        runShr(ctx, program, ectx);
        break;
      case JS_OPERATOR::SHL:
        runShl(ctx, program, ectx);
        break;
      case JS_OPERATOR::USHR:
        runUshr(ctx, program, ectx);
        break;
      case JS_OPERATOR::EQ:
        runEq(ctx, program, ectx);
        break;
      case JS_OPERATOR::SEQ:
        runSeq(ctx, program, ectx);
        break;
      case JS_OPERATOR::NE:
        runNe(ctx, program, ectx);
        break;
      case JS_OPERATOR::SNE:
        runSne(ctx, program, ectx);
        break;
      case JS_OPERATOR::GT:
        runGt(ctx, program, ectx);
        break;
      case JS_OPERATOR::LT:
        runLt(ctx, program, ectx);
        break;
      case JS_OPERATOR::GE:
        runGe(ctx, program, ectx);
        break;
      case JS_OPERATOR::LE:
        runLe(ctx, program, ectx);
        break;
      case JS_OPERATOR::INC:
        runInc(ctx, program, ectx);
        break;
      case JS_OPERATOR::DEC:
        runDec(ctx, program, ectx);
        break;
      case JS_OPERATOR::UPDATE_INC:
        runUpdateInc(ctx, program, ectx);
        break;
      case JS_OPERATOR::UPDATE_DEC:
        runUpdateDec(ctx, program, ectx);
        break;
      case JS_OPERATOR::NEXT:
        runNext(ctx, program, ectx);
        break;
      case JS_OPERATOR::AWAIT_NEXT:
        runAwaitNext(ctx, program, ectx);
        break;
      case JS_OPERATOR::OBJECT_SPREAD:
        runObjectSpread(ctx, program, ectx);
        break;
      case JS_OPERATOR::ARRAY_SPREAD:
        runArraySpread(ctx, program, ectx);
        break;
      case JS_OPERATOR::ARGUMENT_SPREAD:
        runArgumentSpread(ctx, program, ectx);
        break;
      case JS_OPERATOR::HLT:
        runHlt(ctx, program, ectx);
        break;
      case JS_OPERATOR::THIS:
        runThis(ctx, program, ectx);
        break;
      case JS_OPERATOR::SUPER:
        runSuper(ctx, program, ectx);
        break;
      case JS_OPERATOR::SUPER_CALL:
        runSuperCall(ctx, program, ectx);
        break;
      case JS_OPERATOR::DEBUGGER:
        runDebugger(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_SUPER_FIELD:
        runSetSuperField(ctx, program, ectx);
        break;
      case JS_OPERATOR::GET_SUPER_FIELD:
        runGetSuperField(ctx, program, ectx);
        break;
      case JS_OPERATOR::SPREAD:
        runSpread(ctx, program, ectx);
        break;
      case JS_OPERATOR::MERGE:
        runMerge(ctx, program, ectx);
        break;
      case JS_OPERATOR::GET_KEYS:
        runGetKeys(ctx, program, ectx);
        break;
      case JS_OPERATOR::TRY_BEGIN:
        runTryBegin(ctx, program, ectx);
        break;
      case JS_OPERATOR::TRY_END:
        runTryEnd(ctx, program, ectx);
        break;
      case JS_OPERATOR::DEFER:
        runDefer(ctx, program, ectx);
        break;
      case JS_OPERATOR::THROW:
        runThrow(ctx, program, ectx);
        break;
      case neo::JS_OPERATOR::EMPTY_CHECK:
        runEmptyCheck(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_GETTER:
        runSetGetter(ctx, program, ectx);
        break;
      case JS_OPERATOR::SET_SETTER:
        runSetSetter(ctx, program, ectx);
        break;
      case neo::JS_OPERATOR::WITH:
        runWith(ctx, program, ectx);
        break;
      }
    }
    if (ectx.stack.empty()) {
      return ctx->createUndefined();
    }
    return *ectx.stack.rbegin();
  }

public:
  JSVirtualMachine() {}
  ~JSVirtualMachine() {}

  JSValue *eval(JSContext *ctx, const JSProgram &program,
                JSEvalContext ectx = {}) {
    if (!ectx.self) {
      ectx.self = ctx->createUndefined();
    }
    return run(ctx, program, ectx);
  }
};

/*****************************************/
/* JSRuntime Implement                    */
/*****************************************/
inline JSRuntime::JSRuntime() {
  _parser = new JSParser();
  _generator = new JSCodeGenerator();
  _vm = new JSVirtualMachine();
}

inline JSRuntime::~JSRuntime() {
  if (_vm) {
    delete _vm;
    _vm = nullptr;
  }
  if (_generator) {
    delete _generator;
    _generator = nullptr;
  }
  if (_parser) {
    delete _parser;
    _parser = nullptr;
  }
}

/*****************************************/
/* JSString Implement                    */
/*****************************************/

inline JSValue *JSString::toString(JSContext *ctx) {
  return ctx->createString(_value);
}

/*****************************************/
/* JSNumber Implement                    */
/*****************************************/

inline JSValue *JSNumber::toString(JSContext *ctx) {
  std::wstringstream ss;
  ss << _value;
  return ctx->createString(ss.str());
}

/*****************************************/
/* JSBoolean Implement                   */
/*****************************************/

inline JSValue *JSBoolean ::toString(JSContext *ctx) {
  return ctx->createString(_value ? L"true" : L"false");
}

/*****************************************/
/* JSUndefined Implement                   */
/*****************************************/

inline JSValue *JSUndefined ::toString(JSContext *ctx) {
  return ctx->createString(L"undefined");
}

/*****************************************/
/* JSObject Implement                   */
/*****************************************/

inline JSValue *JSObject ::toString(JSContext *ctx) {
  return ctx->createString(L"[Object object]");
}

inline JSValue *JSObject ::getField(JSContext *ctx, JSValue *self,
                                    const std::wstring &name) {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field.value) {
      return ctx->getScope()->createValue(field.value);
    } else {
      auto fn = ctx->getScope()->createValue(field.getter);
      return ctx->call(fn, self, {});
    }
  }
  return ctx->createUndefined();
}

inline JSValue *JSObject ::setField(JSContext *ctx, JSValue *self,
                                    const std::wstring &name, JSValue *value) {
  if (_fields.contains(name)) {
    auto &field = _fields.at(name);
    if (field.value != nullptr) {
      if (!field.writable || isFrozen()) {
        return ctx->createException(
            L"TypeError: Cannot assign to read only property '" + name +
            L"' of object");
      }
      self->getAtom()->addChild(value->getAtom());
      self->getAtom()->removeChild(field.value);
      field.value = value->getAtom();
    } else {
      if (!field.setter) {
        return ctx->createException(
            L"TypeError: Cannot assign to read only property '" + name +
            L"' of object");
      }
      auto fn = ctx->getScope()->createValue(field.setter);
      return ctx->call(fn, self, {value});
    }
  } else {
    if (!isExtensible() || isFrozen() || isSealed()) {
      return ctx->createException(L"Cannot add property " + name +
                                  L", object is not extensible");
    }
    self->getAtom()->addChild(value->getAtom());
    _fields[name] = {.configurable = true,
                     .enumable = true,
                     .value = value->getAtom(),
                     .writable = true};
  }
  return value;
}

inline JSValue *JSObject::getField(JSContext *ctx, JSValue *self,
                                   JSValue *name) {
  if (name->getType() == JS_TYPE::STRING) {
    auto str = dynamic_cast<JSString *>(name->getData())->getValue();
    return getField(ctx, self, str);
  } else if (name->getType() == JS_TYPE::NUMBER) {
    auto val = dynamic_cast<JSNumber *>(name->getData())->getValue();
    return getIndex(ctx, self, (size_t)val);
  }
  if (name->getType() == JS_TYPE::SYMBOL) {
    if (_symboledFields.contains(name->getAtom())) {
      auto &field = _symboledFields.at(name->getAtom());
      if (field.value) {
        return ctx->getScope()->createValue(field.value);
      } else {
        auto fn = ctx->getScope()->createValue(field.getter);
        return ctx->call(fn, self, {});
      }
    }
  }
  return getField(ctx, self, name->getData()->toString(ctx));
}

inline JSValue *JSObject::setField(JSContext *ctx, JSValue *self, JSValue *name,
                                   JSValue *value) {
  if (name->getType() == JS_TYPE::STRING) {
    auto str = dynamic_cast<JSString *>(name->getData())->getValue();
    return setField(ctx, self, str, value);
  } else if (name->getType() == JS_TYPE::NUMBER) {
    auto idx = dynamic_cast<JSNumber *>(name->getData())->getValue();
    return setIndex(ctx, self, (size_t)idx, value);
  }
  if (name->getType() == JS_TYPE::SYMBOL) {
    auto namestr =
        dynamic_cast<JSString *>(name->getData()->toString(ctx)->getData())
            ->getValue();
    namestr = L"Symbol(" + namestr + L")";
    if (_symboledFields.contains(name->getAtom())) {
      auto &field = _symboledFields.at(name->getAtom());
      if (field.value != nullptr) {
        if (!field.writable || isFrozen()) {
          return ctx->createException(
              L"TypeError: Cannot assign to read only property '" + namestr +
              L"' of object");
        }
        self->getAtom()->addChild(value->getAtom());
        self->getAtom()->removeChild(field.value);
        field.value = value->getAtom();
      } else {
        if (!field.setter) {
          return ctx->createException(
              L"TypeError: Cannot assign to read only property '" + namestr +
              L"' of object");
        }
        auto fn = ctx->getScope()->createValue(field.setter);
        return ctx->call(fn, self, {value});
      }
    } else {
      if (!isExtensible() || isFrozen() || isSealed()) {
        return ctx->createException(L"Cannot add property " + namestr +
                                    L", object is not extensible");
      }
      self->getAtom()->addChild(value->getAtom());
      self->getAtom()->addChild(name->getAtom());
      _symboledFields[name->getAtom()] = {.configurable = true,
                                          .enumable = true,
                                          .value = value->getAtom(),
                                          .writable = true};
    }
  }
  return setField(ctx, self, name->getData()->toString(ctx), value);
}

inline JSValue *JSObject::getIndex(JSContext *ctx, JSValue *self,
                                   size_t index) {
  return getField(ctx, self, std::to_wstring(index));
}

inline JSValue *JSObject::setIndex(JSContext *ctx, JSValue *self, size_t index,
                                   JSValue *value) {
  return setField(ctx, self, std::to_wstring(index), value);
}

/*****************************************/
/* JSArray Implement                   */
/*****************************************/

inline JSValue *JSArray ::toString(JSContext *ctx) {
  std::wstringstream ss;
  for (size_t index = 0; index < _length; index++) {
    if (_items.contains(index)) {
      ss << ((JSString *)(_items[index]->getData()->toString(ctx)->getData()))
                ->getValue();
    }
    if (index != _length - 1) {
      ss << L",";
    }
  }
  return ctx->createString(ss.str());
}

inline JSValue *JSArray ::getIndex(JSContext *ctx, JSValue *self,
                                   size_t index) {
  if (_items.contains(index)) {
    return ctx->getScope()->createValue(_items.at(index));
  }
  return ctx->createUndefined();
}

inline JSValue *JSArray ::setIndex(JSContext *ctx, JSValue *self, size_t index,
                                   JSValue *value) {
  if (_items.contains(index)) {
    if (isFrozen()) {
      return ctx->createException(
          L"TypeError: Cannot assign to read only property '" +
          std::to_wstring(index) + L"' of object");
    }
    self->getAtom()->removeChild(_items.at(index));
    self->getAtom()->addChild(value->getAtom());
  } else {
    if (!isExtensible() || isFrozen() || isSealed()) {
      return ctx->createException(L"Cannot add property " +
                                  std::to_wstring(index) +
                                  L", object is not extensible");
    }
    self->getAtom()->addChild(value->getAtom());
  }
  _items[index] = value->getAtom();
  if (index >= _length) {
    _length = index + 1;
  }
  return value;
}
/*****************************************/
/* JSNull Implement                   */
/*****************************************/

inline JSValue *JSNull ::toString(JSContext *ctx) {
  return ctx->createString(L"null");
}

/*****************************************/
/* JSCallable Implement                   */
/*****************************************/

inline JSValue *JSCallable ::toString(JSContext *ctx) {
  auto name = _name;
  if (name.empty()) {
    name = L"anonymous";
  }
  return ctx->createString(L"[Function " + name + L"]");
}

/*****************************************/
/* JSFunction Implement                   */
/*****************************************/
inline JSValue *JSFunction::call(JSContext *ctx, JSValue *self,
                                 std::vector<JSValue *> args) {
  (void)_program;
  (void)_address;
  return ctx->createUndefined();
}

/*****************************************/
/* JSException Implement                   */
/*****************************************/

inline JSValue *JSException ::toString(JSContext *ctx) {
  std::wstringstream ss;
  ss << _message << std::endl;
  for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
    auto funcname = it->position.funcname;
    if (funcname.empty()) {
      funcname = L"anonymous";
    }
    if (it->filename.empty()) {
      ss << L"  at " << funcname << L"(<internal>)";
    } else {
      ss << L"  at " << funcname << L"(" << it->filename << L":"
         << it->position.line << L":" << it->position.column << L")\n";
    }
  }
  return ctx->createString(ss.str());
}

/*****************************************/
/* JSContext Implement                   */
/*****************************************/
inline JSValue *JSContext::eval(const std::wstring &filename,
                                const std::wstring &source) {
  auto root = _runtime->getParser()->parse(source);
  if (root->type == JS_NODE_TYPE::ERROR) {
    auto err = dynamic_cast<JSErrorNode *>(root);

    auto exception =
        createException(L"SyntaxError: " + err->message, filename,
                        root->location.end.column, root->location.end.line);
    delete root;
    return exception;
  }
  auto program = _runtime->getGenerator()->compile(filename, source, root);
  if (program.error) {
    auto err = dynamic_cast<JSErrorNode *>(program.error);
    auto exception =
        createException(L"SyntaxError: " + err->message, filename,
                        root->location.end.column, root->location.end.line);
    delete root;
    return exception;
  }
  std::wfstream out("./1.asm", std::ios_base::out);
  if (!out.is_open()) {
    return createException(L"cannot open 1.asm");
  }
  out << program.toString();
  out.close();
  auto vm = _runtime->getVirtualMachine();
  auto value = vm->eval(this, program);
  delete root;
  return value;
}

} // namespace neo
#endif