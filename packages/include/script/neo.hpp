#ifndef _NEO_JS_HEADER_
#define _NEO_JS_HEADER_
#include <algorithm>
#include <array>
#include <codecvt>
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace neo {
class JSRuntime;
class JSContext;
class JSValue;

enum class JS_TYPE {
  UNDEFINED,
  NUMBER,
  STRING,
  BOOLEAN,
  BIGINT,
  OBJECT,
  FUNCTION,
  SYMBOL,
  EXCEPTION
};

struct JSPosition {
  std::wstring filename;
  std::wstring funcname;
  size_t line;
  size_t column;
  size_t offset;
};

struct JSLocation {
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
  std::wstring get(const std::wstring &source) {
    std::wstring result;
    for (auto i = start.offset; i < end.offset; i++) {
      result += source[i];
    }
    return result;
  }
};
enum class ACCESSOR_TYPE { GET, SET };

enum class JS_DECLARATION_TYPE { VAR, CONST, LET };

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

  THIS,

  SUPER,

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

  VARIABLE_DECLARATION,
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
  STATIC_BLOCK,

  IMPORT_DECLARATION,
  IMPORT_SPECIFIER,
  IMPORT_DEFAULT,
  IMPORT_NAMESPACE,
  IMPORT_ATTARTUBE,
  EXPORT_DECLARATION,
  EXPORT_DEFAULT,
  EXPORT_SPECIFIER,
  EXPORT_NAMED,
  EXPORT_ALL,

  DECLARATION_FUNCTION_ARGUMENT,
  DECLARATION_ARROW_FUNCTION,
  DECLARATION_FUNCTION,
  DECLARATION_FUNCTION_BODY,
  DECLARATION_PARAMETER,
  DECLARATION_OBJECT,
  DECLARATION_ARRAY,
  DECLARATION_CLASS,
};

struct JSNode {
  JSLocation location;
  JS_NODE_TYPE type = JS_NODE_TYPE::ERROR;
  JSNode *parent = nullptr;
  std::vector<JSNode *> children;
  std::vector<JSNode *> comments;
  virtual ~JSNode() {
    for (auto child : children) {
      delete child;
    }
    for (auto comment : comments) {
      delete comment;
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
};

struct JSThisNode : public JSNode {
  JSThisNode() { type = JS_NODE_TYPE::THIS; }
};

struct JSSuperNode : public JSNode {
  JSSuperNode() { type = JS_NODE_TYPE::SUPER; }
};

struct JSCommentLiteralNode : public JSNode {
  JSCommentLiteralNode() { type = JS_NODE_TYPE::LITERAL_COMMENT; }
};

struct JSMultilineCommentLiteralNode : public JSNode {
  JSMultilineCommentLiteralNode() {
    type = JS_NODE_TYPE::LITERAL_MULTILINE_COMMENT;
  }
};

struct JSProgramNode : public JSNode {
  JSNode *interpreter{};
  std::vector<JSNode *> directives;
  std::vector<JSNode *> body;
  JSProgramNode() { type = JS_NODE_TYPE::PROGRAM; }
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
};
struct JSDebuggerStatement : public JSNode {
  JSDebuggerStatement() { type = JS_NODE_TYPE::STATEMENT_DEBUGGER; }
};
struct JSReturnStatement : public JSNode {
  JSNode *value{};
  JSReturnStatement() { type = JS_NODE_TYPE::STATEMENT_RETURN; }
};
struct JSLabelStatement : public JSNode {
  JSNode *label{};
  JSNode *statement{};
  JSLabelStatement() { type = JS_NODE_TYPE::STATEMENT_LABEL; }
};
struct JSBreakStatement : public JSNode {
  JSNode *label{};
  JSBreakStatement() { type = JS_NODE_TYPE::STATEMENT_BREAK; }
};
struct JSContinueStatement : public JSNode {
  JSNode *label{};
  JSContinueStatement() { type = JS_NODE_TYPE::STATEMENT_CONTINUE; }
};
struct JSIfStatement : public JSNode {
  JSNode *condition{};
  JSNode *alternate{};
  JSNode *consequent{};
  JSIfStatement() { type = JS_NODE_TYPE::STATEMENT_IF; }
};
struct JSSwitchStatement : public JSNode {
  JSNode *condition{};
  std::vector<JSNode *> cases;
  JSSwitchStatement() { type = JS_NODE_TYPE::STATEMENT_SWITCH; }
};
struct JSSwitchCaseStatement : public JSNode {
  JSNode *match{};
  std::vector<JSNode *> statements;
  JSSwitchCaseStatement() { type = JS_NODE_TYPE::STATEMENT_SWITCH_CASE; }
};
struct JSThrowStatement : public JSNode {
  JSNode *value{};
  JSThrowStatement() { type = JS_NODE_TYPE::STATEMENT_THROW; }
};
struct JSTryStatement : public JSNode {
  JSNode *body{};
  JSNode *onerror{};
  JSNode *onfinish{};
  JSTryStatement() { type = JS_NODE_TYPE::STATEMENT_TRY; }
};
struct JSTryCatchStatement : public JSNode {
  JSNode *identifier{};
  JSNode *body{};
  JSTryCatchStatement() { type = JS_NODE_TYPE::STATEMENT_TRY_CATCH; }
};
struct JSWhileStatement : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSWhileStatement() { type = JS_NODE_TYPE::STATEMENT_WHILE; }
};
struct JSDoWhileStatement : public JSNode {
  JSNode *body{};
  JSNode *condition{};
  JSDoWhileStatement() { type = JS_NODE_TYPE::STATEMENT_DO_WHILE; }
};

struct JSForStatement : public JSNode {
  JSNode *init{};
  JSNode *condition{};
  JSNode *after{};
  JSNode *body{};
  JSForStatement() { type = JS_NODE_TYPE::STATEMENT_FOR; }
};

struct JSForInStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForInStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_IN; }
};
struct JSForOfStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForOfStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_OF; }
};

