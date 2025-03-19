#pragma once
#include "JSOperator.hpp"
#include "JSParser.hpp"
#include "JSProgram.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class JSCodeGenerator {
private:
  struct LabelFrame {
    std::wstring label;
    size_t address;
  };

private:
  std::vector<LabelFrame> _breaks;
  std::vector<LabelFrame> _continues;
  std::wstring _label;
  size_t _scope{};
  JSNode *_lexContext{};
  JSAllocator *_allocator;

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
    auto err = _allocator->create<JSErrorNode>();
    err->message = message;
    err->location = loc;
    return err;
  }

  JSNode *resolveStore(const std::wstring &source, JSNode *node,
                       JSProgram &program);

  void resolveExport(const std::wstring &source, JSNode *node,
                     JSProgram &program);

  JSNode *resolveMemberChain(const std::wstring &source, JSNode *node,
                             JSProgram &program,
                             std::vector<size_t> &addresses);

  std::wstring pushBreakFrame(JSProgram &program) {
    pushOperator(program, JS_OPERATOR::BREAK_LABEL_BEGIN);
    pushString(program, _label);
    pushOperator(program, JS_OPERATOR::SET_LABELE_ADDRESS);
    _breaks.push_back({
        .label = _label,
        .address = program.codes.size(),
    });
    pushAddress(program, 0);
    auto label = _label;
    _label = L"";
    return label;
  }
  void pushContinueFrame(JSProgram &program, const std::wstring &label) {
    pushOperator(program, JS_OPERATOR::CONTINUE_LABEL_BEGIN);
    pushString(program, label);
    pushOperator(program, JS_OPERATOR::SET_LABELE_ADDRESS);
    _breaks.push_back({
        .label = label,
        .address = program.codes.size(),
    });
    pushAddress(program, 0);
  }

  void popLabelFrame(JSProgram &program, size_t addr,
                     const std::wstring &label = L"") {
    *(size_t *)(program.codes.data() + _breaks.rbegin()->address) = addr;
    _breaks.pop_back();
    _label = label;
    pushOperator(program, JS_OPERATOR::LABEL_END);
  }

public:
  JSCodeGenerator(JSAllocator *allocator) : _allocator(allocator) {}

  virtual ~JSCodeGenerator() {}

  JSNode *resolveNode(const std::wstring &source, JSNode *node,
                      JSProgram &program);

  std::unordered_map<JSNode *, size_t>
  beginScope(const std::wstring &source, JSNode *node, JSProgram &program);

  JSNode *endScope(const std::wstring &source, JSNode *node, JSProgram &program,
                   std::unordered_map<JSNode *, size_t> &ctx);

  JSNode *resolveFunctionDeclaration(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveFunctionBodyDeclaration(const std::wstring &source,
                                         JSNode *node, JSProgram &program);

  JSNode *resolveImportDeclaration(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveExportDeclaration(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveArrowFunctionDeclaration(const std::wstring &source,
                                          JSNode *node, JSProgram &program);

  JSNode *resolveFunctionDeclarator(const std::wstring &source, JSNode *node,
                                    JSProgram &program);

  JSNode *resolve(const std::wstring &source, JSNode *node, JSProgram &program);

  JSNode *resolveRegexLiteral(const std::wstring &source, JSNode *node,
                              JSProgram &program);

  JSNode *resolveNullLiteral(const std::wstring &source, JSNode *node,
                             JSProgram &program);

  JSNode *resolveUndefinedLiteral(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveIdentityLiteral(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveBigintLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program);

  JSNode *resolveTemplateLiteral(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveStringLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program);

  JSNode *resolveBooleanLiteral(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveNumberLiteral(const std::wstring &source, JSNode *node,
                               JSProgram &program);

  JSNode *resolveExpressionStatement(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveWhileStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveDoWhileStatement(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveForStatement(const std::wstring &source, JSNode *node,
                              JSProgram &program);

  JSNode *resolveForOfStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveForInStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveForAwaitOfStatement(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveObjectDeclaration(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveArrayDeclaration(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveClassDeclaration(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveVariableDeclaration(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveBlockStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveDebuggerStatement(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveReturnStatement(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveLabelStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveBreakStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveContinueStatement(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveThrowStatement(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveTryCatchStatement(const std::wstring &source, JSNode *node,
                                   JSProgram &program);

  JSNode *resolveTryStatement(const std::wstring &source, JSNode *node,
                              JSProgram &program);

  JSNode *resolveIfStatement(const std::wstring &source, JSNode *node,
                             JSProgram &program);

  JSNode *resolveSwitchStatement(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveBinaryExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveMemberExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveOptionalMemberExpression(const std::wstring &source,
                                          JSNode *node, JSProgram &program);

  JSNode *resolveComputedMemberExpression(const std::wstring &source,
                                          JSNode *node, JSProgram &program);

  JSNode *resolveOptionalComputedMemberExpression(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program);

  JSNode *resolveConditionExpression(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveCallExpression(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveOptionalCallExpression(const std::wstring &source,
                                        JSNode *node, JSProgram &program);

  JSNode *resolveNewExpression(const std::wstring &source, JSNode *node,
                               JSProgram &program);

  JSNode *resolveDeleteExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveVoidExpression(const std::wstring &source, JSNode *node,
                                JSProgram &program);

  JSNode *resolveTypeofExpression(const std::wstring &source, JSNode *node,
                                  JSProgram &program);

  JSNode *resolveYieldExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveYieldDelegateExpression(const std::wstring &source,
                                         JSNode *node, JSProgram &program);

  JSNode *resolveAwaitExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveGroupExpression(const std::wstring &source, JSNode *node,
                                 JSProgram &program);

  JSNode *resolveAssigmentExpression(const std::wstring &source, JSNode *node,
                                     JSProgram &program);

  JSNode *resolveThis(const std::wstring &source, JSNode *node,
                      JSProgram &program);

  JSNode *resolveProgram(const std::wstring &source, JSNode *node,
                         JSProgram &program);

  void compile(JSProgram &program, const std::wstring &source, JSNode *node) {
    auto err = resolve(source, node, program);
    if (err) {
      program.error = err->cast<JSErrorNode>();
    }
  }
};