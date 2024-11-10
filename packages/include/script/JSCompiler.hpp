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
    /// \details <private_name>:<invisible>token['#']<identifier>
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

    LITERAL_BIGINT,  // TODO:
    LITERAL_DECIMAL, // TODO:

    PROGRAM,

    STATEMENT_EMPTY,
    /// \details
    /// <statement_block>:<invisible|semi>token['{']<statement>*token['}']
    STATEMENT_BLOCK,
    /// \details <statement_block>:<invisible|semi><identifier['debugger']>
    STATEMENT_DEBUGGER,
    STATEMENT_WITH,        // TODO:
    STATEMENT_RETURN,      // TODO:
    STATEMENT_YIELD,       // TODO:
    STATEMENT_LABEL,       // TODO:
    STATEMENT_BREAK,       // TODO:
    STATEMENT_CONTINUE,    // TODO:
    STATEMENT_IF,          // TODO:
    STATEMENT_SWITCH,      // TODO:
    STATEMENT_SWITCH_CASE, // TODO:
    STATEMENT_THROW,       // TODO:
    STATEMENT_TRY,         // TODO:
    STATEMENT_CACHE,       // TODO:
    STATEMENT_WHILE,
    STATEMENT_DO_WHILE, // TODO:
    STATEMENT_FOR,      // TODO:
    STATEMENT_FOR_IN,   // TODO:
    STATEMENT_FOR_OF,   // TODO:

    VARIABLE_DECLARATION,
    VARIABLE_DECLARATOR,

    DECORATOR, // TODO:

    DIRECTIVE,
    DIRECTIVE_LITERAL,
    INTERPRETER_DIRECTIVE,

    OBJECT_PROPERTY,
    OBJECT_METHOD,
    OBJECT_ACCESSOR,

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
    EXPRESSION_NEW,
    EXPRESSION_DELETE,
    EXPRESSION_AWAIT,
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
    IMPORT_ATTARTUBE, // TODO:
    EXPORT_DECLARATION,
    EXPORT_DEFAULT,
    EXPORT_SPECIFIER,
    EXPORT_ALL,

    DECLARATION_ARROW_FUNCTION,
    DECLARATION_FUNCTION,
    DECLARATION_PARAMETER,
    DECLARATION_REST_PARAMETER,
    DECLARATION_OBJECT,
    DECLARATION_ARRAY,
    DECLARATION_CLASS,
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

  struct NodeArray : public core::Array<core::AutoPtr<Node>> {
    std::string toJSON(const std::wstring &source) {
      size_t index = 0;
      std::string str;
      for (auto &item : *this) {
        str += item->toJSON(source);
        if (index != size() - 1) {
          str += ",";
        }
        index++;
      }
      return fmt::format("[{}]", str);
    }
  };

  struct Program : public Node {
    core::AutoPtr<Node> interpreter;
    NodeArray directives;
    NodeArray body;
    std::string toJSON(const std::wstring &source) override {
      std::stringstream ss;
      ss << "{";
      ss << "\"type\":\"PROGRAM\",";
      if (interpreter != nullptr) {
        ss << "\"interpreter\":" << interpreter->toJSON(source) << ",";
      }
      ss << "\"directives\":[";
      ss << directives.toJSON(source);
      ss << "],";
      ss << "\"body\":[";
      ss << body.toJSON(source);
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
    NodeArray expressions;
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
      return fmt::format(
          R"({{"type":"LITERAL_TEMPLATE","quasis":[{}],"expressions":{},"location":{}}})",
          patterns, expressions.toJSON(source), location.toJSON(source));
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

  struct BlockStatement : public Node {
    NodeArray body;
    NodeArray directives;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"STATEMENT_BLOCK","directives":{},"body":{},"location":{}}})",
          directives.toJSON(source), body.toJSON(source),
          location.toJSON(source));
    }
  };

  struct EmptyStatement : public Node {
    std::string toJSON(const std::wstring &source) {
      return fmt::format(R"({{"type":"STATEMENT_EMPTY","location":{}}})",
                         location.toJSON(source));
    }
  };

  struct DebuggerStatement : public Node {
    std::string toJSON(const std::wstring &source) {
      return fmt::format(R"({{"type":"STATEMENT_DEBUGGER","location":{}}})",
                         location.toJSON(source));
    }
  };

  struct WhileStatement : public Node {
    core::AutoPtr<Node> condition;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_WHILE","condition":{},"body":{}}})",
          condition->toJSON(source), body->toJSON(source));
    }
  };

  struct YieldStatement : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(R"({{"type":"STATEMENT_YIELD","value":{}}})",
                         value != nullptr ? value->toJSON(source) : "null");
    }
  };

  struct ReturnStatement : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(R"({{"type":"STATEMENT_RETURN","value":{}}})",
                         value != nullptr ? value->toJSON(source) : "null");
    }
  };

  struct LabelStatement : public Node {
    core::AutoPtr<Node> label;
    core::AutoPtr<Node> statement;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_LABEL","label":{},"statement":{}}})",
          label->toJSON(source), statement->toJSON(source));
    }
  };

  struct BinaryExpression : public Node {
    core::AutoPtr<Node> left;
    core::AutoPtr<Node> right;
    std::wstring opt;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_BINARY","operator":"{}","left":{},"right":{},"location":{}}})",
          converter.to_bytes(opt),
          left == nullptr ? "null" : left->toJSON(source),
          right == nullptr ? "null" : right->toJSON(source),
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
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_COMPUTED_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct OptionalComputedMemberExpression : public BinaryExpression {

    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_OPTIONAL_COMPUTED_MEMBER","host":{},"field":{},"location":{}}})",
          left->toJSON(source), right->toJSON(source), location.toJSON(source));
    }
  };

  struct CallExpression : public BinaryExpression {
    NodeArray arguments;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPRESSION_CALL","callee":{},"arguments":{},"location":{}}})",
          left->toJSON(source), arguments.toJSON(source),
          location.toJSON(source));
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
          R"({{"type":"EXPRESSION_OPTIONAL_CALL","callee":{},"arguments":{},"location":{}}})",
          left->toJSON(source), sargs, location.toJSON(source));
    }
  };

  struct AwaitExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_AWAIT","right":{},"location":{}}})",
          right->toJSON(source), location.toJSON(source));
    }
  };

  struct TypeofExpression : public BinaryExpression {

    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_TYPEOF","right":{},"location":{}}})",
          right->toJSON(source), location.toJSON(source));
    }
  };

  struct VoidExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_VOID","right":{},"location":{}}})",
          right->toJSON(source), location.toJSON(source));
    }
  };

  struct NewExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_NEW","right":{},"location":{}}})",
          right->toJSON(source), location.toJSON(source));
    }
  };

  struct DeleteExpression : public BinaryExpression {
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      return fmt::format(
          R"({{"type":"EXPRESSION_DELETE","right":{},"location":{}}})",
          right->toJSON(source), location.toJSON(source));
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

    NodeArray arguments;

    core::AutoPtr<Node> body;

    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"DECLARATION_ARROW_FUNCTION","location":{},"arguments":{},"body":{},"async":{}}})",
          location.toJSON(source), arguments.toJSON(source),
          body->toJSON(source), async);
    };
  };

  struct FunctionDeclaration : public Node {
    NodeArray arguments;
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> body;
    bool async;
    bool generator;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"DECLARATION_FUNCTION","location":{},"identifier":{},"arguments":{},"body":{},"async":{},"generator":{}}})",
          location.toJSON(source),
          identifier == nullptr ? "null" : identifier->toJSON(source),
          arguments.toJSON(source), body->toJSON(source), async, generator);
    };
  };

  struct ObjectAccessor : public Node {
    enum class KIND { GET, SET } kind;
    NodeArray arguments;
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) override {
      std::string skind;
      switch (kind) {
      case KIND::GET:
        skind = "get";
        break;
      case KIND::SET:
        skind = "set";
        break;
      }
      return fmt::format(
          R"({{"type":"OBJECT_ACCESSOR","location":{},"identifier":{},"arguments":{},"body":{},"kind":"{}"}})",
          location.toJSON(source), identifier->toJSON(source),
          arguments.toJSON(source), body->toJSON(source), skind);
    };
  };

  struct ObjectMethod : public FunctionDeclaration {
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
          R"({{"type":"OBJECT_METHOD","location":{},"identifier":{},"arguments":{},"body":{},"async":{},"generator":{}}})",
          location.toJSON(source), identifier->toJSON(source), params,
          body->toJSON(source), async, generator);
    };
  };

  struct ObjectProperty : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> implement;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"OBJECT_PROPERTY","identifier":{},"implement":{},"location":{}}})",
          identifier == nullptr ? "null" : identifier->toJSON(source),
          implement == nullptr ? "null" : implement->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ObjectDeclaration : public Node {
    NodeArray properties;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"DECLARATION_OBJECT","properties":{},"location":{}}})",
          properties.toJSON(source), location.toJSON(source));
    }
  };

  struct ClassProperty : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    bool static_;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"CLASS_PROPERTY","identifier":{},"value":{},"static":{},"location":{}}})",
          identifier->toJSON(source), value->toJSON(source), static_,
          location.toJSON(source));
    }
  };

  struct ClassMethod : public FunctionDeclaration {
    bool static_;
    std::string toJSON(const std::wstring &source) {
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
          R"({{"type":"CLASS_METHOD","identifier":{},"arguments":{},"body":{},"async":{},"generator":{},"static":{},"location":{}}})",
          identifier->toJSON(source), params, body->toJSON(source), async,
          generator, static_, location.toJSON(source));
    }
  };

  struct ClassAccessor : public FunctionDeclaration {
    bool static_;
    enum class KIND { GET, SET } kind;
    std::string toJSON(const std::wstring &source) {
      std::string params;
      size_t index = 0;
      for (auto &param : arguments) {
        params += param->toJSON(source);
        if (index != arguments.size() - 1) {
          params += ",";
        }
        index++;
      }
      std::string skind;
      switch (kind) {

      case KIND::GET:
        skind = "get";
        break;
      case KIND::SET:
        skind = "set";
        break;
      }
      return fmt::format(
          R"({{"type":"CLASS_ACCESSOR","identifier":{},"arguments":{},"body":{},"static":{},"kind":"{}","location":{}}})",
          identifier->toJSON(source), params, body->toJSON(source), static_,
          skind, location.toJSON(source));
    }
  };

  struct ClassDeclaration : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> extends;
    NodeArray properties;
    std::string toJSON(const std::wstring &source) {

      return fmt::format(
          R"({{"type":"DECLARATION_CLASS","identifier":{},"extends":{},"properties":{},"location":{}}})",
          identifier == nullptr ? "null" : identifier->toJSON(source),
          extends == nullptr ? "null" : extends->toJSON(source),
          properties.toJSON(source), location.toJSON(source));
    }
  };

  struct RestPatternItem : public Node {
    core::AutoPtr<Node> identifier;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_REST_ITEM","identifier":{},"location":{}}})",
          identifier->toJSON(source), location.toJSON(source));
    }
  };

  struct ObjectPatternItem : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> match;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_OBJECT_ITEM","identifier":{},"match":{},"value":{},"location":{}}})",
          identifier->toJSON(source),
          match == nullptr ? "null" : match->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ObjectPattern : public Node {
    NodeArray items;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_OBJECT","items":{},"location":{}}})",
          items.toJSON(source), location.toJSON(source));
    }
  };

  struct ArrayPatternItem : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_ARRAY_ITEM","identifier":{},"value":{},"location":{}}})",
          identifier == nullptr ? "null" : identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ArrayPattern : public Node {
    NodeArray items;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"PATTERN_ARRAY","items":{},"location":{}}})",
          items.toJSON(source), location.toJSON(source));
    }
  };

  struct ArrayDeclaration : public Node {
    NodeArray items;
    std::string toJSON(const std::wstring &source) override {

      return fmt::format(
          R"({{"type":"DECALRE_ARRAY","items":{},"location":{}}})",
          items.toJSON(source), location.toJSON(source));
    }
  };

  struct VariableDeclarator : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> value;
    enum class Assigment { SET, OF, IN } assigment;
    std::string toJSON(const std::wstring &source) override {
      std::string sassigment;
      switch (assigment) {
      case Assigment::SET:
        sassigment = "=";
        break;
      case Assigment::OF:
        sassigment = "of";
        break;
      case Assigment::IN:
        sassigment = "in";
        break;
      }
      return fmt::format(
          R"({{"type":"VARIABLE_DECLARATOR","assigment":"{}","identifier":{},"value":{},"location":{}}})",
          sassigment, identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct VariableDeclaration : public Node {
    enum class Kind { VAR, LET, CONST } kind;
    NodeArray declarations;
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
      return fmt::format(
          R"({{"type":"VARIABLE_DECLARATION","kind":"{}","declarations":{},"location":{}}})",
          skind, declarations.toJSON(source), location.toJSON(source));
    }
  };

  struct ImportSpecifier : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> alias;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"IMPORT_SPECIFIER","identifier":{},"alias":{},"location":{}}})",
          identifier->toJSON(source),
          alias == nullptr ? "null" : alias->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ImportDefaultSpecifier : public Node {
    core::AutoPtr<Node> identifier;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"IMPORT_DEFAULT_SPECIFIER","identifier":{},"location":{}}})",
          identifier->toJSON(source), location.toJSON(source));
    }
  };

  struct ImportNamespaceSpecifier : public Node {
    core::AutoPtr<Node> alias;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"IMPORT_NAMESPACE_SPECIFIER","alias":{},"location":{}}})",
          alias->toJSON(source), location.toJSON(source));
    }
  };

  struct ImportDeclaration : public Node {
    core::AutoPtr<Node> source;
    NodeArray items;
    std::string toJSON(const std::wstring &source) {

      return fmt::format(
          R"({{"type":"IMPORT_DECLARATION","source":{},"items":{},"location":{}}})",
          this->source->toJSON(source), items.toJSON(source),
          location.toJSON(source));
    }
  };

  struct ExportDefaultSpecifier : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPORT_DEFAULT_SPECIFIER","value":{},"location":{}}})",
          value->toJSON(source), location.toJSON(source));
    }
  };

  struct ExportSpecifier : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> alias;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPORT_SPECIFIER","identifier":{},"alias":{},"location":{}}})",
          identifier->toJSON(source), alias->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ExportAllSpecifier : public Node {
    core::AutoPtr<Node> source;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPORT_ALL_SPECIFIER","source":{},"location":{}}})",
          this->source->toJSON(source), location.toJSON(source));
    }
  };

  struct ExportDeclaration : public Node {
    NodeArray items;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"EXPORT_DECLARATION","items":{},"location":{}}})",
          items.toJSON(source), location.toJSON(source));
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
                     Position &position, bool *isNewline = nullptr);

  void skipNewLine(const std::string &filename, const std::wstring &source,
                   Position &position, bool *isNewline = nullptr);

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
  core::AutoPtr<Node> readProgram(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position);

  core::AutoPtr<Node> readStatement(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position);

  core::AutoPtr<Node> readEmptyStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readYieldStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readReturnStatement(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readLabelStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readExpressionStatement(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readValue(const std::string &filename,
                                const std::wstring &source, Position &position);

  core::AutoPtr<Node> readRValue(const std::string &filename,
                                 const std::wstring &source, Position &position,
                                 int level);

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

  core::AutoPtr<Node> readAssigmentExpression(const std::string &filename,
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

  core::AutoPtr<Node> readRestExpression(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readAwaitExpression(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readTypeofExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readVoidExpression(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readDeleteExpression(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readNewExpression(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readInExpression(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readInstanceOfExpression(const std::string &filename,
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

  core::AutoPtr<Node> readArrayDeclaration(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readObjectDeclaration(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readObjectProperty(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readObjectMethod(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readObjectAccessor(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readClassDeclaration(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readClassProperty(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readClassMethod(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Node> readClassAccessor(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position);

  core::AutoPtr<Node> readStaticBlock(const std::string &filename,
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

  core::AutoPtr<Node> readBlockStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readComment(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position);

  core::AutoPtr<Node> readImportSpecifier(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readImportDefaultSpecifier(const std::string &filename,
                                                 const std::wstring &source,
                                                 Position &position);

  core::AutoPtr<Node> readImportNamespaceSpecifier(const std::string &filename,
                                                   const std::wstring &source,
                                                   Position &position);

  core::AutoPtr<Node> readImportDeclaration(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readExportSpecifier(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readExportDefaultSpecifier(const std::string &filename,
                                                 const std::wstring &source,
                                                 Position &position);

  core::AutoPtr<Node> readExportAllSpecifier(const std::string &filename,
                                             const std::wstring &source,
                                             Position &position);

  core::AutoPtr<Node> readExportDeclaration(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readDebuggerStatement(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readWhileStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

public:
  core::AutoPtr<Node> compile(const std::string &filename,
                              const std::string &source);
};
}; // namespace firefly::script