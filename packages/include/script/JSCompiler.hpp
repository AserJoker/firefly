#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Type.hpp"
#include <codecvt>
#include <fmt/format.h>
#include <locale>
#include <sstream>
#include <string>
namespace firefly::script {
class JSCompiler : public core::Object {
public:
  enum class NodeType {
    PRIVATE_NAME, // TODO:

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

    LITERAL_BIGINT,  // TODO:
    LITERAL_DECIMAL, // TODO:

    PROGRAM,
    STATEMENT_BLOCK,
    STATEMENT_DEBUGGER,    // TODO:
    STATEMENT_WITH,        // TODO:
    STATEMENT_RETURN,      // TODO:
    STATEMENT_LABEL,       // TODO:
    STATEMENT_BREAK,       // TODO:
    STATEMENT_CONTINUE,    // TODO:
    STATEMENT_IF,          // TODO:
    STATEMENT_SWITCH,      // TODO:
    STATEMENT_SWITCH_CASE, // TODO:
    STATEMENT_THROW,       // TODO:
    STATEMENT_TRY,         // TODO:
    STATEMENT_CACHE,       // TODO:
    STATEMENT_WHILE,       // TODO:
    STATEMENT_DO_WHILE,    // TODO:
    STATEMENT_FOR,         // TODO:
    STATEMENT_FOR_IN,      // TODO:
    STATEMENT_FOR_OF,      // TODO:

    VARIABLE_DECLARATION,
    VARIABLE_DECLARATOR,

    DECORATOR, // TODO:

    DIRECTIVE,
    DIRECTIVE_LITERAL,
    INTERPRETER_DIRECTIVE,

    OBJECT_MEMBER,   // TODO:
    OBJECT_PROPERTY, // TODO:
    OBJECT_METHOD,   // TODO:

    EXPRESSION_RECORD, // TODO:
    EXPRESSION_TUPLE,  // TODO:

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
    EXPRESSION_NEW,    // TODO:
    EXPRESSION_DELETE, // TODO:
    EXPRESSION_GROUP,

    EXPRESSION_YIELD, // TODO:
    EXPRESSION_AWAIT, // TODO:
    EXPRESSION_REST,  // TODO:

    PATTERN_OBJECT,
    PATTERN_OBJECT_ITEM,
    PATTERN_ARRAY,
    PATTERN_ARRAY_ITEM,
    PATTERN_REST,
    PATTERN_ASSIGMENT,

    CLASS_BODY,              // TODO:
    CLASS_METHOD,            // TODO:
    CLASS_PRIVATE_METHOD,    // TODO:
    CLASS_PROPERTY,          // TODO:
    CLASS_PRIVATE_PROPERTY,  // TODO:
    CLASS_ACCESSOR_PROPERTY, // TODO:
    STATIC_BLOCK,            // TODO:
    CLASS_DECLARATION,       // TODO:

    IMPORT_DECLARATION, // TODO:
    IMPORT_SPECIFIER,   // TODO:
    IMPORT_DEFAULT,     // TODO:
    IMPORT_NAMESPACE,   // TODO:
    IMPORT_ATTARTUBE,   // TODO:
    EXPORT_DECLARATION, // TODO:
    EXPORT_NAMED,       // TODO:
    EXPORT_DEFAULT,     // TODO:
    EXPORT_NAMESPACE,   // TODO:
    EXPORT_ALL,         // TODO:

    DECLARATION_ARROW_FUNCTION,
    DECLARATION_FUNCTION,
    DECLARATION_PARAMETER,
    DECLARATION_REST_PARAMETER,
    DECLARATION_OBJECT,
    DECLARATION_ARRAY,
    DECLARATION_CLASS, // TODO:
  };

  struct Position {
    core::Unsigned_t line;
    core::Unsigned_t column;
    core::Unsigned_t offset;
    std::string toJSON() {
      return fmt::format("{{\"line\":{},\"column\":{},\"offset\":{}}}",
                         line + 1, column + 1, offset);
    }
  };