struct JSForAwaitOfStatement : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *body{};
  JSForAwaitOfStatement() { type = JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF; }
};

struct JSExpressionStatementNode : public JSNode {
  JSNode *expression{};
  JSExpressionStatementNode() { type = JS_NODE_TYPE::STATEMENT_EXPRESSION; }
};

struct JSBinaryExpressionNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *opt{};
  JSBinaryExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_BINARY; }
};
struct JSAssigmentExpressionNode : public JSNode {
  JSNode *left{};
  JSNode *right{};
  JSNode *opt{};
  JSAssigmentExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_ASSIGMENT; }
};

struct JSGroupExpressionNode : public JSNode {
  JSNode *expression{};
  JSGroupExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_GROUP; }
};

struct JSMemberExpressionNode : public JSNode {
  JSNode *host{};
  JSNode *field{};
  JSMemberExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_MEMBER; }
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
};

struct JSYieldExpressionNode : public JSNode {
  JSNode *value{};
  JSYieldExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_YIELD; }
};

struct JSYieldDelegateExpressionNode : public JSYieldExpressionNode {
  JSYieldDelegateExpressionNode() {
    type = JS_NODE_TYPE::EXPRESSION_YIELD_DELEGATE;
  }
};

struct JSDeleteExpressionNode : public JSNode {
  JSNode *value{};
  JSDeleteExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_DELETE; }
};

struct JSVoidExpressionNode : public JSNode {
  JSNode *value{};
  JSVoidExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_VOID; }
};

struct JSConditionExpressionNode : public JSNode {
  JSNode *condition{};
  JSNode *alternate{};
  JSNode *consequent{};
  JSConditionExpressionNode() { type = JS_NODE_TYPE::EXPRESSION_CONDITION; }
};

struct JSSpreadExpression : public JSNode {
  JSNode *value{};
  JSSpreadExpression() { type = JS_NODE_TYPE::EXPRESSION_SPREAD; }
};

struct JSObjectPropertyNode : public JSNode {
  JSNode *key{};
  JSNode *value{};
  bool computed{};
  JSObjectPropertyNode() { type = JS_NODE_TYPE::OBJECT_PROPERTY; }
};

