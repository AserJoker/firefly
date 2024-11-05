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
    PRIVATE_NAME,
    LITERAL_REGEX,
    LITERAL_NULL,
    LITERAL_STRING,
    LITERAL_BOOLEAN,
    LITERAL_NUMBER,
    LITERAL_COMMENT,
    LITERAL_UNDEFINED,
    LITERAL_IDENTITY,
    LITERAL_TEMPLATE,

    LITERAL_BIGINT,
    LITERAL_DECIMAL,
    LITERAL_MULTILINE_COMMENT,

    PROGRAM,
    FUNCTION_DEFINITION,
    STATEMENT_BLOCK,
    STATEMENT_EMPTY,
    STATEMENT_DEBUGGER,
    STATEMENT_WITH,
    STATEMENT_RETURN,
    STATEMENT_LABEL,
    STATEMENT_BREAK,
    STATEMENT_CONTINUE,
    STATEMENT_IF,
    STATEMENT_SWITCH,
    STATEMENT_SWITCH_CASE,
    STATEMENT_THROW,
    STATEMENT_TRY,
    STATEMENT_CACHE,
    STATEMENT_WHILE,
    STATEMENT_DO_WHILE,
    STATEMENT_FOR,
    STATEMENT_FOR_IN,
    STATEMENT_FOR_OF,
    FUNCTION_DECLARATION,
    VARIABLE_DECLARATION,
    VARIABLE_DECORATOR,

    DECORATOR,
    DIRECTIVE,
    DIRECTIVE_LITERAL,
    INTERPRETER_DIRECTIVE,

    EXPRESSION_SUPER,
    EXPRESSION_IMPORT,
    EXPRESSION_THIS,
    EXPRESSION_ARRAY_FUNCTION,
    EXPRESSION_YIELD,
    EXPRESSION_AWAIT,
    EXPRESSION_ARRAY,
    EXPRESSION_OBJECT,

    OBJECT_MEMBER,
    OBJECT_PROPERTY,
    OBJECT_METHOD,

    EXPRESSION_RECORD,
    EXPRESSION_TUPLE,
    EXPRESSION_FUNCTION,
    EXPRESSION_UNARY,
    EXPRESSION_UPDATE,
    EXPRESSION_BINARY,
    EXPRESSION_ASSIGMENT,
    EXPRESSION_LOGICAL,
    EXPRESSION_SPREAD,
    EXPRESSION_ARGUMENT,
    EXPRESSION_MEMBER,
    EXPRESSION_OPTIONAL_MEMBER,
    EXPRESSION_BIND,
    EXPRESSION_CONDITION,
    EXPRESSION_CALL,
    EXPRESSION_OPTIONAL_CALL,
    EXPRESSION_NEW,
    EXPRESSION_SEQUENCE,
    EXPRESSION_PARENTHESIZED,
    EXPRESSION_DO,
    EXPRESSION_MODULE,
    EXPRESSION_TOPIC,
    EXPRESSION_GROUP,

    PATTERN_OBJECT,
    PATTERN_ARRAY,
    PATTERN_REST,
    PATTERN_ASSIGMENT,
    CLASS,
    CLASS_BODY,
    CLASS_METHOD,
    CLASS_PRIVATE_METHOD,
    CLASS_PROPERTY,
    CLASS_PRIVATE_PROPERTY,
    CLASS_ACCESSOR_PROPERTY,
    STATIC_BLOCK,
    CLASS_DECLARATION,
    IMPORT_DECLARATION,
    IMPORT_SPECIFIER,
    IMPORT_DEFAULT,
    IMPORT_NAMESPACE,
    IMPORT_ATTARTUBE,
    EXPORT_DECLARATION,
    EXPORT_NAMED,
    EXPORT_DEFAULT,
    EXPORT_NAMESPACE,
    EXPORT_ALL,

    DEFINITION_ARROW_FUNCTION,
    DEFINITION_PARAMETER,
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
    Position getStart() {
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
    Position getEnd() {
      if (right != nullptr) {
        auto r = right.cast<BinaryExpression>();
        if (r != nullptr) {
          return r->getStart();
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

  struct Parameter : public Node {
    std::wstring identifier;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"PARMETER","location":{},"identifier":"{}","value":{}}})",
          location.toJSON(source), converter.to_bytes(identifier),
          value == nullptr ? "null" : value->toJSON(source));
    }
  };

  struct ArrowFunctionDefinition : public Node {
    std::wstring name;

    std::string sourceFile;

    bool async;

    core::Array<core::AutoPtr<Node>> parameters;

    core::AutoPtr<Node> restParameter;

    core::AutoPtr<Node> body;

    std::string toJSON(const std::wstring &source) override {
      std::string params;
      size_t index = 0;
      for (auto &param : parameters) {
        params += param->toJSON(source);
        if (index != parameters.size() - 1) {
          params += ",";
        }
        index++;
      }
      return fmt::format(
          R"({{"type":"DEFINITION_ARROW_FUNCTION","location":{},"restParameter":{},"parameters":[{}],"body":{},"async":{}}})",
          location.toJSON(source),
          restParameter == nullptr ? "null" : restParameter->toJSON(source),
          params, body->toJSON(source), async);
    };
  };

private:
  bool isUnicodeVariableName(wchar_t chr);
  std::string formatException(const std::string &message,
                              const std::string &filename,
                              const std::wstring &source, Position position);

  Location getLocation(const std::wstring &source, Position start,
                       Position end);

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

  core::AutoPtr<Node> readUpdateExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readUnaryExpression(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readGroupExpression(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readConditionExpression(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readParameter(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position);

  core::AutoPtr<Node> readRestParameter(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readArrowFunctionDefinition(const std::string &filename,
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