#include "script/compiler/JSCodeGenerator.hpp"
#include "script/compiler/JSOperator.hpp"
#include "script/compiler/JSParser.hpp"
JSNode *JSCodeGenerator::resolveStore(const std::wstring &source, JSNode *node,
                                      JSProgram &program) {
  if (is(node, JS_NODE_TYPE::LITERAL_IDENTITY)) {
    pushOperator(program, JS_OPERATOR::STORE);
    pushString(program, node->location.get(source));
  } else if (is(node, JS_NODE_TYPE::EXPRESSION_MEMBER)) {
    auto expression = unwrap(node)->cast<JSMemberExpressionNode>();
    pushOperator(program, JS_OPERATOR::PUSH_VALUE);
    pushUint32(program, 0);
    pushOperator(program, JS_OPERATOR::STR);
    pushString(program, expression->field->location.get(source));
    if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
      auto err = resolve(source, expression->host, program);
      if (err) {
        return err;
      }
    }
    if (!is(expression->host, JS_NODE_TYPE::LITERAL_SUPER)) {
      if (expression->field->type == JS_NODE_TYPE::PRIVATE_NAME) {
        pushOperator(program, JS_OPERATOR::SET_PRIVATE_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::SET_FIELD);
      }
    } else {
      pushOperator(program, JS_OPERATOR::SET_SUPER_FIELD);
    }
  } else if (is(node, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER)) {
    auto expression = unwrap(node)->cast<JSComputedMemberExpressionNode>();
    pushOperator(program, JS_OPERATOR::PUSH_VALUE);
    pushUint32(program, 0);
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
    pushOperator(program, JS_OPERATOR::PUSH_VALUE);
    pushUint32(program, 0);
    auto expression = node->cast<JSObjectPatternNode>();
    bool spread = false;
    if (!expression->items.empty()) {
      auto last = expression->items[expression->items.size() - 1];
      if (last->cast<JSObjectPatternItemNode>()->key->type ==
          JS_NODE_TYPE::PATTERN_SPREAD_ITEM) {
        spread = true;
      }
    }
    for (size_t index = 0; index < expression->items.size(); index++) {
      auto item = expression->items[index];
      auto field = item->cast<JSObjectPatternItemNode>();
      if (is(field->key, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
        if (index != expression->items.size() - 1) {
          return createError(L"Rest element must be last element",
                             item->location);
        }
        auto spread = field->key->cast<JSSpreadPatternItemNode>();
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, index);
        pushOperator(program, JS_OPERATOR::OBJECT_SPREAD);
        pushUint32(program, index);
        auto err = resolveStore(source, spread->value, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::POP);
        break;
      }
      if (field->computed) {
        auto err = resolve(source, field->key, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, field->key->location.get(source));
      }
      if (spread) {
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 0);
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, index + 2);
      } else {
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, index + 1);
      }
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
    }
    pushOperator(program, JS_OPERATOR::POP);
  } else if (is(node, JS_NODE_TYPE::PATTERN_ARRAY)) {
    auto expression = node->cast<JSArrayPatternNode>();
    pushOperator(program, JS_OPERATOR::PUSH_VALUE);
    pushUint32(program, 0);
    pushOperator(program, JS_OPERATOR::ITERATOR);
    for (size_t index = 0; index < expression->items.size(); index++) {
      auto item = expression->items[index];
      if (item) {
        auto field = item->cast<JSArrayPatternItemNode>();

        if (field->alias->type == JS_NODE_TYPE::PATTERN_SPREAD_ITEM) {
          if (index != expression->items.size() - 1) {
            return createError(L"Rest element must be last element",
                               item->location);
          }
          auto spread = field->alias->cast<JSSpreadPatternItemNode>();
          pushOperator(program, JS_OPERATOR::PUSH_VALUE);
          pushUint32(program, 0);
          pushOperator(program, JS_OPERATOR::ARRAY_SPREAD);
          auto err = resolveStore(source, spread->value, program);
          if (err) {
            return err;
          }
          pushOperator(program, JS_OPERATOR::POP);
          break;
        } else {
          pushOperator(program, JS_OPERATOR::PUSH_VALUE);
          pushUint32(program, 0);
          pushOperator(program, JS_OPERATOR::NEXT);
          pushOperator(program, JS_OPERATOR::POP);
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
          auto err = resolveStore(source, field->alias, program);
          if (err) {
            return err;
          }
        }
      }
      pushOperator(program, JS_OPERATOR::POP);
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

void JSCodeGenerator::resolveExport(const std::wstring &source, JSNode *node,
                                    JSProgram &program) {
  if (is(node, JS_NODE_TYPE::LITERAL_IDENTITY)) {
    pushOperator(program, JS_OPERATOR::LOAD);
    pushString(program, node->location.get(source));
    pushOperator(program, JS_OPERATOR::EXPORT);
    pushString(program, node->location.get(source));
    pushOperator(program, JS_OPERATOR::POP);
  } else if (is(node, JS_NODE_TYPE::PATTERN_OBJECT)) {
    auto expression = node->cast<JSObjectPatternNode>();
    for (auto item : expression->items) {
      auto field = item->cast<JSObjectPatternItemNode>();
      if (is(field->key, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
        auto spread = field->key->cast<JSSpreadPatternItemNode>();
        resolveExport(source, spread->value, program);

      } else {
        if (field->alias) {
          resolveExport(source, field->alias, program);
        } else {
          resolveExport(source, field->key, program);
        }
      }
    }
  } else if (is(node, JS_NODE_TYPE::PATTERN_ARRAY)) {
    auto expression = node->cast<JSArrayPatternNode>();
    for (auto &item : expression->items) {
      if (item && is(item, JS_NODE_TYPE::PATTERN_SPREAD_ITEM)) {
        auto field = item->cast<JSSpreadPatternItemNode>();
        resolveExport(source, field->value, program);

      } else {
        if (item) {
          auto field = item->cast<JSArrayPatternItemNode>();
          resolveExport(source, field->alias, program);
        }
      }
    }
  }
}

JSNode *JSCodeGenerator::resolveMemberChain(const std::wstring &source,
                                            JSNode *node, JSProgram &program,
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
      if (expression->field->type == JS_NODE_TYPE::PRIVATE_NAME) {
        pushOperator(program, JS_OPERATOR::GET_PRIVATE_FIELD);
      } else {
        pushOperator(program, JS_OPERATOR::GET_FIELD);
      }
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
    bool isPrivate = false;
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
      if (expression->field->type == JS_NODE_TYPE::PRIVATE_NAME) {
        isPrivate = true;
      }
    } else if (is(callee, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) ||
               is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
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
    size_t index = 0;
    bool spread = false;
    for (auto it = expression->arguments.begin();
         it != expression->arguments.end(); it++) {
      auto arg = *it;
      if (is(arg, JS_NODE_TYPE::EXPRESSION_SPREAD)) {
        auto expr = arg->cast<JSSpreadExpressionNode>();
        auto err = resolve(source, expr->value, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::SPREAD);
        pushUint32(program, index);
        spread = true;
      } else {
        auto err = resolve(source, arg, program);
        if (err) {
          return err;
        }
        index++;
      }
    }
    if (!spread) {
      pushOperator(program, JS_OPERATOR::PUSH);
      pushNumber(program, index);
    }
    if (is(callee, JS_NODE_TYPE::EXPRESSION_MEMBER) ||
        is(callee, JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) ||
        is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_MEMBER) ||
        is(callee, JS_NODE_TYPE::EXPRESSION_OPTIONAL_COMPUTED_MEMBER)) {
      if (isPrivate) {
        pushOperator(program, JS_OPERATOR::PRIVATE_MEMBER_CALL);
      } else {
        pushOperator(program, JS_OPERATOR::MEMBER_CALL);
      }
    } else if (is(callee, JS_NODE_TYPE::LITERAL_SUPER)) {
      if (!_lexContext || !dynamic_cast<JSClassMethodNode *>(_lexContext) ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)->computed ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)->identifier->type !=
              JS_NODE_TYPE::LITERAL_IDENTITY ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)
                  ->identifier->location.get(source) != L"constructor") {
        return createError(L"'super' keyword unexpected here",
                           unwrap(callee)->location);
      }
      pushOperator(program, JS_OPERATOR::SUPER_CALL);
    } else {
      pushOperator(program, JS_OPERATOR::CALL);
    }
    auto addr = program.codes.size();
    program.stacks[addr] = {
        .position = callee->location.end,
        .filename = program.filename,
    };
  } else if (is(node, JS_NODE_TYPE::EXPRESSION_GROUP)) {
    auto expression = node->cast<JSGroupExpressionNode>();
    return resolveMemberChain(source, expression->expression, program,
                              addresses);
  } else {
    return resolve(source, node, program);
  }
  return nullptr;
}