struct JSObjectMethodNode : public JSNode {
  JSNode *key{};
  std::vector<JSNode *> arguments;
  JSNode *body{};
  bool async{};
  bool generator{};
  bool computed{};
  JSObjectMethodNode() { type = JS_NODE_TYPE::OBJECT_METHOD; }
};

struct JSObjectAccessorNode : public JSNode {
  JSNode *key{};
  std::vector<JSNode *> arguments;
  JSNode *body{};
  bool computed{};
  ACCESSOR_TYPE accessor;
  JSObjectAccessorNode() { type = JS_NODE_TYPE::OBJECT_ACCESSOR; }
};

struct JSObjectDeclarationNode : public JSNode {
  std::vector<JSNode *> properties;
  JSObjectDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_OBJECT; }
};

struct JSArrayDeclarationNode : public JSNode {
  std::vector<JSNode *> items;
  JSArrayDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_ARRAY; }
};

struct JSFunctionArgumentDeclarationNode : public JSNode {
  JSNode *identifier{};
  JSNode *value{};
  JSFunctionArgumentDeclarationNode() {
    type = JS_NODE_TYPE::DECLARATION_FUNCTION_ARGUMENT;
  }
};

struct JSFunctionBodyDeclarationNode : public JSNode {
  std::vector<JSNode *> directives;
  std::vector<JSNode *> statements;
  JSFunctionBodyDeclarationNode() {
    type = JS_NODE_TYPE::DECLARATION_FUNCTION_BODY;
  }
};

struct JSFunctionDeclarationNode : public JSNode {
  JSNode *name{};
  JSNode *body{};
  std::vector<JSNode *> arguments;
  bool async{};
  bool generator{};
  JSFunctionDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_FUNCTION; }
};
struct JSArrowDeclarationNode : public JSNode {
  JSNode *name{};
  JSNode *body{};
  std::vector<JSNode *> arguments;
  bool async{};
  JSArrowDeclarationNode() { type = JS_NODE_TYPE::DECLARATION_ARROW_FUNCTION; }
};

struct JSObjectPatternItemNode : public JSNode {
  JSNode *key{};
  JSNode *alias{};
  JSNode *value{};
  bool computed{};
  JSObjectPatternItemNode() { type = JS_NODE_TYPE::PATTERN_OBJECT_ITEM; }
};

struct JSObjectPatternNode : public JSNode {
  std::vector<JSNode *> items;
  JSObjectPatternNode() { type = JS_NODE_TYPE::PATTERN_OBJECT; }
};

struct JSArrayPatternItemNode : public JSNode {
  JSNode *value{};
  JSArrayPatternItemNode() { type = JS_NODE_TYPE::PATTERN_ARRAY_ITEM; }
};

struct JSArrayPatternNode : public JSNode {
  std::vector<JSNode *> items;
  JSArrayPatternNode() { type = JS_NODE_TYPE::PATTERN_ARRAY; }
};

struct JSSpreadPatternItemNode : public JSNode {
  JSNode *value{};
  JSSpreadPatternItemNode() { type = JS_NODE_TYPE::PATTERN_SPREAD_ITEM; }
};

struct JSVariableDeclaraion : public JSNode {
  JS_DECLARATION_TYPE kind = JS_DECLARATION_TYPE::CONST;
  std::vector<JSNode *> declarations{};
  JSVariableDeclaraion() { type = JS_NODE_TYPE::VARIABLE_DECLARATION; }
};

struct JSVariableDeclarator : public JSNode {
  JSNode *identifier{};
  JSNode *initialize{};
  JSVariableDeclarator() { type = JS_NODE_TYPE::VARIABLE_DECLARATOR; }
};