  struct Location {
    Position start;
    Position end;
    std::wstring source;
    std::string toJSON(const std::wstring &source) {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      auto val = converter.to_bytes(getSource(source));
      std::string result = "";
      for (auto &ch : val) {
        if (ch == '\"') {
          result += "\\\"";
        } else if (ch == '\n') {
          result += "\\n";
        } else if (ch == '\r') {
          result += "\\r";
        } else {
          result += ch;
        }
      }
      return fmt::format("{{\"start\":{},\"end\":{},\"source\":\"{}\"}}",
                         start.toJSON(), end.toJSON(), result);
    }
    std::wstring getSource(const std::wstring &source) {
      return source.substr(start.offset, end.offset - start.offset + 1);
    }
  };

  struct Token : public core::Object {
    Location location;
  };

  struct Node : public core::Object {
    NodeType type;
    Location location;
    int32_t level;
    virtual std::string toJSON(const std::wstring &source) = 0;
  };

  struct Program : public Node {
    core::AutoPtr<Node> interpreter;
    core::Array<core::AutoPtr<Node>> directives;
    core::Array<core::AutoPtr<Node>> body;
    std::string toJSON(const std::wstring &source) override {
      std::stringstream ss;
      ss << "{";
      ss << "\"type\":\"PROGRAM\",";
      if (interpreter != nullptr) {
        ss << "\"interpreter\":" << interpreter->toJSON(source) << ",";
      }
      ss << "\"directives\":[";
      size_t index = 0;
      for (auto &directive : directives) {
        ss << directive->toJSON(source);
        if (index != directives.size() - 1) {
          ss << ",";
        }
        index++;
      }
      ss << "],";
      ss << "\"body\":[";
      index = 0;
      for (auto &statement : body) {
        ss << statement->toJSON(source);
        if (index != body.size() - 1) {
          ss << ",";
        }
        index++;
      }
      ss << "]";
      ss << "}";
      return ss.str();
    }
  };

  struct StringLiteral : public Node {
    std::wstring value;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::string val = converter.to_bytes(value);
      std::string result = "";
      for (auto &ch : val) {
        if (ch == '\"') {
          result += "\\\"";
        } else if (ch == '\n') {
          result += "\\n";
        } else if (ch == '\r') {
          result += "\\r";
        } else {
          result += ch;
        }
      }
      return fmt::format(
          "{{\"type\":\"LITERAL_STRING\",\"location\":{},\"value\":\"{}\"}}",
          location.toJSON(source), result);
    };
  };