std::unordered_map<JSNode *, size_t>
JSCodeGenerator::beginScope(const std::wstring &source, JSNode *node,
                            JSProgram &program) {
  std::vector<JSLexDeclaration> functions;
  std::unordered_map<JSNode *, size_t> ctx;
  pushOperator(program, JS_OPERATOR::BEGIN);
  ++_scope;
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
      pushOperator(program, JS_OPERATOR::VAR);
      pushString(program, declar.name);
      functions.push_back(declar);
    } break;
    }
  }
  for (auto &declar : functions) {
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
    pushOperator(program, JS_OPERATOR::SET_FUNCTION_NAME);
    pushString(program, declar.name);
    for (auto ref : func->closure) {
      pushOperator(program, JS_OPERATOR::REF);
      pushString(program, ref);
    }
    pushOperator(program, JS_OPERATOR::STORE);
    pushString(program, declar.name);
    pushOperator(program, JS_OPERATOR::POP);
  }
  return ctx;
}

JSNode *JSCodeGenerator::endScope(const std::wstring &source, JSNode *node,
                                  JSProgram &program,
                                  std::unordered_map<JSNode *, size_t> &ctx) {
  if (!ctx.empty()) {
    pushOperator(program, JS_OPERATOR::JMP);
    auto address = program.codes.size();
    pushAddress(program, 0);
    for (auto [declar, addr] : ctx) {
      *(size_t *)(program.codes.data() + addr) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, (JSNode *)declar, program);
      if (err) {
        return err;
      }
    }
    *(size_t *)(program.codes.data() + address) = program.codes.size();
  }
  --_scope;
  pushOperator(program, JS_OPERATOR::END);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveNode(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
  switch (node->type) {
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
    return resolveLabelStatement(source, node, program);
  case JS_NODE_TYPE::STATEMENT_BREAK:
    return resolveBreakStatement(source, node, program);
  case JS_NODE_TYPE::STATEMENT_CONTINUE:
    return resolveContinueStatement(source, node, program);
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
    return resolveImportDeclaration(source, node, program);
  case JS_NODE_TYPE::EXPORT_DECLARATION:
    return resolveExportDeclaration(source, node, program);
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

JSNode *JSCodeGenerator::resolve(const std::wstring &source, JSNode *node,
                                 JSProgram &program) {
  std::unordered_map<JSNode *, size_t> ctx;
  auto funcion = node->cast<JSFunctionBaseNode>();
  auto clazz = node->cast<JSClassDeclarationNode>();
  if (node->scope && !funcion && !clazz &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_IN &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_OF) {
    ctx = beginScope(source, node, program);
  }
  auto err = resolveNode(source, node, program);
  if (err) {
    return err;
  }
  if (node->scope && !funcion && !clazz &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_AWAIT_OF &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_IN &&
      node->type != JS_NODE_TYPE::STATEMENT_FOR_OF) {
    auto err = endScope(source, node, program, ctx);
    if (err) {
      return err;
    }
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveFunctionDeclaration(const std::wstring &source,
                                                    JSNode *node,
                                                    JSProgram &program) {
  auto func = node->cast<JSFunctionBaseNode>();
  auto lexCtx = _lexContext;
  _lexContext = node;
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
      pushOperator(program, JS_OPERATOR::POP);
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
      pushOperator(program, JS_OPERATOR::POP);
    }
  }
  auto err = resolve(source, func->body, program);
  if (err) {
    return err;
  }
  if (func->body->type != JS_NODE_TYPE::DECLARATION_FUNCTION_BODY) {
    pushOperator(program, JS_OPERATOR::RET);
  }
  err = endScope(source, node, program, ctx);
  if (err) {
    return err;
  }
  _lexContext = lexCtx;
  return nullptr;
}

JSNode *JSCodeGenerator::resolveFunctionBodyDeclaration(
    const std::wstring &source, JSNode *node, JSProgram &program) {
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

JSNode *JSCodeGenerator::resolveImportDeclaration(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
  auto declaration = node->cast<JSImportDeclarationNode>();
  auto str = declaration->source->location.get(source);
  str = str.substr(1, str.length() - 2);
  pushOperator(program, JS_OPERATOR::IMPORT);
  pushString(program, str);
  for (auto attr : declaration->attributes) {
    auto attribute = attr->cast<JSImportAttributeNode>();
    auto err = resolve(source, attribute->value, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::ASSERT);
    pushString(program, attribute->key->location.get(source));
  }
  for (auto specifier : declaration->specifiers) {
    if (specifier->type == JS_NODE_TYPE::IMPORT_DEFAULT) {
      auto s = specifier->cast<JSImportDefaultNode>();
      pushOperator(program, JS_OPERATOR::STR);
      pushString(program, L"default");
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 1);
      pushOperator(program, JS_OPERATOR::GET_FIELD);
      pushOperator(program, JS_OPERATOR::STORE);
      pushString(program, s->identifier->location.get(source));
    } else if (specifier->type == JS_NODE_TYPE::IMPORT_NAMESPACE) {
      auto s = specifier->cast<JSImportNamespaceNode>();
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 0);
      pushOperator(program, JS_OPERATOR::STORE);
      pushString(program, s->alias->location.get(source));
    } else if (specifier->type == JS_NODE_TYPE::IMPORT_SPECIFIER) {
      auto s = specifier->cast<JSImportSpecifierNode>();
      pushOperator(program, JS_OPERATOR::STR);
      pushString(program, s->identifier->location.get(source));
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 1);
      pushOperator(program, JS_OPERATOR::GET_FIELD);
      pushOperator(program, JS_OPERATOR::STORE);
      if (s->alias) {
        pushString(program, s->alias->location.get(source));
      } else {
        pushString(program, s->identifier->location.get(source));
      }
    }
  }
  pushOperator(program, JS_OPERATOR::POP);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveExportDeclaration(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
  auto declaration = node->cast<JSExportDeclarationNode>();
  if (declaration->source) {
    auto str = declaration->source->location.get(source);
    str = str.substr(1, str.length() - 2);
    pushOperator(program, JS_OPERATOR::IMPORT);
    pushString(program, str);
    for (auto attr : declaration->attributes) {
      auto attribute = attr->cast<JSImportAttributeNode>();
      auto err = resolve(source, attribute->value, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::ASSERT);
      pushString(program, attribute->key->location.get(source));
    }
    for (auto specifier : declaration->specifiers) {
      if (specifier->type == JS_NODE_TYPE::EXPORT_NAMESPACE) {
        auto s = specifier->cast<JSExportNamespaceNode>();
        if (s->alias) {
          pushOperator(program, JS_OPERATOR::EXPORT);
          pushString(program, s->alias->location.get(source));
        } else {
          pushOperator(program, JS_OPERATOR::EXPORT_ALL);
        }
      } else if (specifier->type == JS_NODE_TYPE::EXPORT_SPECIFIER) {
        auto s = specifier->cast<JSExportSpecifierNode>();
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, s->identifier->location.get(source));
        pushOperator(program, JS_OPERATOR::PUSH_VALUE);
        pushUint32(program, 1);
        pushOperator(program, JS_OPERATOR::GET_FIELD);
        pushOperator(program, JS_OPERATOR::EXPORT);
        if (s->alias) {
          pushString(program, s->alias->location.get(source));
        } else {
          pushString(program, s->identifier->location.get(source));
        }
      }
    }
    pushOperator(program, JS_OPERATOR::POP);
  } else {
    for (auto s : declaration->specifiers) {
      if (s->type == JS_NODE_TYPE::EXPORT_DEFAULT) {
        auto specifier = s->cast<JSExportDefaultNode>();
        auto err = resolve(source, specifier->expression, program);
        if (err) {
          return err;
        }
        pushOperator(program, JS_OPERATOR::EXPORT);
        pushString(program, L"default");
      } else if (s->type == JS_NODE_TYPE::EXPORT_SPECIFIER) {
        auto specifier = s->cast<JSExportSpecifierNode>();
        pushOperator(program, JS_OPERATOR::LOAD);
        pushString(program, specifier->identifier->location.get(source));
        pushOperator(program, JS_OPERATOR::EXPORT);
        if (specifier->alias) {
          pushString(program, specifier->alias->location.get(source));
        } else {
          pushString(program, specifier->identifier->location.get(source));
        }
      } else if (s->type == JS_NODE_TYPE::EXPORT_NAMED) {
        auto specifier = s->cast<JSExportNamedNode>();
        auto err = resolve(source, specifier->declaration, program);
        if (err) {
          return err;
        }
        auto declaration = specifier->declaration;
        if (declaration->type == JS_NODE_TYPE::DECLARATION_CLASS) {
          auto clazz = declaration->cast<JSClassDeclarationNode>();
          pushOperator(program, JS_OPERATOR::LOAD);
          pushString(program, clazz->identifier->location.get(source));
          pushOperator(program, JS_OPERATOR::EXPORT);
          pushString(program, clazz->identifier->location.get(source));
        } else if (declaration->type == JS_NODE_TYPE::DECLARATION_FUNCTION) {
          auto clazz = declaration->cast<JSFunctionDeclarationNode>();
          pushOperator(program, JS_OPERATOR::LOAD);
          pushString(program, clazz->identifier->location.get(source));
          pushOperator(program, JS_OPERATOR::EXPORT);
          pushString(program, clazz->identifier->location.get(source));
        } else if (declaration->type == JS_NODE_TYPE::DECLARATION_VARIABLE) {
          for (auto &declarator :
               declaration->cast<JSVariableDeclaraionNode>()->declarations) {
            auto declar = declarator->cast<JSVariableDeclaratorNode>();
            resolveExport(source, declar->identifier, program);
          }
        }
      }
    }
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveArrowFunctionDeclaration(
    const std::wstring &source, JSNode *node, JSProgram &program) {
  auto func = node->cast<JSArrowDeclarationNode>();
  if (func->async) {
    pushOperator(program, JS_OPERATOR::ASYNCARROW);
  } else {
    pushOperator(program, JS_OPERATOR::ARROW);
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
  return nullptr;
}

JSNode *JSCodeGenerator::resolveFunctionDeclarator(const std::wstring &source,
                                                   JSNode *node,
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

JSNode *JSCodeGenerator::resolveRegexLiteral(const std::wstring &source,
                                             JSNode *node, JSProgram &program) {
  pushOperator(program, JS_OPERATOR::REGEX);
  pushString(program, node->location.get(source));
  return nullptr;
}

JSNode *JSCodeGenerator::resolveNullLiteral(const std::wstring &source,
                                            JSNode *node, JSProgram &program) {
  pushOperator(program, JS_OPERATOR::NIL);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveUndefinedLiteral(const std::wstring &source,
                                                 JSNode *node,
                                                 JSProgram &program) {
  pushOperator(program, JS_OPERATOR::UNDEFINED);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveIdentityLiteral(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  pushOperator(program, JS_OPERATOR::LOAD);
  pushString(program, node->location.get(source));
  return nullptr;
}

JSNode *JSCodeGenerator::resolveBigintLiteral(const std::wstring &source,
                                              JSNode *node,
                                              JSProgram &program) {
  pushOperator(program, JS_OPERATOR::BIGINT);
  auto raw = node->location.get(source);
  raw = raw.substr(0, raw.length() - 1);
  pushString(program, raw);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveTemplateLiteral(const std::wstring &source,
                                                JSNode *node,
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
      if (expr->field->type == JS_NODE_TYPE::PRIVATE_NAME) {
        opt = JS_OPERATOR::PRIVATE_MEMBER_CALL;
      } else {
        opt = JS_OPERATOR::MEMBER_CALL;
      }
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
      if (!_lexContext || !dynamic_cast<JSClassMethodNode *>(_lexContext) ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)->computed ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)->identifier->type !=
              JS_NODE_TYPE::LITERAL_IDENTITY ||
          dynamic_cast<JSClassMethodNode *>(_lexContext)
                  ->identifier->location.get(source) != L"constructor") {
        return createError(L"'super' keyword unexpected here",
                           unwrap(tag)->location);
      }
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
    program.stacks[program.codes.size()] = {.position = temp->tag->location.end,
                                            .filename = program.filename};
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

JSNode *JSCodeGenerator::resolveStringLiteral(const std::wstring &source,
                                              JSNode *node,
                                              JSProgram &program) {
  auto string = unwrap(node)->cast<JSStringLiteralNode>();
  pushOperator(program, JS_OPERATOR::STR);
  auto str = string->location.get(source);
  str = str.substr(1, str.length() - 2);
  pushString(program, str);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveBooleanLiteral(const std::wstring &source,
                                               JSNode *node,
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

JSNode *JSCodeGenerator::resolveNumberLiteral(const std::wstring &source,
                                              JSNode *node,
                                              JSProgram &program) {
  auto number = unwrap(node)->cast<JSNumberLiteralNode>();
  auto str = number->location.get(source);
  double val = std::stold(str);
  pushOperator(program, JS_OPERATOR::PUSH);
  pushNumber(program, val);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveExpressionStatement(const std::wstring &source,
                                                    JSNode *node,
                                                    JSProgram &program) {
  auto statement = unwrap(node)->cast<JSExpressionStatementNode>();
  return resolve(source, statement->expression, program);
}

JSNode *JSCodeGenerator::resolveWhileStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = unwrap(node)->cast<JSWhileStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  auto start = program.codes.size();
  auto err = resolve(source, statement->condition, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::JFALSE);
  auto end_address = program.codes.size();
  pushAddress(program, 0);
  pushOperator(program, JS_OPERATOR::POP);
  err = resolve(source, statement->body, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::JMP);
  pushAddress(program, start);
  auto end = program.codes.size();
  *(size_t *)(program.codes.data() + end_address) = end;
  pushOperator(program, JS_OPERATOR::POP);
  popLabelFrame(program, start);
  popLabelFrame(program, end, label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveDoWhileStatement(const std::wstring &source,
                                                 JSNode *node,
                                                 JSProgram &program) {
  auto statement = unwrap(node)->cast<JSDoWhileStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  pushOperator(program, JS_OPERATOR::UNDEFINED);
  auto start = program.codes.size();
  pushOperator(program, JS_OPERATOR::POP);
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
  pushOperator(program, JS_OPERATOR::POP);
  auto end = program.codes.size();
  popLabelFrame(program, start);
  popLabelFrame(program, end, label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveForStatement(const std::wstring &source,
                                             JSNode *node, JSProgram &program) {
  auto statement = unwrap(node)->cast<JSForStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  if (statement->init) {
    auto err = resolve(source, statement->init, program);
    if (err) {
      return err;
    }
  }
  size_t end_address = 0;
  auto start = program.codes.size();
  if (statement->condition) {
    auto err = resolve(source, statement->condition, program);
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
  if (statement->after) {
    auto err = resolve(source, statement->after, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::JMP);
  pushAddress(program, start);
  auto end = program.codes.size();
  if (end_address != 0) {
    *(size_t *)(program.codes.data() + end_address) = end;
  }
  popLabelFrame(program, start);
  popLabelFrame(program, end, label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveForOfStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = node->cast<JSForOfStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  auto err = resolve(source, statement->right, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::ITERATOR);
  auto start = program.codes.size();
  pushOperator(program, JS_OPERATOR::BEGIN);
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::NEXT);
  pushOperator(program, JS_OPERATOR::JTRUE);
  auto end_address = program.codes.size();
  pushAddress(program, 0);
  pushOperator(program, JS_OPERATOR::POP);
  if (statement->left->type == JS_NODE_TYPE::DECLARATION_VARIABLE) {
    auto declaration = statement->left->cast<JSVariableDeclaraionNode>();
    if (declaration->declarations.size() != 1) {
      return createError(L"Invalid left-hand side in for-of loop: Must have "
                         L"a single binding.",
                         statement->left->location);
    }
    auto declar =
        declaration->declarations[0]->cast<JSVariableDeclaratorNode>();
    if (declar->initialize) {
      return createError(
          L"for-of loop variable declaration may not have an initializer.",
          declar->initialize->location);
    }
    err = resolveStore(source, declar->identifier, program);
    if (err) {
      return err;
    }
  } else {
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::POP);
  err = resolve(source, statement->body, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::JMP);
  pushAddress(program, start);
  *(size_t *)(program.codes.data() + end_address) = program.codes.size();
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::END);
  popLabelFrame(program, start);
  popLabelFrame(program, program.codes.size(), label);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveForInStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = node->cast<JSForInStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  auto err = resolve(source, statement->right, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::GET_KEYS);
  pushOperator(program, JS_OPERATOR::ITERATOR);
  auto start = program.codes.size();
  pushOperator(program, JS_OPERATOR::BEGIN);
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::NEXT);
  pushOperator(program, JS_OPERATOR::JTRUE);
  auto end_address = program.codes.size();
  pushAddress(program, 0);
  pushOperator(program, JS_OPERATOR::POP);
  if (statement->left->type == JS_NODE_TYPE::DECLARATION_VARIABLE) {
    auto declaration = statement->left->cast<JSVariableDeclaraionNode>();
    if (declaration->declarations.size() != 1) {
      return createError(L"Invalid left-hand side in for-in loop: Must have "
                         L"a single binding.",
                         statement->left->location);
    }
    auto declar =
        declaration->declarations[0]->cast<JSVariableDeclaratorNode>();
    if (declar->initialize) {
      return createError(
          L"for-in loop variable declaration may not have an initializer.",
          declar->initialize->location);
    }
    err = resolveStore(source, declar->identifier, program);
    if (err) {
      return err;
    }
  } else {
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::POP);
  err = resolve(source, statement->body, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::JMP);
  pushAddress(program, start);
  *(size_t *)(program.codes.data() + end_address) = program.codes.size();
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::END);
  popLabelFrame(program, start);
  popLabelFrame(program, program.codes.size(), label);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveForAwaitOfStatement(const std::wstring &source,
                                                    JSNode *node,
                                                    JSProgram &program) {
  auto statement = node->cast<JSForAwaitOfStatementNode>();
  auto label = pushBreakFrame(program);
  pushContinueFrame(program, label);
  auto err = resolve(source, statement->right, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::ITERATOR);
  auto start = program.codes.size();
  pushOperator(program, JS_OPERATOR::BEGIN);
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::AWAIT_NEXT);
  pushOperator(program, JS_OPERATOR::JTRUE);
  auto end_address = program.codes.size();
  pushAddress(program, 0);
  pushOperator(program, JS_OPERATOR::POP);
  if (statement->left->type == JS_NODE_TYPE::DECLARATION_VARIABLE) {
    auto declaration = statement->left->cast<JSVariableDeclaraionNode>();
    if (declaration->declarations.size() != 1) {
      return createError(L"Invalid left-hand side in for-of loop: Must have "
                         L"a single binding.",
                         statement->left->location);
    }
    auto declar =
        declaration->declarations[0]->cast<JSVariableDeclaratorNode>();
    if (declar->initialize) {
      return createError(
          L"for-of loop variable declaration may not have an initializer.",
          declar->initialize->location);
    }
    err = resolveStore(source, declar->identifier, program);
    if (err) {
      return err;
    }
  } else {
    err = resolveStore(source, statement->left, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::POP);
  err = resolve(source, statement->body, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::JMP);
  pushAddress(program, start);
  *(size_t *)(program.codes.data() + end_address) = program.codes.size();
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::END);
  popLabelFrame(program, start);
  popLabelFrame(program, program.codes.size(), label);
  pushOperator(program, JS_OPERATOR::POP);
  pushOperator(program, JS_OPERATOR::POP);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveObjectDeclaration(const std::wstring &source,
                                                  JSNode *node,
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
      auto func = prop->cast<JSObjectMethodNode>();
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
      pushOperator(program, JS_OPERATOR::SET_FUNCTION_NAME);
      pushString(program, func->key->location.get(source));
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
      pushOperator(program, JS_OPERATOR::JMP);
      auto end_address = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + address) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, func, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + end_address) = program.codes.size();
      if (func->computed) {
        auto err = resolve(source, func->key, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, func->key->location.get(source));
      }
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 2);
      pushOperator(program, JS_OPERATOR::SET_METHOD);
      pushOperator(program, JS_OPERATOR::POP);
    } else if (prop->type == JS_NODE_TYPE::OBJECT_ACCESSOR) {
      auto func = prop->cast<JSObjectAccessorNode>();
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
      pushOperator(program, JS_OPERATOR::SET_FUNCTION_NAME);
      pushString(program, func->key->location.get(source));
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
      pushOperator(program, JS_OPERATOR::JMP);
      auto end_address = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + address) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, func, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + end_address) = program.codes.size();
      if (func->computed) {
        auto err = resolve(source, func->key, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, func->key->location.get(source));
      }
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 2);
      if (func->kind == JS_ACCESSOR_TYPE::GET) {
        pushOperator(program, JS_OPERATOR::SET_ACCESSOR_GETTER);
      } else {
        pushOperator(program, JS_OPERATOR::SET_ACCESSOR_SETTER);
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

JSNode *JSCodeGenerator::resolveArrayDeclaration(const std::wstring &source,
                                                 JSNode *node,
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

JSNode *JSCodeGenerator::resolveClassDeclaration(const std::wstring &source,
                                                 JSNode *node,
                                                 JSProgram &program) {
  auto declaration = node->cast<JSClassDeclarationNode>();
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
  auto lexCtx = _lexContext;
  _lexContext = node;
  auto ctx = beginScope(source, node, program);
  pushOperator(program, JS_OPERATOR::PUSH_VALUE);
  pushUint32(program, 0);
  pushOperator(program, JS_OPERATOR::WITH);
  for (auto prop : declaration->properties) {
    if (prop->type == JS_NODE_TYPE::CLASS_METHOD) {
      auto func = prop->cast<JSClassMethodNode>();
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
      pushOperator(program, JS_OPERATOR::SET_FUNCTION_NAME);
      pushString(program, func->identifier->location.get(source));
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
      pushOperator(program, JS_OPERATOR::JMP);
      auto end_address = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + address) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, func, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + end_address) = program.codes.size();
      if (func->computed) {
        auto err = resolve(source, func->identifier, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, func->identifier->location.get(source));
      }
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 2);
      if (func->static_) {
        if (func->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          pushOperator(program, JS_OPERATOR::SET_PRIVATE_METHOD);
        } else {
          pushOperator(program, JS_OPERATOR::SET_METHOD);
        }
      } else {
        if (func->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          pushOperator(program, JS_OPERATOR::SET_PRIVATE_PROP_METHOD);
        } else {
          pushOperator(program, JS_OPERATOR::SET_PROP_METHOD);
        }
      }
      pushOperator(program, JS_OPERATOR::POP);
    } else if (prop->type == JS_NODE_TYPE::CLASS_ACCESSOR) {
      auto func = prop->cast<JSClassAccessorNode>();
      auto funcname = func->identifier->location.get(source);
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
      pushOperator(program, JS_OPERATOR::SET_FUNCTION_NAME);
      pushString(program, func->identifier->location.get(source));
      for (auto &ref : func->closure) {
        pushOperator(program, JS_OPERATOR::REF);
        pushString(program, ref);
      }
      pushOperator(program, JS_OPERATOR::JMP);
      auto end_address = program.codes.size();
      pushAddress(program, 0);
      *(size_t *)(program.codes.data() + address) = program.codes.size();
      auto err = resolveFunctionDeclaration(source, func, program);
      if (err) {
        return err;
      }
      *(size_t *)(program.codes.data() + end_address) = program.codes.size();
      if (func->computed) {
        auto err = resolve(source, func->identifier, program);
        if (err) {
          return err;
        }
      } else {
        pushOperator(program, JS_OPERATOR::STR);
        pushString(program, func->identifier->location.get(source));
      }
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 2);
      if (func->static_) {
        if (func->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          if (func->kind == JS_ACCESSOR_TYPE::GET) {
            pushOperator(program, JS_OPERATOR::SET_PRIVATE_ACCESSOR_GETTER);
          } else {
            pushOperator(program, JS_OPERATOR::SET_PRIVATE_ACCESSOR_SETTER);
          }
        } else {
          if (func->kind == JS_ACCESSOR_TYPE::GET) {
            pushOperator(program, JS_OPERATOR::SET_ACCESSOR_GETTER);
          } else {
            pushOperator(program, JS_OPERATOR::SET_ACCESSOR_SETTER);
          }
        }
      } else {
        if (func->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          if (func->kind == JS_ACCESSOR_TYPE::GET) {
            pushOperator(program,
                         JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_GETTER);
          } else {
            pushOperator(program,
                         JS_OPERATOR::SET_PRIVATE_PROP_ACCESSOR_SETTER);
          }
        } else {
          if (func->kind == JS_ACCESSOR_TYPE::GET) {
            pushOperator(program, JS_OPERATOR::SET_PROP_ACCESSOR_GETTER);
          } else {
            pushOperator(program, JS_OPERATOR::SET_PROP_ACCESSOR_SETTER);
          }
        }
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
        if (p->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          pushOperator(program, JS_OPERATOR::SET_PRIVATE_FIELD);
        } else {
          pushOperator(program, JS_OPERATOR::SET_FIELD);
        }
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
        if (p->identifier->type == JS_NODE_TYPE::PRIVATE_NAME) {
          pushOperator(program, JS_OPERATOR::SET_PRIVATE_INITIALIZER);
        } else {
          pushOperator(program, JS_OPERATOR::SET_INITIALIZER);
        }
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
        *(size_t *)(program.codes.data() + end_address) = program.codes.size();
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
  _lexContext = lexCtx;
  return nullptr;
}

JSNode *JSCodeGenerator::resolveVariableDeclaration(const std::wstring &source,
                                                    JSNode *node,
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
    pushOperator(program, JS_OPERATOR::POP);
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveBlockStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = unwrap(node)->cast<JSBlockStatementNode>();
  std::wstring label;
  if (!_label.empty()) {
    label = pushBreakFrame(program);
  }
  for (auto statement : statement->statements) {
    auto err = resolve(source, statement, program);
    if (err) {
      return err;
    }
  }
  if (!label.empty()) {
    popLabelFrame(program, program.codes.size(), label);
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveDebuggerStatement(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
  pushOperator(program, JS_OPERATOR::DEBUGGER);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveReturnStatement(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  auto statement = unwrap(node)->cast<JSReturnStatementNode>();
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

JSNode *JSCodeGenerator::resolveLabelStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = node->cast<JSLabelStatementNode>();
  auto label = _label;
  _label = statement->label->location.get(source);
  auto err = resolve(source, statement->statement, program);
  if (err) {
    return err;
  }
  _label = label;
  return nullptr;
}

JSNode *JSCodeGenerator::resolveBreakStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = node->cast<JSBreakStatementNode>();
  std::wstring label = L"";
  if (statement->label) {
    label = statement->label->location.get(source);
  }
  pushOperator(program, JS_OPERATOR::BREAK);
  pushString(program, label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveContinueStatement(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
  auto statement = node->cast<JSContinueStatementNode>();
  std::wstring label = L"";
  if (statement->label) {
    label = statement->label->location.get(source);
  }
  pushOperator(program, JS_OPERATOR::CONTINUE);
  pushString(program, label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveThrowStatement(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto statement = unwrap(node)->cast<JSThrowStatementNode>();
  auto err = resolve(source, statement->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::THROW);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveTryCatchStatement(const std::wstring &source,
                                                  JSNode *node,
                                                  JSProgram &program) {
  auto statement = unwrap(node)->cast<JSTryCatchStatementNode>();
  if (statement->identifier) {
    auto err = resolveStore(source, statement->identifier, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::POP);
  auto err = resolve(source, statement->body, program);
  if (err) {
    return err;
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveTryStatement(const std::wstring &source,
                                             JSNode *node, JSProgram &program) {
  auto statement = unwrap(node)->cast<JSTryStatementNode>();
  std::wstring label;
  if (!_label.empty()) {
    label = pushBreakFrame(program);
  }
  pushOperator(program, JS_OPERATOR::TRY_BEGIN);
  size_t onerror = 0;
  size_t onfinish = 0;
  if (statement->onfinish) {
    pushOperator(program, JS_OPERATOR::ON_FINISH);
    onfinish = program.codes.size();
    pushAddress(program, 0);
  }
  if (statement->onerror) {
    pushOperator(program, JS_OPERATOR::ON_ERROR);
    onerror = program.codes.size();
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
    pushOperator(program, JS_OPERATOR::DEFER);
    *(size_t *)(program.codes.data() + onfinish_end) = program.codes.size();
  }
  if (!label.empty()) {
    popLabelFrame(program, program.codes.size(), label);
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveIfStatement(const std::wstring &source,
                                            JSNode *node, JSProgram &program) {
  auto statement = unwrap(node)->cast<JSIfStatementNode>();
  std::wstring label;
  if (!_label.empty()) {
    label = pushBreakFrame(program);
  }
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
  pushAddress(program, 0);
  *(uint64_t *)(program.codes.data() + alternate) = program.codes.size();
  pushOperator(program, JS_OPERATOR::POP);
  if (statement->alternate) {
    err = resolve(source, statement->alternate, program);
    if (err) {
      return err;
    }
  }
  *(uint64_t *)(program.codes.data() + end) = program.codes.size();
  if (!label.empty()) {
    popLabelFrame(program, program.codes.size(), label);
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveSwitchStatement(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  auto statement = unwrap(node)->cast<JSSwitchStatementNode>();
  auto label = pushBreakFrame(program);
  auto err = resolve(source, statement->condition, program);
  if (err) {
    return err;
  }
  std::vector<std::pair<JSSwitchCaseStatementNode *, size_t>> addresses;
  JSSwitchCaseStatementNode *defNode = nullptr;
  for (auto c : statement->cases) {
    auto cas = c->cast<JSSwitchCaseStatementNode>();
    if (cas->match) {
      pushOperator(program, JS_OPERATOR::PUSH_VALUE);
      pushUint32(program, 0);
      auto err = resolve(source, cas->match, program);
      if (err) {
        return err;
      }
      pushOperator(program, JS_OPERATOR::SEQ);
      pushOperator(program, JS_OPERATOR::JTRUE);
      addresses.push_back({cas, program.codes.size()});
      pushAddress(program, 0);
      pushOperator(program, JS_OPERATOR::POP);
    } else {
      addresses.push_back({cas, 0});
      defNode = cas;
    }
  }
  pushOperator(program, JS_OPERATOR::UNDEFINED);
  if (defNode) {
    for (auto &[cas, addr] : addresses) {
      if (cas == defNode) {
        pushOperator(program, JS_OPERATOR::JMP);
        addr = program.codes.size();
        pushAddress(program, 0);
        break;
      }
    }
  }
  for (auto &[cas, addr] : addresses) {
    *(uint64_t *)(program.codes.data() + addr) = program.codes.size();
    pushOperator(program, JS_OPERATOR::POP);
    for (auto statement : cas->statements) {
      auto err = resolve(source, statement, program);
      if (err) {
        return err;
      }
    }
    pushOperator(program, JS_OPERATOR::UNDEFINED);
  }
  pushOperator(program, JS_OPERATOR::POP);
  popLabelFrame(program, program.codes.size(), label);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveBinaryExpression(const std::wstring &source,
                                                 JSNode *node,
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
  } else if (opt == L"!") {
    auto err = resolve(source, expression->right, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::LNOT);
  } else if (opt == L"~") {
    auto err = resolve(source, expression->right, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::NOT);
  } else if (opt == L"+" && !expression->left) {
    auto err = resolve(source, expression->right, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::UPLUS);
  } else if (opt == L"-" && !expression->left) {
    auto err = resolve(source, expression->right, program);
    if (err) {
      return err;
    }
    pushOperator(program, JS_OPERATOR::UNEG);
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
    if (opt == L">") {
      pushOperator(program, JS_OPERATOR::GT);
    }
    if (opt == L"<") {
      pushOperator(program, JS_OPERATOR::LT);
    }
    if (opt == L">=") {
      pushOperator(program, JS_OPERATOR::GE);
    }
    if (opt == L"<=") {
      pushOperator(program, JS_OPERATOR::LE);
    }
    if (opt == L"==") {
      pushOperator(program, JS_OPERATOR::EQ);
    }
    if (opt == L"===") {
      pushOperator(program, JS_OPERATOR::SEQ);
    }
    if (opt == L"!=") {
      pushOperator(program, JS_OPERATOR::NE);
    }
    if (opt == L"!==") {
      pushOperator(program, JS_OPERATOR::SNE);
    }
  }
  return nullptr;
}

JSNode *JSCodeGenerator::resolveMemberExpression(const std::wstring &source,
                                                 JSNode *node,
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

JSNode *JSCodeGenerator::resolveOptionalMemberExpression(
    const std::wstring &source, JSNode *node, JSProgram &program) {
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

JSNode *JSCodeGenerator::resolveComputedMemberExpression(
    const std::wstring &source, JSNode *node, JSProgram &program) {
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

JSNode *JSCodeGenerator::resolveOptionalComputedMemberExpression(
    const std::wstring &source, JSNode *node, JSProgram &program) {
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

JSNode *JSCodeGenerator::resolveConditionExpression(const std::wstring &source,
                                                    JSNode *node,
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

JSNode *JSCodeGenerator::resolveCallExpression(const std::wstring &source,
                                               JSNode *node,
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

JSNode *JSCodeGenerator::resolveOptionalCallExpression(
    const std::wstring &source, JSNode *node, JSProgram &program) {
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

JSNode *JSCodeGenerator::resolveNewExpression(const std::wstring &source,
                                              JSNode *node,
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
  program.stacks[program.codes.size()] = {.position =
                                              expression->callee->location.end,
                                          .filename = program.filename};
  pushUint32(program, (uint32_t)expression->arguments.size());
  return nullptr;
}

JSNode *JSCodeGenerator::resolveDeleteExpression(const std::wstring &source,
                                                 JSNode *node,
                                                 JSProgram &program) {
  auto expression = unwrap(node)->cast<JSDeleteExpressionNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::DELETE);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveVoidExpression(const std::wstring &source,
                                               JSNode *node,
                                               JSProgram &program) {
  auto expression = unwrap(node)->cast<JSVoidExpressionNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::VOID);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveTypeofExpression(const std::wstring &source,
                                                 JSNode *node,
                                                 JSProgram &program) {
  auto expression = unwrap(node)->cast<JSTypeofExpressioNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::TYPEOF);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveYieldExpression(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  if (!_lexContext || !dynamic_cast<JSFunctionBaseNode *>(_lexContext) ||
      !dynamic_cast<JSFunctionBaseNode *>(_lexContext)->generator) {
    return createError(L"yield expression must used in generator",
                       node->location);
  }
  auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::YIELD);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveYieldDelegateExpression(
    const std::wstring &source, JSNode *node, JSProgram &program) {
  if (!_lexContext || !dynamic_cast<JSFunctionBaseNode *>(_lexContext) ||
      !dynamic_cast<JSFunctionBaseNode *>(_lexContext)->generator) {
    return createError(L"yield expression must used in generator",
                       node->location);
  }
  auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::YIELD_DELEGATE);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveAwaitExpression(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  if (!_lexContext || !dynamic_cast<JSFunctionBaseNode *>(_lexContext) ||
      !dynamic_cast<JSFunctionBaseNode *>(_lexContext)->async) {
    return createError(L"await expression must used in async", node->location);
  }
  auto expression = unwrap(node)->cast<JSYieldExpressionNode>();
  auto err = resolve(source, expression->value, program);
  if (err) {
    return err;
  }
  pushOperator(program, JS_OPERATOR::AWAIT);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveGroupExpression(const std::wstring &source,
                                                JSNode *node,
                                                JSProgram &program) {
  auto expression = node->cast<JSGroupExpressionNode>();
  return resolve(source, expression->expression, program);
}

JSNode *JSCodeGenerator::resolveAssigmentExpression(const std::wstring &source,
                                                    JSNode *node,
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
  pushOperator(program, JS_OPERATOR::POP);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveThis(const std::wstring &source, JSNode *node,
                                     JSProgram &program) {
  pushOperator(program, JS_OPERATOR::THIS);
  return nullptr;
}

JSNode *JSCodeGenerator::resolveProgram(const std::wstring &source,
                                        JSNode *node, JSProgram &program) {
  auto prog = unwrap(node)->cast<JSProgramNode>();
  for (auto &directive : prog->directives) {
    auto str = directive->location.get(source);
    str = str.substr(1, str.size() - 2);
    pushOperator(program, JS_OPERATOR::ENABLE);
    pushString(program, str);
  }
  auto ctx = _lexContext;
  _lexContext = node;
  for (auto statement : prog->statements) {
    auto err = resolve(source, statement, program);
    if (err) {
      return err;
    }
  }
  pushOperator(program, JS_OPERATOR::HLT);
  _lexContext = ctx;
  return nullptr;
}