struct JSImportDeclaration : public JSNode {
  std::vector<JSNode *> specifiers;
  JSNode *source{};
  std::vector<JSNode *> attributes;
  JSImportDeclaration() { type = JS_NODE_TYPE::IMPORT_DECLARATION; }
};
struct JSImportSpecifier : public JSNode {
  JSNode *identifier;
  JSNode *alias;
  JSImportSpecifier() { type = JS_NODE_TYPE::IMPORT_SPECIFIER; }
};
struct JSImportDefault : public JSNode {
  JSNode *identifier;
  JSImportDefault() { type = JS_NODE_TYPE::IMPORT_DEFAULT; }
};
struct JSImportNamespace : public JSNode {
  JSNode *alias;
  JSImportNamespace() { type = JS_NODE_TYPE::IMPORT_NAMESPACE; }
};
struct JSImportAttribute : public JSNode {
  JSNode *key;
  JSNode *value;
  JSImportAttribute() { type = JS_NODE_TYPE::IMPORT_ATTARTUBE; }
};
struct JSExportDeclaration : public JSNode {
  std::vector<JSNode *> specifiers;
  JSNode *source;
  std::vector<JSNode *> attributes;
  JSExportDeclaration() { type = JS_NODE_TYPE::EXPORT_DECLARATION; }
};
struct JSExportDefault : public JSNode {
  JSNode *expression;
  JSExportDefault() { type = JS_NODE_TYPE::EXPORT_DEFAULT; }
};
struct JSExportSpecifier : public JSNode {
  JSNode *identifier;
  JSNode *alias;
  JSExportSpecifier() { type = JS_NODE_TYPE::EXPORT_SPECIFIER; }
};
struct JSExportNamed : public JSNode {
  JSNode *declaration;
  JSExportNamed() { type = JS_NODE_TYPE::EXPORT_NAMED; }
};
struct JSExportAll : public JSNode {
  JSNode *source;
  std::vector<JSNode *> attributes;
  JSExportAll() { type = JS_NODE_TYPE::EXPORT_ALL; }
};