  struct NumberLiteral : public Node {
    double value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"LITERAL_NUMBER","location":{},"value":{:g}}})",
          location.toJSON(source), value);
    }
  };

  struct RegexLiteral : public Node {
    std::wstring value;
    bool hasIndices{};
    bool global{};
    bool ignoreCase{};
    bool multiline{};
    bool dotAll{};
    bool sticky{};
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::string val = converter.to_bytes(value);
      std::string result = "";
      for (auto &ch : val) {
        if (ch == '\"') {
          result += "\\\"";
        } else if (ch == '\n') {
          result += "\\n";
        } else if (ch == '\r') {
          result += "\\r";
        } else {
          result += ch;
        }
      }
      return fmt::format(
          R"({{"type":"LITERAL_REGEX","location":{},"value":"{}","hasIndices":{},"global":{},"ignoreCase":{},"multiline":{},"dotAll":{},"sticky":{}}})",
          location.toJSON(source), result, hasIndices, global, ignoreCase,
          multiline, dotAll, sticky);
    }
  };

  struct BooleanLiteral : public Node {
    bool value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"LITERAL_BOOLEAN","value":{},"location":{}}})", value,
          location.toJSON(source));
    }
  };

  struct NullLiteral : public Node {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(R"({{"type":"LITERAL_NULL","location":{}}})",
                         location.toJSON(source));
    }
  };

  struct UndefinedLiteral : public Node {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(R"({{"type":"LITERAL_UNDEFINED","location":{}}})",
                         location.toJSON(source));
    }
  };

  struct IdentifierLiteral : public Node {
    std::wstring value;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::string val = converter.to_bytes(value);
      std::string result = "";
      for (auto &ch : val) {
        if (ch == '\"') {
          result += "\\\"";
        } else if (ch == '\n') {
          result += "\\n";
        } else if (ch == '\r') {
          result += "\\r";
        } else {
          result += ch;
        }
      }
      return fmt::format(
          R"({{"type":"LITERAL_IDENTITY","value":"{}","location":{}}})", result,
          location.toJSON(source));
    }
  };

  struct TemplateLiteral : public Node {
    std::wstring tag;
    core::Array<std::wstring> quasis;
    core::Array<core::AutoPtr<Node>> expressions;
    std::string toJSON(const std::wstring &source) override {
      std::string patterns;
      std::string exps;
      size_t index = 0;
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      for (auto &value : quasis) {
        std::string val = converter.to_bytes(value);
        std::string result = "";
        for (auto &ch : val) {
          if (ch == '\"') {
            result += "\\\"";
          } else if (ch == '\n') {
            result += "\\n";
          } else if (ch == '\r') {
            result += "\\r";
          } else {
            result += ch;
          }
        }
        patterns += fmt::format(R"("{}")", result);
        if (index != quasis.size() - 1) {
          patterns += ",";
        }
        index++;
      }
      index = 0;
      for (auto &value : expressions) {
        exps += value->toJSON(source);
        if (index != expressions.size() - 1) {
          exps += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"LITERAL_TEMPLATE","quasis":[{}],"expressions":[{}],"location":{}}})",
          patterns, exps, location.toJSON(source));
    }
  };

  struct Comment : public Node {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format("{{\"type\":\"LITERAL_STRING\",\"location\":{}}}",
                         location.toJSON(source));
    }
  };

  struct InterpreterDirective : public Node {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          "{{\"type\":\"INTERPRETER_DIRECTIVE\",\"location\":{}}}",
          location.toJSON(source));
    };
  };

  struct Directive : public Node {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format("{{\"type\":\"DIRECTIVE\",\"location\":{}}}",
                         location.toJSON(source));
    };
  };

  struct Statement : public Node {};

  struct BlockStatement : public Statement {
    core::Array<core::AutoPtr<Node>> body;
    std::string toJSON(const std::wstring &source) override {
      std::string str;
      size_t index = 0;
      for (auto &item : body) {
        str += item->toJSON(source);
        if (index != body.size() - 1) {
          str += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"STATEMENT_BLOCK","body":[{}],"location":{}}})", str,
          location.toJSON(source));
    }
  };

  struct BinaryExpression : public Node {
    core::AutoPtr<Node> left;
    core::AutoPtr<Node> right;
    std::wstring opt;
    virtual Position getStart() {
      if (left != nullptr) {
        auto l = left.cast<BinaryExpression>();
        if (l != nullptr) {
          return l->getStart();
        } else {
          return left->location.start;
        }
      }
      return location.start;
    }
    virtual Position getEnd() {
      if (right != nullptr) {
        auto r = right.cast<BinaryExpression>();
        if (r != nullptr) {
          return r->getEnd();
        } else {
          return right->location.end;
        }
      }
      return location.end;
    }
    void updateLocation(const std::wstring &source) {
      auto l = left.cast<BinaryExpression>();
      auto r = right.cast<BinaryExpression>();
      if (l != nullptr) {
        l->updateLocation(source);
      }
      if (r != nullptr) {
        r->updateLocation(source);
      }
      location = {getStart(), getEnd()};
      location.source = location.getSource(source);
    }
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_BINARY","operator":"{}","left":{},"right":{},"location":{}}})",
          converter.to_bytes(opt), left->toJSON(source), right->toJSON(source),
          location.toJSON(source));
    }
  };

  struct UpdateExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_UPDATE","left":{},"location":{},"operator":"{}"}})",
          left->toJSON(source), location.toJSON(source),
          converter.to_bytes(opt));
    }
  };

  struct UnaryExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_UNARY","right":{},"location":{},"operator":"{}"}})",
          right->toJSON(source), location.toJSON(source),
          converter.to_bytes(opt));
    }
  };

  struct GroupExpression : public Node {
    core::AutoPtr<Node> expression;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_GROUP","left":{},"location":{}}})",
          expression->toJSON(source), location.toJSON(source));
    }
  };

  struct ConditionExpression : public BinaryExpression {

    std::string toJSON(const std::wstring &source) override {
      auto r = right.cast<BinaryExpression>();
      return fmt::format(
          R"({{"type":"EXCEPTION_CONDITION","condition":{},"alternate":{},"consequent":{}}})",
          left->toJSON(source), r->left->toJSON(source),
          r->right->toJSON(source));
    }
  };

  struct MemberExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct OptionalMemberExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_OPTIONAL_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct ComputedMemberExpression : public BinaryExpression {
    Position getEnd() override { return location.end; }
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_COMPUTED_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct OptionalComputedMemberExpression : public BinaryExpression {

    Position getEnd() override { return location.end; }

    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_OPTIONAL_COMPUTED_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct CallExpression : public BinaryExpression {
    core::Array<core::AutoPtr<Node>> arguments;
    Position getEnd() override { return location.end; }
    std::string toJSON(const std::wstring &source) override {
      std::string sargs;
      size_t index = 0;
      for (auto &arg : arguments) {
        sargs += arg->toJSON(source);
        if (index != arguments.size() - 1) {
          sargs += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"EXPRESSION_CALL","callee":{},"arguments":[{}],"location":{}}})",
          left->toJSON(source), sargs, location.toJSON(source));
    }
  };

  struct OptionalCallExpression : public CallExpression {
    std::string toJSON(const std::wstring &source) override {
      std::string sargs;
      size_t index = 0;
      for (auto &arg : arguments) {
        sargs += arg->toJSON(source);
        if (index != arguments.size() - 1) {
          sargs += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"EXPRESSION_OPTIONAL_CALL","callee":{},"arguments":[{}],"location":{}}})",
          left->toJSON(source), sargs, location.toJSON(source));
    }
  };

  struct Parameter : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"PARMETER","location":{},"identifier":{},"value":{}}})",
          location.toJSON(source), identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source));
    }
  };

  struct ArrowFunctionDeclaration : public Node {
    std::wstring name;

    std::string sourceFile;

    bool async;

    core::Array<core::AutoPtr<Node>> arguments;

    core::AutoPtr<Node> body;

    std::string toJSON(const std::wstring &source) override {
      std::string params;
      size_t index = 0;
      for (auto &param : arguments) {
        params += param->toJSON(source);
        if (index != arguments.size() - 1) {
          params += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"DECLARATION_ARROW_FUNCTION","location":{},"arguments":[{}],"body":{},"async":{}}})",
          location.toJSON(source), params, body->toJSON(source), async);
    };
  };

  struct FunctionDeclaration : public Node {
    core::Array<core::AutoPtr<Node>> arguments;
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> body;
    bool async;
    bool generator;
    std::string toJSON(const std::wstring &source) override {
      std::string params;
      size_t index = 0;
      for (auto &param : arguments) {
        params += param->toJSON(source);
        if (index != arguments.size() - 1) {
          params += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"DECLARATION_FUNCTION","location":{},"identifier":{},"arguments":[{}],"body":{},"async":{},"generator":{}}})",
          location.toJSON(source),
          identifier == nullptr ? "null" : identifier->toJSON(source), params,
          body->toJSON(source), async, generator);
    };
  };

  struct ObjectPatternItem : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> alias;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_OBJECT_ITEM","identifier":{},"alias":{},"value":{},"location":{}}})",
          identifier->toJSON(source),
          alias == nullptr ? "null" : alias->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ObjectPatternRestItem : public Node {
    core::AutoPtr<Node> identifier;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_REST","identifier":{},"location":{}}})",
          identifier->toJSON(source), location.toJSON(source));
    }
  };

  struct ObjectPattern : public Node {
    core::Array<core::AutoPtr<Node>> items;
    std::string toJSON(const std::wstring &source) override {
      std::string sitems;
      size_t index = 0;
      for (auto &item : items) {
        sitems += item->toJSON(source);
        if (index != items.size() - 1) {
          sitems += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"PATTERN_OBJECT","items":[{}],"location":{}}})", sitems,
          location.toJSON(source));
    }
  };

  struct ArrayPatternItem : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_ARRAY_ITEM","identifier":{},"value":{},"location":{}}})",
          identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ArrayPatternRestItem : public Node {
    core::AutoPtr<Node> identifier;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_ARRAY_REST_ITEM","identifier":{},"location":{}}})",
          identifier->toJSON(source), location.toJSON(source));
    }
  };

  struct ArrayPattern : public Node {
    core::Array<core::AutoPtr<Node>> items;
    std::string toJSON(const std::wstring &source) override {
      std::string sitems;
      size_t index = 0;
      for (auto &item : items) {
        sitems += item->toJSON(source);
        if (index != items.size() - 1) {
          sitems += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"PATTERN_ARRAY","items":[{}],"location":{}}})", sitems,
          location.toJSON(source));
    }
  };

  struct ArrayDeclaration : public Node {
    core::Array<core::AutoPtr<Node>> items;
    std::string toJSON(const std::wstring &source) override {
      std::string sitems;
      size_t index = 0;
      for (auto &item : items) {
        sitems += item->toJSON(source);
        if (index != items.size() - 1) {
          sitems += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"DECALRE_ARRAY","items":[{}],"location":{}}})", sitems,
          location.toJSON(source));
    }
  };
  struct VariableDeclarator : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"VARIABLE_DECLARATOR","identifier":{},"value":{}}})",
          identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source));
    }
  };
  struct VariableDeclaration : public Node {
    enum class Kind { VAR, LET, CONST } kind;
    core::Array<core::AutoPtr<Node>> declarations;
    std::string toJSON(const std::wstring &source) override {
      std::string skind;
      switch (kind) {
      case Kind::VAR:
        skind = "var";
        break;
      case Kind::LET:
        skind = "let";
        break;
      case Kind::CONST:
        skind = "const";
        break;
      }
      std::string sdeclarations;
      size_t index = 0;
      for (auto &declaration : declarations) {
        sdeclarations += declaration->toJSON(source);
        if (index != declarations.size() - 1) {
          sdeclarations += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"VARIABLE_DECLARATION","kind":"{}","declarations":[{}],"location":{}}})",
          skind, sdeclarations, location.toJSON(source));
    }
  };

