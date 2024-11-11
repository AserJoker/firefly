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
    LITERAL_MULTILINE_COMMENT,
    LITERAL_UNDEFINED,
    LITERAL_IDENTITY,
    LITERAL_TEMPLATE,

    LITERAL_BIGINT,
    // LITERAL_DECIMAL,

    PROGRAM,

    STATEMENT_EMPTY,
    STATEMENT_BLOCK,
    STATEMENT_DEBUGGER,
    STATEMENT_RETURN,
    STATEMENT_YIELD,
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

    VARIABLE_DECLARATION,
    VARIABLE_DECLARATOR,

    DECORATOR,

    DIRECTIVE,
    DIRECTIVE_LITERAL,
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
    DECLARATION_PARAMETER,
    DECLARATION_REST_PARAMETER,
    DECLARATION_OBJECT,
    DECLARATION_ARRAY,
    DECLARATION_CLASS,
  };

  enum class AccessorKind { GET, SET };

  enum class DeclarationKind { CONST, LET, VAR, UNKNOWN };

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

  struct BigIntLiteral : public Node {
    std::wstring value;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::string val = converter.to_bytes(value);
      return fmt::format(
          R"({{"type":"LITERAL_BIGINT","location":{},"value":"{}"}})",
          location.toJSON(source), val);
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
      return fmt::format(
          R"({{"type":"LITERAL_IDENTITY","value":"{}","location":{}}})", val,
          location.toJSON(source));
    }
  };

  struct PrivateName : public Node {
    std::wstring value;
    std::string toJSON(const std::wstring &source) override {
      static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
      std::string val = converter.to_bytes(value);
      return fmt::format(
          R"({{"type":"PRIVATE_NAME","value":"{}","location":{}}})", val,
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

  struct ForStatement : public Node {
    core::AutoPtr<Node> init;
    core::AutoPtr<Node> condition;
    core::AutoPtr<Node> update;
    core::AutoPtr<Node> body;

    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_FOR","init":{},"condition":{},"update":{},"body":{},"location":{}}})",
          init == nullptr ? "null" : init->toJSON(source),
          condition == nullptr ? "null" : condition->toJSON(source),
          update == nullptr ? "null" : update->toJSON(source),
          body->toJSON(source), location.toJSON(source));
    }
  };

  struct ForInStatement : public Node {
    DeclarationKind kind;
    core::AutoPtr<Node> declaration;
    core::AutoPtr<Node> expression;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) {
      std::string skind;
      switch (kind) {
      case DeclarationKind::CONST:
        skind = "const";
        break;
      case DeclarationKind::LET:
        skind = "let";
        break;
      case DeclarationKind::VAR:
        skind = "var";
        break;
      case DeclarationKind::UNKNOWN:
        skind = "unknown";
        break;
      }
      return fmt::format(
          R"({{"type":"STATEMENT_FOR_IN","kind":"{}","declaration":{},"expression":{},"body":{},"location":{}}})",
          skind, declaration->toJSON(source), expression->toJSON(source),
          body->toJSON(source), location.toJSON(source));
    }
  };

  struct ForOfStatement : public Node {
    DeclarationKind kind;
    core::AutoPtr<Node> declaration;
    core::AutoPtr<Node> expression;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) {
      std::string skind;
      switch (kind) {
      case DeclarationKind::CONST:
        skind = "const";
        break;
      case DeclarationKind::LET:
        skind = "let";
        break;
      case DeclarationKind::VAR:
        skind = "var";
      case DeclarationKind::UNKNOWN:
        skind = "unknown";
        break;
      }
      return fmt::format(
          R"({{"type":"STATEMENT_FOR_OF","kind":"{}","declaration":{},"expression":{},"body":{},"location":{}}})",
          skind, declaration->toJSON(source), expression->toJSON(source),
          body->toJSON(source), location.toJSON(source));
    }
  };

  struct WhileStatement : public Node {
    core::AutoPtr<Node> condition;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_WHILE","condition":{},"body":{},"location":{}}})",
          condition->toJSON(source), body->toJSON(source),
          location.toJSON(source));
    }
  };

  struct DoWhileStatement : public WhileStatement {
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_DO_WHILE","condition":{},"body":{},"location":{}}})",
          condition->toJSON(source), body->toJSON(source),
          location.toJSON(source));
    }
  };

  struct YieldStatement : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_YIELD","value":{},"location":{}}})",
          value != nullptr ? value->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct ReturnStatement : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_RETURN","value":{},"location":{}}})",
          value != nullptr ? value->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct ThrowStatement : public Node {
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_THROW","value":{},"location":{}}})",
          value != nullptr ? value->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct TryCatchStatement : public Node {
    core::AutoPtr<Node> binding;
    core::AutoPtr<Node> statement;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_TRY_CATCH","binding":{},"statement":{},"location":{}}})",
          binding != nullptr ? binding->toJSON(source) : "null",
          statement->toJSON(source), location.toJSON(source));
    }
  };

  struct TryStatement : public Node {
    core::AutoPtr<Node> try_;
    core::AutoPtr<Node> catch_;
    core::AutoPtr<Node> finally;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_TRY","try":{},"catch":{},"finally":{},"location":{}}})",
          try_->toJSON(source),
          catch_ != nullptr ? catch_->toJSON(source) : "null",
          finally != nullptr ? finally->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct LabelStatement : public Node {
    core::AutoPtr<Node> label;
    core::AutoPtr<Node> statement;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_LABEL","label":{},"statement":{},"location":{}}})",
          label->toJSON(source), statement->toJSON(source),
          location.toJSON(source));
    }
  };

  struct BreakStatement : public Node {
    core::AutoPtr<Node> label;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_BREAK","label":{},"location":{}}})",
          label == nullptr ? "null" : label->toJSON(source),
          location.toJSON(source));
    }
  };

  struct ContinueStatement : public Node {
    core::AutoPtr<Node> label;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"STATEMENT_CONTINUE","label":{},"location":{}}})",
          label == nullptr ? "null" : label->toJSON(source),
          location.toJSON(source));
    }
  };

  struct IfStatement : public Node {
    core::AutoPtr<Node> condition;
    core::AutoPtr<Node> alternate;
    core::AutoPtr<Node> consequent;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"STATEMENT_IF","condition":{},"alternate":{},"consequent":{},"location":{}}})",
          condition->toJSON(source), alternate->toJSON(source),
          consequent != nullptr ? consequent->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct SwitchCaseStatement : public Node {
    core::AutoPtr<Node> match;
    NodeArray statements;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"STATEMENT_SWITCH_CASE","match":{},"statements":{},"location":{}}})",
          match == nullptr ? "null" : match->toJSON(source),
          statements.toJSON(source), location.toJSON(source));
    }
  };

  struct SwitchStatement : public Node {
    core::AutoPtr<Node> expression;
    NodeArray cases;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"STATEMENT_SWITCH","expression":{},"cases":{},"location":{}}})",
          expression->toJSON(source), cases.toJSON(source),
          location.toJSON(source));
    }
  };

  struct Decorator : public Node {
    core::AutoPtr<Node> expression;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"DECORATOR","expression":{},"location":{}}})",
          expression->toJSON(source), location.toJSON(source));
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
    AccessorKind kind;
    NodeArray arguments;
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> body;
    std::string toJSON(const std::wstring &source) override {
      std::string skind;
      switch (kind) {
      case AccessorKind::GET:
        skind = "get";
        break;
      case AccessorKind::SET:
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
    NodeArray decorators;
    bool static_;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"CLASS_PROPERTY","decorators":{},"identifier":{},"value":{},"static":{},"location":{}}})",
          decorators.toJSON(source), identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source), static_,
          location.toJSON(source));
    }
  };

  struct ClassMethod : public FunctionDeclaration {
    bool static_;
    NodeArray decorators;
    std::string toJSON(const std::wstring &source) {

      return fmt::format(
          R"({{"type":"CLASS_METHOD","decorators":{},"identifier":{},"arguments":{},"body":{},"async":{},"generator":{},"static":{},"location":{}}})",
          decorators.toJSON(source), identifier->toJSON(source),
          arguments.toJSON(source), body->toJSON(source), async, generator,
          static_, location.toJSON(source));
    }
  };

  struct ClassAccessor : public FunctionDeclaration {
    bool static_;
    AccessorKind kind;
    NodeArray decorators;
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

      case AccessorKind::GET:
        skind = "get";
        break;
      case AccessorKind::SET:
        skind = "set";
        break;
      }
      return fmt::format(
          R"({{"type":"CLASS_ACCESSOR","decorators":{},"identifier":{},"arguments":{},"body":{},"static":{},"kind":"{}","location":{}}})",
          decorators.toJSON(source), identifier->toJSON(source), params,
          body->toJSON(source), static_, skind, location.toJSON(source));
    }
  };

  struct ClassDeclaration : public Node {
    core::AutoPtr<Node> identifier;
    core::AutoPtr<Node> extends;
    NodeArray decorators;
    NodeArray properties;
    std::string toJSON(const std::wstring &source) {

      return fmt::format(
          R"({{"type":"DECLARATION_CLASS","decorators":{},"identifier":{},"extends":{},"properties":{},"location":{}}})",
          decorators.toJSON(source),
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
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"VARIABLE_DECLARATOR","identifier":{},"value":{},"location":{}}})",
          identifier->toJSON(source),
          value == nullptr ? "null" : value->toJSON(source),
          location.toJSON(source));
    }
  };

  struct VariableDeclaration : public Node {
    DeclarationKind kind;
    NodeArray declarations;
    std::string toJSON(const std::wstring &source) override {
      std::string skind;
      switch (kind) {
      case DeclarationKind::VAR:
        skind = "var";
        break;
      case DeclarationKind::LET:
        skind = "let";
        break;
      case DeclarationKind::CONST:
        skind = "const";
        break;
      case DeclarationKind::UNKNOWN:
        skind = "unknown";
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
  struct ImportAttribute : public Node {
    core::AutoPtr<Node> key;
    core::AutoPtr<Node> value;
    std::string toJSON(const std::wstring &source) {
      return fmt::format(
          R"({{"type":"IMPORT_ATTRIBUTE","key":{},"value":{},"location":{}}})",
          key->toJSON(source), value->toJSON(source), location.toJSON(source));
    }
  };

  struct ImportDeclaration : public Node {
    core::AutoPtr<Node> source;
    NodeArray items;
    NodeArray attributes;
    std::string toJSON(const std::wstring &source) {

      return fmt::format(
          R"({{"type":"IMPORT_DECLARATION","attributes":{},"source":{},"items":{},"location":{}}})",
          attributes.toJSON(source), this->source->toJSON(source),
          items.toJSON(source), location.toJSON(source));
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
          identifier->toJSON(source),
          alias != nullptr ? alias->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct ExportAllSpecifier : public Node {
    core::AutoPtr<Node> alias;
    std::string toJSON(const std::wstring &source) override {
      return fmt::format(
          R"({{"type":"EXPORT_ALL_SPECIFIER","alias":{},"location":{}}})",
          alias != nullptr ? alias->toJSON(source) : "null",
          location.toJSON(source));
    }
  };

  struct ExportDeclaration : public Node {
    NodeArray items;
    core::AutoPtr<Node> source;
    NodeArray attributes;
    std::string toJSON(const std::wstring &source) {
      auto src = this->source;
      return fmt::format(
          R"({{"type":"EXPORT_DECLARATION","attributes":{},"items":{},"source":{},"location":{}}})",
          attributes.toJSON(source), items.toJSON(source),
          src != nullptr ? src->toJSON(source) : "null",
          location.toJSON(source));
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

  core::AutoPtr<Token> readBigIntToken(const std::string &filename,
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

  NodeArray readStatements(const std::string &filename,
                           const std::wstring &source, Position &position);

  core::AutoPtr<Node> readDebuggerStatement(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readWhileStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readDoWhileStatement(const std::string &filename,
                                           const std::wstring &source,
                                           Position &position);

  core::AutoPtr<Node> readForStatement(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readForInStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readForOfStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readBlockStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readYieldStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readReturnStatement(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readThrowStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readBreakStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readContinueStatement(const std::string &filename,
                                            const std::wstring &source,
                                            Position &position);

  core::AutoPtr<Node> readLabelStatement(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position);

  core::AutoPtr<Node> readIfStatement(const std::string &filename,
                                      const std::wstring &source,
                                      Position &position);

  core::AutoPtr<Node> readSwitchStatement(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readSwitchCaseStatement(const std::string &filename,
                                              const std::wstring &source,
                                              Position &position);

  core::AutoPtr<Node> readTryStatement(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position);

  core::AutoPtr<Node> readTryCatchStatement(const std::string &filename,
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

  core::AutoPtr<Node> readDecorator(const std::string &filename,
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

  core::AutoPtr<Node> readBigIntLiteral(const std::string &filename,
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

  core::AutoPtr<Node> readPrivateName(const std::string &filename,
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

  core::AutoPtr<Node> readComment(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position);

  core::AutoPtr<Node> readImportSpecifier(const std::string &filename,
                                          const std::wstring &source,
                                          Position &position);

  core::AutoPtr<Node> readImportAttriabue(const std::string &filename,
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

public:
  core::AutoPtr<Node> compile(const std::string &filename,
                              const std::string &source);
};
}; // namespace firefly::script