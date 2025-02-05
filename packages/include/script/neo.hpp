#ifndef _NEO_JS_HEADER_
#define _NEO_JS_HEADER_
#include <algorithm>
#include <codecvt>
#include <fmt/format.h>
#include <fmt/xchar.h>
#include <functional>
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

  EXPRESSION_UNARY,
  EXPRESSION_UPDATE,
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

  EXPRESSION_REST,

  PATTERN_REST_ITEM,
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
  EXPORT_ALL,

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
  }
  void addParent(JSNode *parent) {
    parent->children.push_back(this);
    this->parent = parent;
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

struct JSCommentLiteralNode : public JSNode {
  JSCommentLiteralNode() { type = JS_NODE_TYPE::LITERAL_COMMENT; }
};

struct JSMultilineCommentLiteralNode : public JSNode {
  JSMultilineCommentLiteralNode() {
    type = JS_NODE_TYPE::LITERAL_MULTILINE_COMMENT;
  }
};

struct JSProgramNode : public JSNode {
  JSNode *interpreter;
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

struct JSExpressionStatementNode : public JSNode {
  JSNode *expression;
  JSExpressionStatementNode() { type = JS_NODE_TYPE::STATEMENT_EXPRESSION; }
};

class JSParser {
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
  JSLocation getLocation(const std::wstring &filename,
                         const std::wstring &source, const JSPosition &start,
                         const JSPosition &end) {
    JSLocation loc;
    loc.start = {.filename = filename,
                 .funcname = L"neo.compile",
                 .line = 1,
                 .column = 1,
                 .offset = 0};
    while (loc.start.offset < start.offset) {
      if (isLineTerminatior(source[loc.start.offset])) {
        loc.start.column = 1;
        loc.start.line++;
      } else {
        loc.start.column++;
      }
      loc.start.offset++;
    }

    loc.end = {.filename = filename,
               .funcname = L"neo.compile",
               .line = 1,
               .column = 1,
               .offset = 0};
    while (loc.end.offset < end.offset) {
      if (isLineTerminatior(source[loc.end.offset])) {
        loc.end.column = 1;
        loc.end.line++;
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
  JSNode *readSymbolToken(const std::wstring &filename,
                          const std::wstring &source, JSPosition &position) {
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
        token->location = getLocation(filename, source, position, current);
        position = current;
        return token;
      }
    }
    return nullptr;
  }

  JSNode *readComments(const std::wstring &filename, const std::wstring &source,
                       JSPosition &position, JSNode *node) {
    auto comment = readComment(filename, source, position);
    while (comment != nullptr) {
      if (comment->type == JS_NODE_TYPE::ERROR) {
        return comment;
      }
      node->comments.push_back(comment);
      comment->addParent(node);
      while (skipInvisible(source, position)) {
      }
      comment = readComment(filename, source, position);
    }
    return nullptr;
  }

private:
  JSNode *readIdentifyLiteral(const std::wstring &filename,
                              const std::wstring &source,
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
      node->location = getLocation(filename, source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readStringLiteral(const std::wstring &filename,
                            const std::wstring &source, JSPosition &position) {

    if (source[position.offset] == '\'' || source[position.offset] == '\"') {
      auto current = position;
      current.offset++;
      for (;;) {
        if (source[current.offset] == source[position.offset]) {
          break;
        }
        if (!source[current.offset]) {
          auto *node = new JSErrorNode();
          node->message = L"Invalid or unexpected token";
          node->location = getLocation(filename, source, position, current);
          return node;
        }
        if (source[current.offset] == '\\') {
          current.offset++;
        }
        current.offset++;
      }
      current.offset++;
      auto token = new JSStringLiteralNode();
      token->location = getLocation(filename, source, position, current);
      position = current;
      return token;
    }
    return nullptr;
  }

  JSNode *readNumberLiteral(const std::wstring &filename,
                            const std::wstring &source, JSPosition &position) {
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
          node->location = getLocation(filename, source, position, current);
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
          node->location = getLocation(filename, source, position, current);
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
          node->location = getLocation(filename, source, position, current);
          position = current;
          return node;
        }
        while (source[current.offset] >= '0' && source[current.offset] <= '9') {
          current.offset++;
        }
        if (source[current.offset] == 'n') {
          current.offset++;
          auto node = new JSBigIntLiteralNode();
          node->location = getLocation(filename, source, position, current);
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
        auto node = new JSTokenNode();
        node->location = getLocation(filename, source, position, current);
        position = current;
        return node;
      }
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
        auto err = new JSErrorNode();
        err->message = L"Invalid or unexpected token";
        err->location = getLocation(filename, source, position, current);
        return err;
      }
      while (source[current.offset] >= '0' && source[current.offset] <= '9') {
        current.offset++;
      }
      auto node = new JSNumberLiteralNode();
      node->location = getLocation(filename, source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readRegexpLiteral(const std::wstring &filename,
                            const std::wstring &source, JSPosition &position) {
    auto current = position;
    if (source[current.offset] == '/') {
      current.offset++;
      for (;;) {
        if (source[current.offset] == '/') {
          break;
        }
        if (!source[current.offset]) {
          auto err = new JSErrorNode{};
          err->message = L"Invalid or unexpected token";
          err->location = getLocation(filename, source, position, current);
          return err;
        }
        if (source[current.offset] == '[') {
          for (;;) {
            if (source[current.offset == ']']) {
              break;
            }
            if (!source[current.offset]) {
              auto err = new JSErrorNode{};
              err->message = L"Invalid or unexpected token";
              err->location = getLocation(filename, source, position, current);
              return err;
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
      node->location = getLocation(filename, source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readProgram(const std::wstring &filename, const std::wstring &source,
                      JSPosition &position) {
    auto current = position;
    auto *node = new JSProgramNode();
    auto interpreter = readInterpreterDirective(filename, source, current);
    if (interpreter) {
      if (interpreter->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return interpreter;
      } else {
        node->interpreter = interpreter;
        interpreter->addParent(node);
      }
    }
    while (skipInvisible(source, current)) {
    }
    for (;;) {
      auto err = readComments(filename, source, current, node);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto directive = readDirective(filename, source, current);
      if (!directive) {
        break;
      }
      if (directive->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return directive;
      }
      node->directives.push_back(directive);
      directive->addParent(node);
      while (skipInvisible(source, current)) {
      }
    }
    for (;;) {
      auto err = readComments(filename, source, current, node);
      if (err != nullptr) {
        delete node;
        return err;
      }
      auto statement = readStatement(filename, source, current);
      if (!statement) {
        break;
      }
      if (statement->type == JS_NODE_TYPE::ERROR) {
        delete node;
        return statement;
      }
      node->body.push_back(statement);
      statement->addParent(node);
      while (skipInvisible(source, current)) {
      }
    }
    auto err = readComments(filename, source, current, node);
    if (err != nullptr) {
      delete node;
      return err;
    }
    node->location = getLocation(filename, source, position, current);
    while (skipInvisible(source, current)) {
    }
    position = current;
    return node;
  }

  JSNode *readInterpreterDirective(const std::wstring &filename,
                                   const std::wstring &source,
                                   JSPosition &position) {
    auto current = position;
    if (source[position.offset] == '#' && source[position.offset + 1] == '!') {
      auto *node = new JSInterpreterDirectiveNode();
      current.offset += 2;
      while (source[current.offset] &&
             !isLineTerminatior(source[current.offset])) {
        current.offset++;
      }
      node->location = getLocation(filename, source, position, current);
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readDirective(const std::wstring &filename,
                        const std::wstring &source, JSPosition &position) {
    auto current = position;
    auto node = readExpression(filename, source, current);
    if (!node || node->type == JS_NODE_TYPE::ERROR) {
      return node;
    }
    JS_NODE_TYPE type = node->type;
    delete node;
    if (type != JS_NODE_TYPE::LITERAL_STRING) {
      return nullptr;
    }
    node = new JSDirectiveNode();
    node->location = getLocation(filename, source, position, current);
    position = current;
    return node;
  }

  JSNode *readEmptyStatement(const std::wstring &filename,
                             const std::wstring &source, JSPosition &position) {
    if (source[position.offset] == ';') {
      auto current = position;
      current.offset++;
      auto node = new JSEmptyStatementNode{};
      node->location = getLocation(filename, source, position, current);
      return node;
    }
    return nullptr;
  }

  JSNode *readStatement(const std::wstring &filename,
                        const std::wstring &source, JSPosition &position) {
    auto current = position;
    JSNode *node = readEmptyStatement(filename, source, current);
    if (!node) {
      node = readExpressionStatement(filename, source, current);
    }
    if (node) {
      position = current;
      return node;
    }
    return nullptr;
  }

  JSNode *readExpressionStatement(const std::wstring &filename,
                                  const std::wstring &source,
                                  JSPosition &position) {
    auto current = position;
    auto expression = readExpression(filename, source, current);
    if (expression != nullptr) {
      if (expression->type == JS_NODE_TYPE::ERROR) {
        return expression;
      }
      auto node = new JSExpressionStatementNode{};
      node->expression = expression;
      return node;
    }
    return nullptr;
  }

  JSNode *readExpression(const std::wstring &filename,
                         const std::wstring &source, JSPosition &position) {
    return nullptr;
  }

  JSNode *readComment(const std::wstring &filename, const std::wstring &source,
                      JSPosition &position) {
    if (source[position.offset] == '/') {
      if (source[position.offset + 1] == '/') {
        auto current = position;
        while (!isLineTerminatior(source[current.offset])) {
          current.offset++;
        }
        auto node = new JSCommentLiteralNode();
        node->location = getLocation(filename, source, position, current);
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
          if (!source[current.offset]) {
            auto node = new JSErrorNode();
            node->message = L"Invalid or unexpected token";
            node->location = getLocation(filename, source, position, current);
            return node;
          }
          current.offset++;
        }
        auto node = new JSMultilineCommentLiteralNode();
        node->location = getLocation(filename, source, position, current);
        position = current;
        return node;
      }
    }
    return nullptr;
  }

public:
  JSNode *parse(const std::wstring &filename, const std::wstring &source) {
    JSPosition pos = {};
    return readProgram(filename, source, pos);
  }
};

class JSExecutor {
public:
  JSValue *exec(JSNode *node) { return nullptr; }
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
    auto root = _runtime->getParser()->parse(filename, source);
    if (root->type == JS_NODE_TYPE::ERROR) {
      auto err = dynamic_cast<JSErrorNode *>(root);
      auto message = err->message;
      auto msg =
          fmt::format(L"SyntaxError: {} \nat {}({}:{}:{})", message,
                      root->location.end.funcname, root->location.end.filename,
                      root->location.end.line, root->location.end.column);

      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      auto res = converter.to_bytes(msg);
      delete root;
      throw std::runtime_error(res);
    }
    auto res = _runtime->getExecutor()->exec(root);
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