private:
  bool isUnicodeVariableName(wchar_t chr);
  std::string formatException(const std::string &message,
                              const std::string &filename,
                              const std::wstring &source, Position position);

  Location getLocation(const std::wstring &source, Position &start,
                       Position &end);

private:
  bool skipWhiteSpace(const std::string &filename, const std::wstring &source,
                      Position &position);

  bool skipComment(const std::string &filename, const std::wstring &source,
                   Position &position);

  bool skipLineTerminatior(const std::string &filename,
                           const std::wstring &source, Position &position);

  bool skipSemi(const std::string &filename, const std::wstring &source,
                Position &position);

  void skipInvisible(const std::string &filename, const std::wstring &source,
                     Position &position);

  void skipNewLine(const std::string &filename, const std::wstring &source,
                   Position &position);

private:
  core::AutoPtr<Token> readStringToken(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Token> readNumberToken(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Token> readRegexToken(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Token> readCommentToken(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Token> readBooleanToken(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Token> readNullToken(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position);

  core::AutoPtr<Token> readUndefinedToken(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Token> readIdentifierToken(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Token> readSymbolToken(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Token> readTemplateToken(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Token> readTemplateStartToken(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Token> readTemplatePatternToken(const std::string &filename,
                                                const std::wstring &source,
                                                Position &position);

  core::AutoPtr<Token> readTemplateEndToken(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

private:
  bool insertExpressionNode(core::AutoPtr<Node> &current,
                            core::AutoPtr<Node> &child, bool &isComplete);

private:
  core::AutoPtr<Node> readProgram(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position);

  core::AutoPtr<Node> readStatement(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position);

  core::AutoPtr<Node> readExpressionStatement(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readExpression(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position);

  core::AutoPtr<Node> readExpressions(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Node> readInterpreterDirective(const std::string &filename,
                                               const std::wstring &source,
                                               Position &position);

  core::AutoPtr<Node> readDirective(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position);

  core::AutoPtr<Node> readStringLiteral(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readNumberLiteral(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readBooleanLiteral(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readUndefinedLiteral(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readNullLiteral(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Node> readIdentifierLiteral(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readRegexLiteral(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readTemplateLiteral(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readBinaryExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readConditionExpression(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readUpdateExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readUnaryExpression(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readGroupExpression(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readMemberExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readCallExpression(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readParameter(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position);

  core::AutoPtr<Node> readArrowFunctionDeclaration(const std::string &filename,
                                                   const std::wstring &source,
                                                   Position &position);

  core::AutoPtr<Node> readFunctionDeclaration(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readVariableDeclarator(const std::string &filename,
                                             const std::wstring &source,
                                             Position &position);

  core::AutoPtr<Node> readVariableDeclaration(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readObjectPattern(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readObjectPatternItem(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readArrayPattern(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readArrayPatternItem(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readRestPattern(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Node> readArrayDeclaration(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readBlockStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readComment(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position);

public:
  core::AutoPtr<Node> compile(const std::string &filename,
                              const std::string &source);
};
}; // namespace firefly::script