class JSParser {
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
      return node;
    }
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
      return node;
    }
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
      auto node = new JSUndefinedLiteralNode{};
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
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
      node->body.push_back(statement);
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
    auto n = new JSBinaryExpressionNode{};
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
          delete n;
          delete node;
          return comment;
        }
        n->comments.push_back(comment);
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
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
      delete node;
      return createError(L"Invalid or nexcepted token", source, current);
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
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
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
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  JSNode *readLabelStatement(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto identifier = readIdentifyLiteral(source, current);
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
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
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
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
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
    if (!newline && !checkSymbol({L";"}, source, current) &&
        source[current.offset]) {
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
    auto n = new JSBinaryExpressionNode{};
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
          delete n;
          delete node;
          return comment;
        }
        n->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readVariableDeclaration(const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto node = new JSVariableDeclaraion{};
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
      auto backup = current;
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
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
    auto node = new JSVariableDeclarator{};
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
    auto node = new JSImportNamespace{};
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
    auto node = new JSImportDefault{};
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
    auto node = new JSImportSpecifier{};
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
    auto node = new JSImportAttribute{};
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
    auto node = new JSImportDeclaration{};
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      delete node;
      return err;
    }
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
            return createError(L"Invalid or unexpected token", source, current);
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
    auto n = new JSBinaryExpressionNode{};
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
          delete n;
          delete node;
          return comment;
        }
        n->comments.push_back(comment);
        newline |= isCommentNewline;
        continue;
      }
      break;
    }
    if (!checkSymbol({L";"}, source, current) && !newline &&
        source[current.offset]) {
      delete node;
      return createError(L"Invalid or unexpected token", source, current);
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
        return createError(L"Invalid or nexcepted token", source, current);
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
    auto argument = readFunctionArgument(source, current);
    while (argument) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
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
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readObjectAccessor(const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto node = new JSObjectAccessorNode{};
    if (checkIdentifier({L"get"}, source, current)) {
      node->accessor = ACCESSOR_TYPE::GET;
    } else if (checkIdentifier({L"set"}, source, current)) {
      node->accessor = ACCESSOR_TYPE::SET;
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
      auto identifier = readIdentifyLiteral(source, current);
      if (!identifier) {
        identifier = readStringLiteral(source, current);
      }
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
    } else {
      auto value = readSpreadPattern(source, current);
      if (!value) {
        delete node;
        return nullptr;
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return value;
      }
      node->alias = value;
      value->addParent(node);
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
    auto item = readPartPattern(source, current);
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
      item = readPartPattern(source, current);
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
      node->name = identifier;
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
    auto argument = readFunctionArgument(source, current);
    while (argument) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
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
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }

  JSNode *readArrowFunctionDeclaration(const std::wstring &source,
                                       JSPosition &position) {
    auto current = position;
    auto node = new JSArrowDeclarationNode;
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
          while (skipInvisible(source, current)) {
          };
          auto err = readComments(source, current, node->comments);
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
        delete node;
        return nullptr;
      }
    } else {
      auto identifier = readIdentifyLiteral(source, current);
      if (identifier == nullptr) {
        delete node;
        return nullptr;
      }
      node->arguments.push_back(identifier);
      identifier->addParent(node);
      while (skipInvisible(source, current)) {
      };
      auto err = readComments(source, current, node->comments);
      if (err) {
        delete node;
        return err;
      }
      if (!checkSymbol({L"=>"}, source, current)) {
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
      node = readObjectDeclaration(source, position);
    }
    if (!node) {
      node = readArrayDeclaration(source, position);
    }
    if (!node) {
      node = readFunctionDeclaration(source, position);
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
    auto node = new JSSpreadExpression{};
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
          optional = true;
        }
        if (!next) {
          next = readOptionalComputedMemberExpression(source, current, node);
          optional = true;
        }
        if (!next) {
          next = readOptionalCallExpression(source, current, node);
          optional = true;
        }
        if (!next) {
          next = readTemplateLiteral(source, current, node);
          if (next && next->type != JS_NODE_TYPE::ERROR) {
            if (optional) {
              delete next;
              current = next->location.start;
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
    auto node = new JSBinaryExpressionNode{};
    node->opt = token;
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
    node->right = value;
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
  JSNode *parse(const std::wstring &source) {
    JSPosition pos = {};
    return readProgram(source, pos);
  }
};

class JSExecutor {
private:
  std::wstring print(const std::wstring &filename, const std::wstring &source,
                     JSNode *node) {
    std::wstring type;
    switch (node->type) {
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
    case JS_NODE_TYPE::THIS:
      type = L"THIS";
      break;
    case JS_NODE_TYPE::SUPER:
      type = L"SUPER";
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
    case JS_NODE_TYPE::VARIABLE_DECLARATION:
      type = L"VARIABLE_DECLARATION";
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
    case JS_NODE_TYPE::STATIC_BLOCK:
      type = L"STATIC_BLOCK";
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
    case JS_NODE_TYPE::EXPORT_ALL:
      type = L"EXPORT_ALL";
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
    case JS_NODE_TYPE::DECLARATION_PARAMETER:
      type = L"DECLARATION_PARAMETER";
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
    std::wstring result = LR"({"type":")" + type + LR"(","children":[)";
    for (size_t i = 0; i < node->children.size(); i++) {
      if (i != 0) {
        result += L",";
      }
      result += print(filename, source, node->children[i]);
    }
    result += LR"(],"source":")";
    std::wstring src = node->location.get(source);
    for (auto &ch : src) {
      if (ch == '\n') {
        result += L"\\n";
      } else if (ch == '\t') {
        result += L"\\t";
      } else if (ch == '\r') {
        result += L"\\r";
      } else if (ch == '\"') {
        result += L"\\\"";
      } else {
        result += ch;
      }
    }
    result += LR"("})";
    return result;
  }

public:
  JSValue *exec(const std::wstring &filename, const std::wstring &source,
                JSNode *node) {
    std::wcout << print(filename, source, node) << std::endl;
    return nullptr;
  }
};

class JSScope;
class JSAtom;

class JSBase {
public:
  JSBase() {}
  virtual ~JSBase() {}

  virtual inline JSValue *toString(JSContext *ctx) = 0;
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

  JSAtom() : _data(nullptr) {}

  ~JSAtom() {
    if (_data != nullptr) {
      delete _data;
      _data = nullptr;
    }
    for (auto child : _children) {
      removeChild(child);
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
    case JS_TYPE::EXCEPTION:
      return L"exception";
    }
    return L"unknown";
  }

public:
  static void gc() {
    std::vector<JSAtom *> cache = {};
    std::vector<JSAtom *> disposed = {};
    while (!_destroyed.empty()) {
      auto item = *_destroyed.begin();
      _destroyed.erase(_destroyed.begin());
      if (std::find(cache.begin(), cache.end(), item) == cache.end()) {
        cache.push_back(item);
        std::vector<JSAtom *> workflow = {item};
        for (;;) {
          if (workflow.empty()) {
            disposed.push_back(item);
            while (!item->_children.empty()) {
              auto child = *item->_children.begin();
              item->removeChild(child);
            }
            break;
          }
          auto item = *workflow.begin();
          workflow.erase(workflow.begin());
          if (std::find(disposed.begin(), disposed.end(), item) !=
              disposed.end()) {
            continue;
          }
          if (std::find(cache.begin(), cache.end(), item) == cache.end()) {
            auto alive = false;
            for (auto parent : item->_parents) {
              if (parent->_data != nullptr) {
                alive = true;
                break;
              }
              workflow.push_back(parent);
            }
            if (alive) {
              break;
            }
          }
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
  std::wstring _name;

public:
  JSValue(JSAtom *atom, const std::wstring &name = L"")
      : _atom(atom), _name(name) {}

  ~JSValue() {}

  JSAtom *getAtom() { return _atom; }

  const JSAtom *getAtom() const { return _atom; }

  void setAtom(JSAtom *atom) { _atom = atom; }

  const std::wstring &getName() const { return _name; }

  const JS_TYPE &getType() const { return _atom->getType(); }
};

class JSScope {
private:
  JSScope *_parent;
  JSAtom *_root;
  std::vector<JSScope *> _children;
  std::unordered_map<std::wstring, JSValue *> _namedVariables;
  std::vector<JSValue *> _variables;

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
    _namedVariables.clear();
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
  }

  JSAtom *createAtom(const JS_TYPE &type, JSBase *data) {
    return new JSAtom(_root, type, data);
  }

  JSValue *createValue(const std::wstring &name, JSAtom *atom) {
    _root->addChild(atom);
    JSValue *value = nullptr;
    if (!name.empty() && _namedVariables.contains(name)) {
      value = _namedVariables.at(name);
      value->setAtom(atom);
    } else {
      value = new JSValue(atom, name);
      _variables.push_back(value);
      if (!name.empty()) {
        _namedVariables[name] = value;
      }
    }
    return value;
  }

  JSValue *createValue(const std::wstring &name, const JS_TYPE &type,
                       JSBase *val) {
    auto atom = createAtom(type, val);
    JSValue *value = nullptr;
    if (!name.empty() && _namedVariables.contains(name)) {
      value = _namedVariables.at(name);
      value->setAtom(atom);
    } else {
      value = new JSValue(atom, name);
      _variables.push_back(value);
      if (!name.empty()) {
        _namedVariables[name] = value;
      }
    }
    return value;
  }

  JSValue *queryValue(const std::wstring &name) {
    if (_namedVariables.contains(name)) {
      return _namedVariables.at(name);
    }
    return nullptr;
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
  bool configurable;
  bool enumable;

  JSAtom *value;
  bool writable;

  JSAtom *getter;
  JSAtom *setter;
};

class JSObject : public JSBase {
private:
  std::unordered_map<std::wstring, JSField> _fields;

  JSAtom *_prototype;

public:
  JSObject(JSAtom *prototype) : _prototype(prototype) {}

  JSAtom *getPrototype() { return _prototype; }

  const JSAtom *getPrototype() const { return _prototype; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSArray : public JSObject {
private:
  std::unordered_map<size_t, JSAtom *> _items;

public:
  JSArray(JSAtom *prototype) : JSObject(prototype) {}

  inline JSValue *toString(JSContext *ctx) override;
};

class JSNull : public JSObject {
public:
  JSNull() : JSObject(nullptr) {}
  inline JSValue *toString(JSContext *ctx) override;
};

class JSCallable : public JSObject {
public:
  std::unordered_map<std::wstring, JSAtom *> _closure;

public:
  JSCallable(JSAtom *prototype,
             const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSObject(prototype), _closure(closure){};
  const std::unordered_map<std::wstring, JSAtom *> &getClosure() const {
    return _closure;
  }

  virtual JSValue *call(JSContext *ctx, JSValue *self,
                        const std::vector<JSValue *> args) = 0;

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSNativeFunction : public JSCallable {
private:
  JS_NATIVE _native;

public:
  JSNativeFunction(JSAtom *prototype, const JS_NATIVE &native,
                   const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSCallable(prototype, closure), _native(native) {}
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) {
    return _native(ctx, self, args);
  }
};

class JSFunction : public JSCallable {};

class JSException : public JSBase {
private:
  std::wstring _message;
  std::vector<JSPosition> _stack;

public:
  JSException(const std::wstring &message, const std::vector<JSPosition> &stack)
      : _message(message), _stack(stack){};

  const std::wstring &getMessage() const { return _message; };

  const std::vector<JSPosition> &getStack() const { return _stack; }

public:
  inline JSValue *toString(JSContext *ctx) override;
};

class JSRuntime {
private:
  JSParser *_parser;
  JSExecutor *_executor;

public:
  JSRuntime() {
    _parser = new JSParser;
    _executor = new JSExecutor;
  }
  ~JSRuntime() {}
  JSParser *getParser() { return _parser; }
  JSExecutor *getExecutor() { return _executor; }
};

class JSContext {
private:
  JSRuntime *_runtime;
  JSScope *_root;
  JSScope *_current;
  std::vector<JSPosition> _callStack;

private:
  JSValue *_null;
  JSValue *_undefined;

public:
  JSContext(JSRuntime *runtime) : _runtime(runtime) {
    _root = new JSScope();
    _current = _root;
    _callStack.push_back({.filename = L"",
                          .funcname = L"neo.eval",
                          .line = 0,
                          .column = 0,
                          .offset = 0});
    _null = _current->createValue(L"", JS_TYPE::OBJECT, new JSNull{});
    _undefined = _current->createValue(L"", JS_TYPE::OBJECT, new JSUndefined{});
  }

  ~JSContext() {
    _current = nullptr;
    delete _root;
    _root = nullptr;
    _runtime = nullptr;
  }

  JSValue *eval(const std::wstring &filename, const std::wstring &source) {
    auto root = _runtime->getParser()->parse(source);
    if (root->type == JS_NODE_TYPE::ERROR) {
      auto err = dynamic_cast<JSErrorNode *>(root);
      auto message = err->message;
      auto msg =
          fmt::format(L"SyntaxError: {} \nat {}({}:{}:{})", message,
                      root->location.end.funcname, filename,
                      root->location.end.line, root->location.end.column);

      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      auto res = converter.to_bytes(msg);
      delete root;
      throw std::runtime_error(res);
    }
    auto res = _runtime->getExecutor()->exec(filename, source, root);
    delete root;
    return res;
  }

  const std::vector<JSPosition> &getCallStack() const { return _callStack; }

  void pushCallStack(const JSPosition &pos) {
    _callStack.rbegin()->filename = pos.filename;
    _callStack.rbegin()->line = pos.line;
    _callStack.rbegin()->column = pos.column;
    _callStack.push_back({.filename = L"",
                          .funcname = pos.funcname,
                          .line = 0,
                          .column = 0,
                          .offset = 0});
  }
  void popCallStack() {
    _callStack.pop_back();
    _callStack.rbegin()->filename = L"";
    _callStack.rbegin()->column = 0;
    _callStack.rbegin()->line = 0;
  }
  std::vector<JSPosition> trace(const std::wstring &filename, size_t column,
                                size_t line) const {
    std::vector<JSPosition> stack = _callStack;
    stack.rbegin()->filename = filename;
    stack.rbegin()->column = column;
    stack.rbegin()->line = line;
    return stack;
  }

  void pushScope() { _current = new JSScope(_current); }

  void popScope() {
    auto parent = _current->getParent();
    parent->removeChild(_current);
    delete _current;
    _current = parent;
  }

  JSValue *createUndefined(const std::wstring &name) {
    return _current->createValue(name, _undefined->getAtom());
  }

  JSValue *createNull(const std::wstring &name) {
    return _current->createValue(name, _null->getAtom());
  }

  JSValue *createNumber(const std::wstring &name, double value) {
    return _current->createValue(name, JS_TYPE::NUMBER, new JSNumber{value});
  }

  JSValue *createString(const std::wstring &name, const std::wstring &value) {
    return _current->createValue(name, JS_TYPE::STRING, new JSString{value});
  }

  JSValue *createBoolean(const std::wstring &name, bool value) {
    return _current->createValue(name, JS_TYPE::BOOLEAN, new JSBoolean{value});
  }

  JSValue *createObject(const std::wstring &name) {
    return _current->createValue(name, JS_TYPE::OBJECT, new JSObject{nullptr});
  }

  JSValue *createArray(const std::wstring &name) {
    return _current->createValue(name, JS_TYPE::OBJECT, new JSArray{nullptr});
  }

  JSValue *createNativeFunction(
      const std::wstring &name, const JS_NATIVE &func,
      const std::unordered_map<std::wstring, JSValue *> &closure = {}) {
    std::unordered_map<std::wstring, JSAtom *> clo;
    for (auto &[n, val] : closure) {
      clo[n] = val->getAtom();
    }
    auto val = _current->createValue(name, JS_TYPE::FUNCTION,
                                     new JSNativeFunction{nullptr, func, clo});
    for (auto &[n, atom] : clo) {
      val->getAtom()->addChild(atom);
    }
    return val;
  }

  JSValue *createException(const std::wstring &name,
                           const std::wstring &message,
                           const std::wstring &filename, size_t column,
                           size_t line) {
    return _current->createValue(
        name, JS_TYPE::EXCEPTION,
        new JSException{message, trace(filename, column, line)});
  }

  JSValue *createUndefined() { return _undefined; }

  JSValue *createNull() { return _null; }

  JSValue *createNumber(double value) { return createNumber(L"", value); }

  JSValue *createString(const std::wstring &value) {
    return createString(L"", value);
  }

  JSValue *createBoolean(bool value) { return createBoolean(L"", value); }

  JSValue *createObject() { return createObject(L""); }

  JSValue *createArray() { return createArray(L""); }

  JSValue *createNativeFunction(
      const JS_NATIVE &func,
      const std::unordered_map<std::wstring, JSValue *> &closure = {}) {
    return createNativeFunction(L"", func, closure);
  }

  JSValue *createException(const std::wstring &message,
                           const std::wstring &filename, size_t column,
                           size_t line) {
    return createException(L"", message, filename, column, line);
  }

  JSValue *call(JSValue *func, JSValue *self,
                const std::vector<JSValue *> args) {
    auto current = _current;
    pushScope();
    auto fn = const_cast<JSCallable *>(
        dynamic_cast<const JSCallable *>(func->getAtom()->getData()));
    for (auto &[key, val] : fn->getClosure()) {
      _current->createValue(key, val);
    }
    for (auto &arg : args) {
      _current->createValue(arg->getName(), arg->getAtom());
    }
    auto res = fn->call(this, self, args);
    auto result = current->createValue(res->getName(), res->getAtom());
    popScope();
    return result;
  }
};
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
  return ctx->createString(fmt::format(L"{}", _value));
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

/*****************************************/
/* JSArray Implement                   */
/*****************************************/

inline JSValue *JSArray ::toString(JSContext *ctx) {
  return ctx->createString(L"[Array array]");
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
  return ctx->createString(L"[Function function]");
}

/*****************************************/
/* JSException Implement                   */
/*****************************************/

inline JSValue *JSException ::toString(JSContext *ctx) {
  std::wstring result = fmt::format(L"{}\n", _message);
  for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
    auto funcname = it->funcname;
    if (funcname.empty()) {
      funcname = L"anonymous";
    }
    if (it->filename.empty()) {
      result += fmt::format(L" at {}(<internal>)", funcname);
    } else {
      result += fmt::format(L" at {}({}:{}:{})\n", funcname, it->filename,
                            it->line, it->column);
    }
  }
  return ctx->createString(result);
}

} // namespace neo
#endif