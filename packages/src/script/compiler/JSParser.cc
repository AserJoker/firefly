#include "script/compiler/JSParser.hpp"
#include <string>
JSNode *JSParser::resolveDeclarator(const JS_DECLARATION_TYPE &type,
                                    const std::wstring &source,
                                    JSNode *identifier, JSNode *declaration) {
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

JSNode *JSParser::declarVariable(const JS_DECLARATION_TYPE &type,
                                 const std::wstring &source,
                                 JSNode *declaration) {
  if (declaration->type == JS_NODE_TYPE::VARIABLE_DECLARATOR) {
    auto declarator = dynamic_cast<JSVariableDeclaratorNode *>(declaration);
    auto err =
        resolveDeclarator(type, source, declarator->identifier, declaration);
    if (err) {
      return err;
    }
  } else if (declaration->type == JS_NODE_TYPE::DECLARATION_FUNCTION_ARGUMENT) {
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
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
      if (err) {
        return err;
      }
    }
  } else if (declaration->type == JS_NODE_TYPE::DECLARATION_CLASS) {
    auto declarator = dynamic_cast<JSClassDeclarationNode *>(declaration);
    if (declarator->identifier) {
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
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
    auto err = resolveDeclarator(type, source, declarator->alias, declaration);
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
      auto err =
          resolveDeclarator(type, source, declarator->identifier, declaration);
      if (err) {
        return err;
      }
    }
  }
  return nullptr;
}
void JSParser::resolveBinding(const std::wstring &source, JSNode *node) {
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
          auto clazz = dynamic_cast<JSClassDeclarationNode *>(scope->node);
          if (clazz) {
            clazz->closure.insert(name);
            clazz->scope->refs.insert(name);
          }
        }
      }
    }
  }
}

JSNode *JSParser::readSymbolToken(const std::wstring &source,
                                  JSPosition &position) {
  static const std::vector<std::wstring> operators = {
      L">>>=",   L"...", L"<<=", L">>>", L"===", L"!==", L"**=", L">>=", L"&&=",
      LR"(??=)", L"**",  L"==",  L"!=",  L"<<",  L">>",  L"<=",  L">=",  L"&&",
      L"||",     L"??",  L"++",  L"--",  L"+=",  L"-=",  L"*=",  L"/=",  L"%=",
      L"||=",    L"&=",  L"^=",  L"|=",  L"=>",  L"?.",  L"=",   L"*",   L"/",
      L"%",      L"+",   L"-",   L"<",   L">",   L"&",   L"^",   L"|",   L",",
      L"!",      L"~",   L"(",   L")",   L"[",   L"]",   L"{",   L"}",   L"@",
      L"#",      L".",   L"?",   L":",   L";",
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
      auto token = _allocator->create<JSTokenNode>();
      token->location = getLocation(source, position, current);
      position = current;
      return token;
    }
  }
  return nullptr;
}

JSNode *JSParser::readComments(const std::wstring &source, JSPosition &position,
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

JSNode *JSParser::readTemplateLiteral(const std::wstring &source,
                                      JSPosition &position, JSNode *tag) {
  auto current = position;
  if (source[current.offset] != '`') {
    return nullptr;
  }
  auto node = _allocator->create<JSTemplateLiteralNode>();
  current.offset++;
  auto start = current;
  JSNode *quasi = nullptr;
  for (;;) {
    if (!source[current.offset]) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (source[current.offset] == '`') {
      auto end = current;
      quasi = _allocator->create<JSStringLiteralNode>();
      quasi->location = getLocation(source, start, end);
      node->quasis.push_back(quasi);
      quasi->addParent(node);
      current.offset++;
      break;
    }
    if (source[current.offset] == '$' && source[current.offset + 1] == '{') {
      auto end = current;
      quasi = _allocator->create<JSStringLiteralNode>();
      quasi->location = getLocation(source, start, end);
      node->quasis.push_back(quasi);
      quasi->addParent(node);
      current.offset++;
      current.offset++;
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      auto exp = readExpression(source, current);
      if (!exp) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (exp->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return exp;
      }
      node->expressions.push_back(exp);
      exp->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      if (source[current.offset] != '}') {
        _allocator->dispose(node);
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

JSNode *JSParser::readPrivateName(const std::wstring &source,
                                  JSPosition &position) {

  auto current = position;
  if (source[current.offset] == '#') {
    current.offset++;
  } else {
    return nullptr;
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    return createError(L"Invalid or unexpected token", source, current);
  }
  auto node = _allocator->create<JSPrivateNameNode>();
  node->location = getLocation(source, position, current);
  _allocator->dispose(identifier);
  position = current;
  return node;
}

JSNode *JSParser::readIdentifyLiteral(const std::wstring &source,
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
    auto node = _allocator->create<JSIdentityLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readStringLiteral(const std::wstring &source,
                                    JSPosition &position) {

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
    auto token = _allocator->create<JSStringLiteralNode>();
    token->location = getLocation(source, position, current);
    position = current;
    return token;
  }
  return nullptr;
}

JSNode *JSParser::readNullLiteral(const std::wstring &source,
                                  JSPosition &position) {
  auto current = position;
  auto identify = readIdentifyLiteral(source, current);
  if (!identify) {
    return identify;
  }
  if (identify->location.is(source, L"null")) {
    auto node = _allocator->create<JSNullLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    _allocator->dispose(identify);
    return node;
  }
  _allocator->dispose(identify);
  return nullptr;
}

JSNode *JSParser::readUndefinedLiteral(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  auto identify = readIdentifyLiteral(source, current);
  if (!identify) {
    return identify;
  }
  if (identify->location.is(source, L"undefined")) {
    auto node = _allocator->create<JSUndefinedLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    _allocator->dispose(identify);
    return node;
  }
  _allocator->dispose(identify);
  return nullptr;
}

JSNode *JSParser::readThisLiteral(const std::wstring &source,
                                  JSPosition &position) {
  auto current = position;
  auto identify = readIdentifyLiteral(source, current);
  if (!identify) {
    return identify;
  }
  if (identify->location.is(source, L"this")) {
    auto node = _allocator->create<JSThisLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    _allocator->dispose(identify);
    return node;
  }
  _allocator->dispose(identify);
  return nullptr;
}

JSNode *JSParser::readSuperLiteral(const std::wstring &source,
                                   JSPosition &position) {
  auto current = position;
  auto identify = readIdentifyLiteral(source, current);
  if (!identify) {
    return identify;
  }
  if (identify->location.is(source, L"super")) {
    auto node = _allocator->create<JSSuperLiteralNode>();
    node->location = getLocation(source, position, current);
    _allocator->dispose(identify);
    position = current;
    return node;
  }
  _allocator->dispose(identify);
  return nullptr;
}

JSNode *JSParser::readBooleanLiteral(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  auto identify = readIdentifyLiteral(source, current);
  if (!identify) {
    return identify;
  }
  if (identify && (identify->location.is(source, L"true") ||
                   identify->location.is(source, L"false"))) {
    auto node = _allocator->create<JSBooleanLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    _allocator->dispose(identify);
    return node;
  }
  _allocator->dispose(identify);
  return nullptr;
}

JSNode *JSParser::readNumberLiteral(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  if (source[current.offset] >= '0' && source[current.offset] <= '9') {
    if (source[current.offset] == '0') {
      if (source[current.offset + 1] == 'x' ||
          source[current.offset + 1] == 'X') {
        current.offset += 2;
        while (
            (source[current.offset] >= '0' && source[current.offset] <= '9') ||
            (source[current.offset] >= 'a' && source[current.offset] <= 'f') ||
            (source[current.offset] >= 'A' && source[current.offset] <= 'F')) {
          current.offset++;
        }
        auto node = _allocator->create<JSNumberLiteralNode>();
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
      if (source[current.offset + 1] == 'o' ||
          source[current.offset + 1] == 'O') {
        current.offset += 2;
        while (
            (source[current.offset] >= '0' && source[current.offset] <= '7')) {
          current.offset++;
        }
        auto node = _allocator->create<JSNumberLiteralNode>();
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
      if (source[current.offset + 1] == 'b' ||
          source[current.offset + 1] == 'B') {
        current.offset += 2;
        while (
            (source[current.offset] >= '0' && source[current.offset] <= '1')) {
          current.offset++;
        }
        auto node = _allocator->create<JSNumberLiteralNode>();
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
      auto node = _allocator->create<JSBigIntLiteralNode>();
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
    auto node = _allocator->create<JSNumberLiteralNode>();
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
    auto node = _allocator->create<JSNumberLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readRegexpLiteral(const std::wstring &source,
                                    JSPosition &position) {
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

            return createError(L"Invalid or unexpected token", source, current);
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
    auto node = _allocator->create<JSRegexpLiteralNode>();
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readProgram(const std::wstring &source,
                              JSPosition &position) {
  auto current = position;
  auto *node = _allocator->create<JSProgramNode>();
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto interpreter = readInterpreterDirective(source, current);
  if (interpreter) {
    if (interpreter->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
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
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto directive = readDirective(source, current);
    if (!directive) {
      break;
    }
    if (directive->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
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
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto statement = readStatement(source, current);
    if (!statement) {
      break;
    }
    if (statement->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return statement;
    }
    node->statements.push_back(statement);
    statement->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (source[current.offset]) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  popScope();
  return node;
}

JSNode *JSParser::readInterpreterDirective(const std::wstring &source,
                                           JSPosition &position) {
  auto current = position;
  if (source[position.offset] == '#' && source[position.offset + 1] == '!') {
    auto *node = _allocator->create<JSInterpreterDirectiveNode>();
    current.offset += 2;
    while (source[current.offset] &&
           !isLineTerminator(source[current.offset])) {
      current.offset++;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readDirective(const std::wstring &source,
                                JSPosition &position) {
  auto current = position;
  auto node = readStringLiteral(source, current);
  if (!node || node->type == JS_NODE_TYPE::ERROR) {
    return node;
  }
  _allocator->dispose(node);
  node = _allocator->create<JSDirectiveNode>();
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
        _allocator->dispose(node);
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
    _allocator->dispose(node);
    return nullptr;
  }
  position = current;
  return node;
}

JSNode *JSParser::readEmptyStatement(const std::wstring &source,
                                     JSPosition &position) {
  if (checkSymbol({L";"}, source, position)) {
    auto current = position;
    current.offset++;
    auto node = _allocator->create<JSEmptyStatementNode>();
    node->location = getLocation(source, position, current);
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readBlockStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"{"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSBlockStatementNode>();
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto statement = readStatement(source, current);
  while (statement) {
    if (statement->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return statement;
    }
    node->statements.push_back(statement);
    statement->addParent(node);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    statement = readStatement(source, current);
  }
  if (!checkSymbol({L"}"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid o runexcepted token", source, current);
  }
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readDebuggerStatement(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"debugger"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSDebuggerStatementNode>();
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
        _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readReturnStatement(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"return"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSReturnStatementNode>();

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
        _allocator->dispose(node);
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
          _allocator->dispose(node);
          return val;
        }
        node->value = val;
        val->addParent(node);
      } else {
        _allocator->dispose(node);
        return createError(L"Invalid or nexcepted token", source, current);
      }
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readLabelStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  auto identifier = readIdentifyLiteral(source, current);
  if (identifier && isKeyword(source, identifier->location)) {
    _allocator->dispose(identifier);
    return nullptr;
  }
  if (!identifier || identifier->type == JS_NODE_TYPE::ERROR) {
    return identifier;
  }
  auto node = _allocator->create<JSLabelStatementNode>();
  node->label = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L":"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto statement = readStatement(source, current);
  if (!statement) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (statement->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return statement;
  }
  node->statement = statement;
  statement->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readBreakStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"break"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSBreakStatementNode>();
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
        _allocator->dispose(node);
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
          _allocator->dispose(node);
          return val;
        }
        node->label = val;
        val->addParent(node);
      } else {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readContinueStatement(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"continue"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSContinueStatementNode>();
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
        _allocator->dispose(node);
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
          _allocator->dispose(node);
          return val;
        }
        node->label = val;
        val->addParent(node);
      } else {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readIfStatement(const std::wstring &source,
                                  JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"if"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSIfStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto condition = readExpression(source, current);
  if (!condition) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (condition->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return condition;
  }
  node->condition = condition;
  condition->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto consequent = readStatement(source, current);
  if (!consequent) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (consequent->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
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
    _allocator->dispose(node);
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
      _allocator->dispose(node);
      return err;
    }
    auto alternate = readStatement(source, current);
    if (!alternate) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (alternate->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
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

JSNode *JSParser::readSwitchStatement(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"switch"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSSwitchStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto condition = readExpression(source, current);
  if (!condition) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (condition->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return condition;
  }
  node->condition = condition;
  condition->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  if (!checkSymbol({L"{"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto cas = readSwitchCaseStatement(source, current);
  while (cas) {
    if (cas->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return cas;
    }
    node->cases.push_back(cas);
    cas->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    cas = readSwitchCaseStatement(source, current);
  }
  if (!checkSymbol({L"}"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readSwitchCaseStatement(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSSwitchCaseStatementNode>();
  if (checkIdentifier({L"default"}, source, current)) {
  } else if (checkIdentifier({L"case"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto expr = readExpression(source, current);
    if (!expr) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (expr->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return expr;
    }
    node->match = expr;
    expr->addParent(node);
  } else {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L":"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto statement = readStatement(source, current);
  while (statement) {
    if (statement->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return statement;
    }
    node->statements.push_back(statement);
    statement->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    statement = readStatement(source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readThrowStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"throw"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSThrowStatementNode>();

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
        _allocator->dispose(node);
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
          _allocator->dispose(node);
          return val;
        }
        node->value = val;
        val->addParent(node);
      } else {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readTryStatement(const std::wstring &source,
                                   JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"try"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSTryStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto body = readBlockStatement(source, current);
  if (!body) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"catch"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto onerror = readTryCatchStatement(source, current);
    if (!onerror) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (onerror->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return onerror;
    }
    node->onerror = onerror;
    onerror->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
  }
  if (checkIdentifier({L"finally"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto onfinish = readBlockStatement(source, current);
    if (!onfinish) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (onfinish->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return onfinish;
    }
    node->onfinish = onfinish;
    onfinish->addParent(node);
  }
  if (!node->onfinish && !node->onerror) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readTryCatchStatement(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSTryCatchStatementNode>();
  if (checkSymbol({L"("}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->identifier = identifier;
    identifier->addParent(node);
    err = resolveDeclarator(JS_DECLARATION_TYPE::LET, source, identifier, node);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
  }
  auto body = readBlockStatement(source, current);
  if (!body) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readWhileStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"while"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSWhileStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  auto condition = readExpression(source, current);
  if (!condition) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (condition->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return condition;
  }
  node->condition = condition;
  condition->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readDoWhileStatement(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"do"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSDoWhileStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"while"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto condition = readExpression(source, current);
  if (!condition) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (condition->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return condition;
  }
  node->condition = condition;
  condition->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readForStatement(const std::wstring &source,
                                   JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"for"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSForStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  if (!checkSymbol({L"("}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto init = readExpression(source, current);
  if (!init) {
    init = readVariableDeclaration(source, current);
  }
  if (init) {
    if (init->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return init;
    }
    node->init = init;
    init->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L";"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto condition = readExpression(source, current);
  if (condition) {
    if (condition->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return condition;
    }
    node->condition = condition;
    condition->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L";"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto after = readExpression(source, current);
  if (after) {
    if (after->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return after;
    }
    node->after = after;
    after->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  node->location = getLocation(source, current, position);
  popScope();
  position = current;
  return node;
}

JSNode *JSParser::readForInStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"for"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSForInStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  if (!checkSymbol({L"("}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto init = readExpression17(source, current);
  if (!init) {
    init = readVariableDeclaration(source, current);
  }
  if (!init) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  if (init->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return init;
  }
  node->left = init;
  init->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"in"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto expr = readExpression(source, current);
  if (!expr) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (expr->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return expr;
  }
  node->right = expr;
  expr->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  node->location = getLocation(source, position, current);
  popScope();
  position = current;
  return node;
}

JSNode *JSParser::readForOfStatement(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"for"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSForOfStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  if (!checkSymbol({L"("}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto init = readExpression17(source, current);
  if (!init) {
    init = readVariableDeclaration(source, current);
  }
  if (!init) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  if (init->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return init;
  }
  node->left = init;
  init->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"of"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto expr = readExpression(source, current);
  if (!expr) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (expr->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return expr;
  }
  node->right = expr;
  expr->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readForAwaitOfStatement(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"for"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSForAwaitOfStatementNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"await"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  if (!checkSymbol({L"("}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto init = readExpression17(source, current);
  if (!init) {
    init = readVariableDeclaration(source, current);
  }
  if (!init) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (init->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return init;
  }
  node->left = init;
  init->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"of"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto expr = readExpression(source, current);
  if (!expr) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (expr->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return expr;
  }
  node->right = expr;
  expr->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readStatement(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readVariableDeclarationStatement(const std::wstring &source,
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
        _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readVariableDeclaration(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSVariableDeclaraionNode>();
  if (checkIdentifier({L"const"}, source, current)) {
    node->kind = JS_DECLARATION_TYPE::CONST;
  } else if (checkIdentifier({L"let"}, source, current)) {
    node->kind = JS_DECLARATION_TYPE::LET;
  } else if (checkIdentifier({L"var"}, source, current)) {
    node->kind = JS_DECLARATION_TYPE::VAR;
  } else {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto declaration = readVariableDeclarator(source, current);
  if (!declaration) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  for (;;) {
    if (declaration->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return declaration;
    }
    node->declarations.push_back(declaration);
    declaration->addParent(node);
    auto err = declarVariable(node->kind, source, declaration);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
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
      _allocator->dispose(node);
      return err;
    }
    declaration = readVariableDeclarator(source, current);
    if (!declaration) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readVariableDeclarator(const std::wstring &source,
                                         JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSVariableDeclaratorNode>();
  auto identifier = readObjectPattern(source, current);
  if (!identifier) {
    identifier = readArrayPattern(source, current);
  }
  if (!identifier) {
    identifier = readIdentifyLiteral(source, current);
  }
  if (!identifier) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->identifier = identifier;
  identifier->addParent(node);
  auto backup = current;
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"="}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression2(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
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

JSNode *JSParser::readImportNamespace(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"*"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSImportNamespaceNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"as"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto alias = readIdentifyLiteral(source, current);
  if (!alias) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  node->alias = alias;
  alias->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readImportDefault(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    return nullptr;
  }
  if (isKeyword(source, identifier->location)) {
    _allocator->dispose(identifier);
    return nullptr;
  }
  auto node = _allocator->create<JSImportDefaultNode>();
  node->identifier = identifier;
  identifier->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readImportSpecifier(const std::wstring &source,
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
    _allocator->dispose(identifier);
    return nullptr;
  }
  auto node = _allocator->create<JSImportSpecifierNode>();
  node->identifier = identifier;
  identifier->addParent(node);
  auto backup = current;
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"as"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto alias = readIdentifyLiteral(source, current);
    if (!alias) {
      _allocator->dispose(node);
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

JSNode *JSParser::readImportAttribute(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    return nullptr;
  }
  auto node = _allocator->create<JSImportAttributeNode>();
  node->key = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L":"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readStringLiteral(source, current);
  if (!value) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readImportDeclaration(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"import"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSImportDeclarationNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
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
        _allocator->dispose(node);
        return specifier;
      }
      node->specifiers.push_back(specifier);
      specifier->addParent(node);
    } else {
      specifier = readImportDefault(source, current);
      if (specifier) {
        if (specifier->type == JS_NODE_TYPE::ERROR) {
          _allocator->dispose(node);
          return specifier;
        }
        node->specifiers.push_back(specifier);
        specifier->addParent(node);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (checkSymbol({L","}, source, current)) {
          auto backup = current;
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            _allocator->dispose(node);
            return err;
          }
          if (!checkSymbol({L"{"}, source, current)) {
            _allocator->dispose(node);
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
        _allocator->dispose(node);
        return err;
      }
      if (checkSymbol({L"{"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        specifier = readImportSpecifier(source, current);
        for (;;) {
          if (specifier) {
            if (specifier->type == JS_NODE_TYPE::ERROR) {
              _allocator->dispose(node);
              return specifier;
            }
            node->specifiers.push_back(specifier);
            specifier->addParent(node);
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            _allocator->dispose(node);
            return err;
          }
          if (!checkSymbol({L","}, source, current)) {
            break;
          }
          while (skipInvisible(source, current)) {
          }
          err = readComments(source, current, node->comments);
          if (err) {
            _allocator->dispose(node);
            return err;
          }
          specifier = readImportSpecifier(source, current);
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (!checkSymbol({L"}"}, source, current)) {
          _allocator->dispose(node);
          return createError(L"Invalid or unexpected token", source, current);
        }
      }
    }

    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    if (!node->specifiers.empty()) {
      if (!checkIdentifier({L"from"}, source, current)) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexpected token", source, current);
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        _allocator->dispose(node);
        return err;
      }
    }
    auto src = readStringLiteral(source, current);
    if (!src) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (src->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
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
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"assert"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"{"}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto attribute = readImportAttribute(source, current);
    if (attribute) {
      for (;;) {
        if (attribute->type == JS_NODE_TYPE::ERROR) {
          _allocator->dispose(node);
          return attribute;
        }
        node->attributes.push_back(attribute);
        attribute->addParent(node);
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (!checkSymbol({L","}, source, current)) {
          break;
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        attribute = readImportAttribute(source, current);
        if (!attribute) {
          _allocator->dispose(node);
          return createError(L"Invalid or unexpected token", source, current);
        }
      }
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"}"}, source, current)) {
      _allocator->dispose(node);
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
        _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
  }
  for (auto item : node->specifiers) {
    auto err = declarVariable(JS_DECLARATION_TYPE::CONST, source, item);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExportSpecifier(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSExportSpecifierNode>();
  auto left = readIdentifyLiteral(source, current);
  if (!left) {
    left = readStringLiteral(source, current);
  }
  if (!left) {
    _allocator->dispose(node);
    return nullptr;
  }
  if (left->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return left;
  }
  node->identifier = left;
  left->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"as"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    if (identifier->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return identifier;
    }
    node->alias = identifier;
    identifier->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExportNamespaceSpecifier(const std::wstring &source,
                                               JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"*"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSExportNamespaceNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"as"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto alias = readIdentifyLiteral(source, current);
    if (!alias) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (alias->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return alias;
    }
    node->alias = alias;
    alias->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExportDefaultSpecifier(const std::wstring &source,
                                             JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"default"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSExportDefaultNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->expression = readExpression(source, current);
  if (!node->expression) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (node->expression->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return node->expression;
  }
  node->expression->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExportDeclaration(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"export"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSExportDeclarationNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
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
    auto specifier = _allocator->create<JSExportNamedNode>();
    specifier->declaration = item;
    item->addParent(specifier);
    specifier->location = item->location;
    node->specifiers.push_back(specifier);
    specifier->addParent(node);
  } else {
    auto specifier = readExportNamespaceSpecifier(source, current);
    if (specifier) {
      if (specifier->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return specifier;
      }
      node->specifiers.push_back(specifier);
      specifier->addParent(node);
    } else {
      if (checkSymbol({L"{"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        auto err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        auto specifier = readExportSpecifier(source, current);
        for (;;) {
          if (specifier) {
            if (specifier->type == JS_NODE_TYPE::ERROR) {
              _allocator->dispose(node);
              return specifier;
            }
            node->specifiers.push_back(specifier);
            specifier->addParent(node);
          }
          while (skipInvisible(source, current)) {
          }
          auto err = readComments(source, current, node->comments);
          if (err) {
            _allocator->dispose(node);
            return err;
          }
          if (!checkSymbol({L","}, source, current)) {
            break;
          }
          specifier = readExportSpecifier(source, current);
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (!checkSymbol({L"}"}, source, current)) {
          _allocator->dispose(node);
          return createError(L"Invalid or unexcepted token", source, current);
        }
      } else {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    if (checkIdentifier({L"from"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err) {
        _allocator->dispose(node);
        return err;
      }
      auto src = readStringLiteral(source, current);
      if (!src) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
      if (src->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return src;
      }
      node->source = src;
      src->addParent(node);
      auto backup = current;
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err) {
        _allocator->dispose(node);
        return err;
      }
      if (checkIdentifier({L"assert"}, source, current)) {
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (!checkSymbol({L"{"}, source, current)) {
          _allocator->dispose(node);
          return createError(L"Invalid or unexpected token", source, current);
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        auto attribute = readImportAttribute(source, current);
        if (attribute) {
          for (;;) {
            if (attribute->type == JS_NODE_TYPE::ERROR) {
              _allocator->dispose(node);
              return attribute;
            }
            node->attributes.push_back(attribute);
            attribute->addParent(node);
            while (skipInvisible(source, current)) {
            }
            err = readComments(source, current, node->comments);
            if (err) {
              _allocator->dispose(node);
              return err;
            }
            if (!checkSymbol({L","}, source, current)) {
              break;
            }
            while (skipInvisible(source, current)) {
            }
            err = readComments(source, current, node->comments);
            if (err) {
              _allocator->dispose(node);
              return err;
            }
            attribute = readImportAttribute(source, current);
            if (!attribute) {
              _allocator->dispose(node);
              return createError(L"Invalid or unexpected token", source,
                                 current);
            }
          }
        }
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err) {
          _allocator->dispose(node);
          return err;
        }
        if (!checkSymbol({L"}"}, source, current)) {
          _allocator->dispose(node);
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
        _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readStatement(const std::wstring &source,
                                JSPosition &position) {
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
    node = readExportDeclaration(source, current);
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

JSNode *JSParser::readExpressionStatement(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto expression = readExpression(source, current);
  if (!expression || expression->type == JS_NODE_TYPE::ERROR) {
    return expression;
  }
  auto node = _allocator->create<JSExpressionStatementNode>();
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
        _allocator->dispose(node);
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
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectProperty(const std::wstring &source,
                                     JSPosition &position) {
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
  auto node = _allocator->create<JSObjectPropertyNode>();
  if (checkSymbol({L"["}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->key = value;
    value->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"]"}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    node->computed = true;
  } else {
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      _allocator->dispose(node);
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
      _allocator->dispose(node);
      return err;
    }
    auto val = readExpression2(source, current);
    if (!val) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    if (val->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return val;
    }
    node->value = val;
    val->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectMethod(const std::wstring &source,
                                   JSPosition &position) {
  auto current = position;
  auto backup = current;
  auto node = _allocator->create<JSObjectMethodNode>();
  if (checkIdentifier({L"async"}, source, current)) {
    node->async = true;
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
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
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"*"}, source, current)) {
    node->generator = true;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"["}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->key = value;
    value->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"]"}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    node->computed = true;
  } else {
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      _allocator->dispose(node);
      return nullptr;
    }
    node->key = identifier;
    identifier->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto argument = readFunctionArgument(source, current);
  while (argument) {
    if (argument->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return argument;
    }
    node->arguments.push_back(argument);
    argument->addParent(node);
    auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L")"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    argument = readFunctionArgument(source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectAccessor(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSObjectAccessorNode>();
  if (checkIdentifier({L"get"}, source, current)) {
    node->kind = JS_ACCESSOR_TYPE::GET;
  } else if (checkIdentifier({L"set"}, source, current)) {
    node->kind = JS_ACCESSOR_TYPE::SET;
  } else {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }

  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"["}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->key = value;
    value->addParent(node);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"]"}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or nexcepted token", source, current);
    }
    node->computed = true;
  } else {
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      _allocator->dispose(node);
      return nullptr;
    }
    node->key = identifier;
    identifier->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }

  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto argument = readFunctionArgument(source, current);
  while (argument) {
    if (argument->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return argument;
    }
    node->arguments.push_back(argument);
    argument->addParent(node);
    auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L")"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    argument = readFunctionArgument(source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectDeclaration(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"{"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSObjectDeclarationNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  auto item = readObjectProperty(source, current);
  if (item) {
    for (;;) {
      if (item->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return item;
      }
      node->properties.push_back(item);
      item->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      auto backup = current;
      if (checkSymbol({L"}"}, source, current)) {
        current = backup;
        break;
      }
      if (!checkSymbol({L","}, source, current)) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      } else {
        auto backup = current;
        while (skipInvisible(source, current)) {
        }
        err = readComments(source, current, node->comments);
        if (err != nullptr) {
          _allocator->dispose(node);
          return err;
        }
        if (checkSymbol({L"}"}, source, current)) {
          current = backup;
          break;
        }
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      item = readObjectProperty(source, current);
      if (!item) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"}"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readClassProperty(const std::wstring &source,
                                    JSPosition &position) {
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
  auto backup = current;
  auto node = _allocator->create<JSClassPropertyNode>();
  if (checkIdentifier({L"static"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto backup2 = current;
    auto identifier = readIdentifyLiteral(source, current);

    if (!identifier) {
      identifier = readPrivateName(source, current);
    }
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (identifier && identifier->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return identifier;
    }
    if (identifier || checkSymbol({L"*"}, source, current) ||
        checkSymbol({L"["}, source, current)) {
      current = backup2;
      node->static_ = true;
      _allocator->dispose(identifier);
    } else {
      current = backup;
    }
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    identifier = readStringLiteral(source, current);
  }
  if (!identifier) {
    identifier = readPrivateName(source, current);
  }
  if (!identifier) {
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      identifier = readExpression(source, current);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->computed = true;
    }
  }
  if (!identifier) {
    _allocator->dispose(node);
    return nullptr;
  }
  if (identifier->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return identifier;
  }
  node->identifier = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"="}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->value = value;
    value->addParent(node);
  }
  auto res = node;
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
        _allocator->dispose(res);
        return comment;
      }
      res->comments.push_back(comment);
      newline |= isCommentNewline;
      continue;
    }
    break;
  }
  backup = current;
  if (!checkSymbol({L";"}, source, current) && !newline) {
    if (checkSymbol({L"}"}, source, current)) {
      current = backup;
    } else {
      _allocator->dispose(res);
      return createError(L"Invalid or unexpected token", source, current);
    }
  }
  res->location = getLocation(source, position, current);
  position = current;
  return res;
}

JSNode *JSParser::readClassMethod(const std::wstring &source,
                                  JSPosition &position) {
  auto current = position;
  auto backup = current;
  auto node = _allocator->create<JSClassMethodNode>();
  if (checkIdentifier({L"static"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto backup2 = current;
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      identifier = readPrivateName(source, current);
    }
    if (identifier && identifier->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return identifier;
    }
    if (identifier || checkSymbol({L"*"}, source, current) ||
        checkSymbol({L"["}, source, current)) {
      current = backup2;
      node->static_ = true;
      _allocator->dispose(identifier);
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
      _allocator->dispose(node);
      return err;
    }
    auto backup2 = current;
    auto identifier = readIdentifyLiteral(source, current);
    if (!identifier) {
      identifier = readStringLiteral(source, current);
    }
    if (!identifier) {
      identifier = readPrivateName(source, current);
    }
    if (identifier && identifier->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return identifier;
    }
    if (identifier || checkSymbol({L"*"}, source, current) ||
        checkSymbol({L"["}, source, current)) {
      current = backup2;
      node->async = true;
      if (identifier) {
        _allocator->dispose(identifier);
      }
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
      _allocator->dispose(node);
      return err;
    }
  }

  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    identifier = readStringLiteral(source, current);
  }
  if (!identifier) {
    identifier = readPrivateName(source, current);
  }
  if (!identifier) {
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      identifier = readExpression(source, current);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->computed = true;
    }
  }
  if (!identifier) {
    _allocator->dispose(node);
    return nullptr;
  }
  if (identifier->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return identifier;
  }
  node->identifier = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto argument = readFunctionArgument(source, current);
  if (argument) {
    for (;;) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        popScope();
        _allocator->dispose(node);
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
      if (err) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      if (!checkSymbol({L","}, source, current)) {
        break;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      argument = readFunctionArgument(source, current);
      if (!argument) {
        popScope();
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readClassAccessor(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSClassAccessorNode>();
  auto backup = current;
  if (checkIdentifier({L"static"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
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
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"get"}, source, current)) {
    node->kind = JS_ACCESSOR_TYPE::GET;
  } else if (checkIdentifier({L"set"}, source, current)) {
    node->kind = JS_ACCESSOR_TYPE::SET;
  } else {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    identifier = readStringLiteral(source, current);
  }
  if (!identifier) {
    identifier = readPrivateName(source, current);
  }
  if (!identifier) {
    if (checkSymbol({L"["}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      identifier = readExpression(source, current);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      if (!checkSymbol({L"]"}, source, current)) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
      node->computed = true;
    }
  }
  if (!identifier) {
    _allocator->dispose(node);
    return nullptr;
  }
  if (identifier->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return identifier;
  }
  node->identifier = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto argument = readFunctionArgument(source, current);
  if (argument) {
    for (;;) {
      if (argument->type == JS_NODE_TYPE::ERROR) {
        popScope();
        _allocator->dispose(node);
        return argument;
      }
      node->arguments.push_back(argument);
      argument->addParent(node);
      auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
      if (err) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      if (!checkSymbol({L","}, source, current)) {
        break;
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        popScope();
        _allocator->dispose(node);
        return err;
      }
      argument = readFunctionArgument(source, current);
      if (!argument) {
        popScope();
        _allocator->dispose(node);
        return createError(L"Invalid or unexcepted token", source, current);
      }
    }
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readStaticBlock(const std::wstring &source,
                                  JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"static"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSStaticBlockNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  auto block = readBlockStatement(source, current);
  if (!block) {
    popScope();
    _allocator->dispose(node);
    return nullptr;
  }
  if (block->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return block;
  }
  popScope();
  node->statement = block;
  block->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readClassDeclaration(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"class"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSClassDeclarationNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  auto backup = current;
  auto identifier = readIdentifyLiteral(source, current);
  if (identifier) {
    if (identifier->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return identifier;
    }
    if (isKeyword(source, identifier->location)) {
      _allocator->dispose(identifier);
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
    _allocator->dispose(node);
    return err;
  }
  if (checkIdentifier({L"extends"}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto extends = readExpression17(source, current);
    if (!extends) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->extends = extends;
    extends->addParent(node);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  if (!checkSymbol({L"{"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto property = readClassProperty(source, current);
  if (property) {
    for (;;) {
      if (property->type == JS_NODE_TYPE::ERROR) {
        popScope();
        _allocator->dispose(node);
        return property;
      }
      if (property->type == JS_NODE_TYPE::STATEMENT_EMPTY) {
        _allocator->dispose(property);
      } else {
        node->properties.push_back(property);
        property->addParent(node);
      }
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        popScope();
        _allocator->dispose(node);
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
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"}"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  err = declarVariable(JS_DECLARATION_TYPE::CONST, source, node);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  return node;
}

JSNode *JSParser::readArrayDeclaration(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"["}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSArrayDeclarationNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
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
        _allocator->dispose(node);
        return item;
      }
      item->addParent(node);
    }
    node->items.push_back(item);
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L"]"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    item = readExpression2(source, current);
    if (!item) {
      item = readSpreadExpression(source, current);
    }
  }
  if (!checkSymbol({L"]"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readSpreadPattern(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"..."}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSSpreadPatternItemNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readPattern(source, current);
  if (!value) {
    _allocator->dispose(node);
    return nullptr;
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectPatternItem(const std::wstring &source,
                                        JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSObjectPatternItemNode>();
  if (checkSymbol({L"["}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto field = readExpression(source, current);
    if (!field) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (field->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return field;
    }
    if (!checkSymbol({L"]"}, source, current)) {
      _allocator->dispose(node);
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
    _allocator->dispose(node);
    return err;
  }
  if (node->key) {
    if (checkSymbol({L":"}, source, current)) {
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
      auto pattern = readPattern(source, current);
      if (!pattern) {
        _allocator->dispose(node);
        return nullptr;
      }
      if (pattern->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return pattern;
      }
      node->alias = pattern;
      pattern->addParent(node);
    }
  } else {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"="}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression2(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->value = value;
    value->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readObjectPattern(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"{"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSObjectPatternNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"}"}, source, current)) {
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  auto item = readObjectPatternItem(source, current);

  for (;;) {
    if (item) {
      if (item->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return item;
      }
      node->items.push_back(item);
      item->addParent(node);
      while (skipInvisible(source, current)) {
      }
      err = readComments(source, current, node->comments);
      if (err != nullptr) {
        _allocator->dispose(node);
        return err;
      }
    }
    auto backup = current;
    if (checkSymbol({L"}"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      _allocator->dispose(node);
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    item = readObjectPatternItem(source, current);
  }
  if (!checkSymbol({L"}"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readArrayPatternItem(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSArrayPatternItemNode>();
  auto identifier = readPartPattern(source, current);
  if (!identifier || identifier->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return identifier;
  }
  node->alias = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"="}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression2(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    node->value = value;
    value->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readArrayPattern(const std::wstring &source,
                                   JSPosition &position) {
  auto current = position;
  if (!checkSymbol({L"["}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSArrayPatternNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
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
        _allocator->dispose(node);
        return item;
      }
      item->addParent(node);
    }
    node->items.push_back(item);
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L"]"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      _allocator->dispose(node);
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    }
    err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    item = readArrayPatternItem(source, current);
  }
  if (!checkSymbol({L"]"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readPattern(const std::wstring &source,
                              JSPosition &position) {
  auto node = readObjectPattern(source, position);
  if (!node) {
    node = readArrayPattern(source, position);
  }
  if (!node) {
    auto current = position;
    std::vector<JSLexDeclaration> backupDeclarations;
    std::set<std::wstring> backupRefs;
    if (_scope) {
      backupDeclarations = _scope->declarations;
      backupRefs = _scope->refs;
    }
    node = readExpression17(source, current);
    if (node) {
      if (node->type != JS_NODE_TYPE::LITERAL_IDENTITY &&
          node->type != JS_NODE_TYPE::LITERAL_STRING &&
          node->type != JS_NODE_TYPE::LITERAL_NUMBER &&
          node->type != JS_NODE_TYPE::EXPRESSION_MEMBER &&
          node->type != JS_NODE_TYPE::EXPRESSION_COMPUTED_MEMBER) {
        _allocator->dispose(node);
        if (_scope) {
          _scope->declarations = backupDeclarations;
          _scope->refs = backupRefs;
        }
        return nullptr;
      } else {
        position = current;
      }
    }
  }
  return node;
}

JSNode *JSParser::readPartPattern(const std::wstring &source,
                                  JSPosition &position) {
  auto node = readPattern(source, position);
  if (!node) {
    node = readSpreadPattern(source, position);
  }
  return node;
}

JSNode *JSParser::readFunctionArgument(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSFunctionArgumentDeclarationNode>();
  auto identifier = readPartPattern(source, current);
  if (!identifier) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->identifier = identifier;
  identifier->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err != nullptr) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"="}, source, current)) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      _allocator->dispose(node);
      return err;
    }
    auto value = readExpression3(source, current);
    if (!value) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->value = value;
    value->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readFunctionBodyDeclaration(const std::wstring &source,
                                              JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSFunctionBodyDeclarationNode>();
  if (!checkSymbol({L"{"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::BLOCK, node);
  for (;;) {
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, node->comments);
    if (err != nullptr) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto directive = readDirective(source, current);
    if (!directive) {
      break;
    }
    if (directive->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
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
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto statement = readStatement(source, current);
    if (!statement) {
      break;
    }
    if (statement->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return statement;
    }
    node->statements.push_back(statement);
    statement->addParent(node);
  }
  if (!checkSymbol({L"}"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->location = getLocation(source, position, current);
  position = current;
  popScope();
  return node;
}

JSNode *JSParser::readFunctionDeclaration(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSFunctionDeclarationNode>();
  if (checkIdentifier({L"async"}, source, current)) {
    node->async = true;
  }
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkIdentifier({L"function"}, source, current)) {
    _allocator->dispose(node);
    return nullptr;
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"*"}, source, current)) {
    node->generator = true;
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (identifier) {
    node->identifier = identifier;
    identifier->addParent(node);
    if (isKeyword(source, identifier->location)) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }

  if (!checkSymbol({L"("}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto argument = readFunctionArgument(source, current);
  while (argument) {
    if (argument->type == JS_NODE_TYPE::ERROR) {
      popScope();
      _allocator->dispose(node);
      return argument;
    }
    node->arguments.push_back(argument);
    argument->addParent(node);
    auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, argument);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L")"}, source, current)) {
      current = backup;
      break;
    }
    if (!checkSymbol({L","}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return createError(L"Invalid or unexcepted token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    argument = readFunctionArgument(source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  if (body->type == JS_NODE_TYPE::ERROR) {
    popScope();
    _allocator->dispose(node);
    return body;
  }
  node->body = body;
  body->addParent(node);
  popScope();
  node->location = getLocation(source, position, current);
  position = current;
  err = declarVariable(JS_DECLARATION_TYPE::FUNCTION, source, node);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  return node;
}

JSNode *JSParser::readArrowFunctionDeclaration(const std::wstring &source,
                                               JSPosition &position) {
  auto current = position;
  auto node = _allocator->create<JSArrowDeclarationNode>();
  node->scope = pushScope(JS_COMPILE_SCOPE_TYPE::LEX, node);
  auto backup = current;
  if (checkIdentifier({L"async"}, source, current)) {
    node->async = true;
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
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
      popScope();
      _allocator->dispose(node);
      return err;
    }
    auto arg = readFunctionArgument(source, current);
    if (arg) {
      for (;;) {
        if (arg->type == JS_NODE_TYPE::ERROR) {
          popScope();
          _allocator->dispose(node);
          return arg;
        }
        node->arguments.push_back(arg);
        arg->addParent(node);
        auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, arg);
        if (err) {
          popScope();
          _allocator->dispose(node);
          return err;
        }
        while (skipInvisible(source, current)) {
        };
        err = readComments(source, current, node->comments);
        if (err) {
          popScope();
          _allocator->dispose(node);
          return err;
        }
        auto backup = current;
        if (checkSymbol({L")"}, source, current)) {
          current = backup;
          break;
        }
        if (!checkSymbol({L","}, source, current)) {
          popScope();
          _allocator->dispose(node);
          return nullptr;
        }
        while (skipInvisible(source, current)) {
        };
        err = readComments(source, current, node->comments);
        if (err) {
          popScope();
          _allocator->dispose(node);
          return err;
        }
        arg = readFunctionArgument(source, current);
        if (!arg) {
          popScope();
          _allocator->dispose(node);
          return nullptr;
        }
      }
    }

    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return nullptr;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"=>"}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return nullptr;
    }
  } else {
    auto identifier = readIdentifyLiteral(source, current);
    if (identifier == nullptr) {
      popScope();
      _allocator->dispose(node);
      return nullptr;
    }
    auto arg = _allocator->create<JSFunctionArgumentDeclarationNode>();
    arg->identifier = identifier;
    arg->location = identifier->location;
    identifier->addParent(arg);
    node->arguments.push_back(arg);
    arg->addParent(node);
    auto err = declarVariable(JS_DECLARATION_TYPE::LET, source, arg);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      popScope();
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L"=>"}, source, current)) {
      popScope();
      _allocator->dispose(node);
      return nullptr;
    }
  }
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    popScope();
    _allocator->dispose(node);
    return err;
  }
  auto body = readFunctionBodyDeclaration(source, current);
  if (!body) {
    body = readExpression2(source, current);
  }
  if (!body) {
    popScope();
    _allocator->dispose(node);
    return createError(L"Invalid or unexcepted token", source, current);
  }
  node->body = body;
  body->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  popScope();
  return node;
}

JSNode *JSParser::readExpression19(const std::wstring &source,
                                   JSPosition &position) {
  auto node = readStringLiteral(source, position);
  if (!node) {
    node = readNumberLiteral(source, position);
  }
  if (!node) {
    node = readPrivateName(source, position);
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
        _allocator->dispose(node);
        node = nullptr;
      } else {
        position = current;
      }
    }
  }
  return node;
}

JSNode *JSParser::readGroupExpression(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (checkSymbol({L"("}, source, current)) {
    auto node = _allocator->create<JSGroupExpressionNode>();
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
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
      _allocator->dispose(node);
      return err;
    }
    if (!checkSymbol({L")"}, source, current)) {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

JSNode *JSParser::readExpression18(const std::wstring &source,
                                   JSPosition &position) {
  auto node = readGroupExpression(source, position);
  if (!node) {
    node = readExpression19(source, position);
  }
  return node;
}

JSNode *JSParser::readNewCallExpression(const std::wstring &source,
                                        JSPosition &position) {

  auto current = position;
  if (!checkIdentifier({L"new"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSNewExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto callee = readIdentifyLiteral(source, current);
  if (!callee) {
    callee = readGroupExpression(source, current);
  }
  if (!callee) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (callee->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return callee;
  }
  for (;;) {
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      _allocator->dispose(callee);
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
      _allocator->dispose(call);
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    next = readOptionalMemberExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return createError(L"Invalid optional chain from new expression", source,
                         current);
    }
    next = readOptionalComputedMemberExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return createError(L"Invalid optional chain from new expression", source,
                         current);
    }
    next = readOptionalCallExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return createError(L"Invalid optional chain from new expression", source,
                         current);
    }
    next = readMemberExpression(source, current, callee);
    if (!next) {
      next = readComputedMemberExpression(source, current, callee);
    }
    if (next) {
      callee = next;
    }
    if (!next) {
      _allocator->dispose(callee);
      _allocator->dispose(node);
      return nullptr;
    }
  }
  return nullptr;
}

JSNode *JSParser::readMemberExpression(const std::wstring &source,
                                       JSPosition &position, JSNode *left) {
  auto current = position;
  if (!checkSymbol({L"."}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSMemberExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    identifier = readPrivateName(source, current);
  }
  if (!identifier) {
    _allocator->dispose(node);
    return createError(L"Unexpected end of input", source, current);
  }
  node->host = left;
  left->addParent(node);
  node->field = identifier;
  identifier->addParent(node);
  node->location = getLocation(source, left->location.start, current);
  position = current;
  return node;
}

JSNode *JSParser::readComputedMemberExpression(const std::wstring &source,
                                               JSPosition &position,
                                               JSNode *left) {
  auto current = position;
  if (!checkSymbol({L"["}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSComputedMemberExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto field = readExpression(source, current);
  if (!field) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (field->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return field;
  }
  node->field = field;
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L"]"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  node->host = left;
  left->addParent(node);
  field->addParent(node);
  node->location = getLocation(source, left->location.start, current);
  position = current;
  return node;
}

JSNode *JSParser::readSpreadExpression(const std::wstring &source,
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
  auto node = _allocator->create<JSSpreadExpressionNode>();
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readCallExpression(const std::wstring &source,
                                     JSPosition &position, JSNode *left) {
  auto current = position;
  if (!checkSymbol({L"("}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSCallExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto arg = readExpression2(source, current);
  if (!arg) {
    arg = readSpreadExpression(source, current);
  }
  if (arg && arg->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return arg;
  }
  while (arg) {
    node->arguments.push_back(arg);
    arg->addParent(node);
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    auto backup = current;
    if (checkSymbol({L","}, source, current)) {
    } else if (checkSymbol({L")"}, source, current)) {
      current = backup;
      break;
    } else {
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    while (skipInvisible(source, current)) {
    };
    err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      return err;
    }
    arg = readExpression2(source, current);
    if (!arg) {
      arg = readSpreadExpression(source, current);
    }
    if (arg && arg->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return arg;
    }
  }
  while (skipInvisible(source, current)) {
  };
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L")"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  node->callee = left;
  left->addParent(node);
  node->location = getLocation(source, left->location.start, current);
  position = current;
  return node;
}

JSNode *JSParser::readOptionalMemberExpression(const std::wstring &source,
                                               JSPosition &position,
                                               JSNode *left) {
  auto current = position;

  if (!checkSymbol({L"?."}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSOptionalMemberExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto identifier = readIdentifyLiteral(source, current);
  if (!identifier) {
    identifier = readPrivateName(source, current);
  }
  if (!identifier) {
    _allocator->dispose(node);
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

JSNode *JSParser::readOptionalComputedMemberExpression(
    const std::wstring &source, JSPosition &position, JSNode *left) {
  auto current = position;

  if (!checkSymbol({L"?."}, source, current)) {

    return nullptr;
  }
  auto computed = readComputedMemberExpression(source, current, left);
  if (!computed || computed->type == JS_NODE_TYPE::ERROR) {
    return computed;
  }
  auto node = _allocator->create<JSOptionalMemberExpressionNode>();
  auto computedNode = dynamic_cast<JSComputedMemberExpressionNode *>(computed);
  node->host = computedNode->host;
  node->field = computedNode->field;
  node->children = computedNode->children;
  node->comments = computedNode->comments;
  node->location = computedNode->location;
  computedNode->children.clear();
  computedNode->comments.clear();
  _allocator->dispose(computedNode);
  position = current;
  return node;
}

JSNode *JSParser::readOptionalCallExpression(const std::wstring &source,
                                             JSPosition &position,
                                             JSNode *left) {
  auto current = position;

  if (!checkSymbol({L"?."}, source, current)) {
    return nullptr;
  }
  auto call = readCallExpression(source, current, left);
  if (!call || call->type == JS_NODE_TYPE::ERROR) {
    return call;
  }
  auto node = _allocator->create<JSCallExpressionNode>();
  auto callNode = dynamic_cast<JSCallExpressionNode *>(call);
  node->callee = callNode->callee;
  node->arguments = callNode->arguments;
  node->children = callNode->children;
  node->comments = callNode->comments;
  node->location = callNode->location;
  callNode->children.clear();
  callNode->comments.clear();
  _allocator->dispose(callNode);
  position = current;
  return node;
}

JSNode *JSParser::readExpression17(const std::wstring &source,
                                   JSPosition &position) {
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
        _allocator->dispose(node);
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
            _allocator->dispose(next);
            return createError(L"Invalid tagged template on optional chain",
                               source, current);
          }
        }
      }
      if (!next) {
        break;
      }
      if (next->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return next;
      }
      node = next;
      position = current;
    }
  }
  return node;
}

JSNode *JSParser::readNewExpression(const std::wstring &source,
                                    JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"new"}, source, current)) {

    return nullptr;
  }

  auto node = _allocator->create<JSNewExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto callee = readIdentifyLiteral(source, current);
  if (callee && isKeyword(source, callee->location)) {
    current = callee->location.start;
    _allocator->dispose(callee);
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
    _allocator->dispose(node);
    return createError(L"Invalid optional chain from new expression", source,
                       current);
  }
  if (callee->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return callee;
  }
  for (;;) {
    while (skipInvisible(source, current)) {
    };
    auto err = readComments(source, current, node->comments);
    if (err) {
      _allocator->dispose(node);
      _allocator->dispose(callee);
      return err;
    }
    auto next = readCallExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return nullptr;
    }
    next = readOptionalMemberExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    next = readOptionalComputedMemberExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
      return createError(L"Invalid or unexpected token", source, current);
    }
    next = readOptionalCallExpression(source, current, callee);
    if (next) {
      _allocator->dispose(next);
      _allocator->dispose(node);
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

JSNode *JSParser::readExpression16(const std::wstring &source,
                                   JSPosition &position) {
  auto node = readNewExpression(source, position);
  if (!node) {
    node = readExpression17(source, position);
  }
  return node;
}

JSNode *JSParser::readExpression15(const std::wstring &source,
                                   JSPosition &position) {
  auto base = position;
  auto node = readExpression16(source, position);
  if (node && node->type == JS_NODE_TYPE::ERROR) {
    return node;
  }
  if (node) {
    auto current = position;
    auto newline = false;
    auto n = _allocator->create<JSBinaryExpressionNode>();
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
          _allocator->dispose(n);
          _allocator->dispose(node);
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
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readTypeofExpression(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  JSNode *token = nullptr;
  if (!checkIdentifier({L"typeof"}, source, current, &token)) {
    return nullptr;
  }
  auto node = _allocator->create<JSTypeofExpressioNode>();
  token->addParent(node);
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readExpression14(source, current);
  if (!value) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readVoidExpression(const std::wstring &source,
                                     JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"void"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSVoidExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readExpression14(source, current);
  if (!value) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readDeleteExpression(const std::wstring &source,
                                       JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"delete"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSDeleteExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readExpression14(source, current);
  if (!value) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readAwaitExpression(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"await"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSAwaitExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto value = readExpression14(source, current);
  if (!value) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (value->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return value;
  }
  node->value = value;
  value->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExpression14(const std::wstring &source,
                                   JSPosition &position) {
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
      auto n = _allocator->create<JSBinaryExpressionNode>();
      n->opt = token;
      token->addParent(n);
      auto value = readExpression14(source, current);
      if (!value) {
        _allocator->dispose(node);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (value->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(node);
        return value;
      }
      n->right = value;
      value->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    }
  }
  if (!node) {
    node = readExpression15(source, position);
  }
  return node;
}

JSNode *JSParser::readExpression13(const std::wstring &source,
                                   JSPosition &position) {
  auto base = position;
  auto node = readExpression14(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression13(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression12(const std::wstring &source,
                                   JSPosition &position) {
  auto base = position;
  auto node = readExpression13(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression12(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression11(const std::wstring &source,
                                   JSPosition &position) {
  auto base = position;
  auto node = readExpression12(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression11(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression10(const std::wstring &source,
                                   JSPosition &position) {
  auto base = position;
  auto node = readExpression11(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression10(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression9(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression10(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression9(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression8(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression9(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
      return err;
    }
    JSNode *token = nullptr;
    if (checkSymbol({L"==", L"!=", L"===", L"!=="}, source, current, &token)) {
      n->left = node;
      node->addParent(n);
      n->opt = token;
      token->addParent(n);
      while (skipInvisible(source, current)) {
      }
      auto err = readComments(source, current, n->comments);
      if (err) {
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression8(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression7(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression8(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression7(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}
JSNode *JSParser::readExpression6(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression7(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression6(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression5(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression6(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression5(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression4(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression5(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression4(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression3(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression4(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression3(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readYieldExpression(const std::wstring &source,
                                      JSPosition &position) {
  auto current = position;
  if (!checkIdentifier({L"yield"}, source, current)) {
    return nullptr;
  }
  auto node = _allocator->create<JSYieldExpressionNode>();
  while (skipInvisible(source, current)) {
  };
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (checkSymbol({L"*"}, source, current)) {
    auto n = _allocator->create<JSYieldDelegateExpressionNode>();
    n->comments = node->comments;
    node->comments.clear();
    _allocator->dispose(node);
    node = n;
  }
  auto value = readExpression2(source, current);
  if (value) {
    if (value->type == JS_NODE_TYPE::ERROR) {
      _allocator->dispose(node);
      return value;
    }
    node->value = value;
    value->addParent(node);
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readConditionExpression(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto condition = readExpression3(source, current);
  if (!condition || condition->type == JS_NODE_TYPE::ERROR) {
    return condition;
  }
  auto backup = current;
  auto node = _allocator->create<JSConditionExpressionNode>();
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    _allocator->dispose(condition);
    return err;
  }
  if (!checkSymbol({L"?"}, source, current)) {
    _allocator->dispose(node);
    position = backup;
    return condition;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto consequent = readExpression2(source, current);
  if (!consequent) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (consequent->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return consequent;
  }
  node->consequent = consequent;
  consequent->addParent(node);

  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  if (!checkSymbol({L":"}, source, current)) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }

  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto alternate = readExpression2(source, current);
  if (!alternate) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (alternate->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return alternate;
  }
  node->alternate = alternate;
  alternate->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readAssigmentExpression(const std::wstring &source,
                                          JSPosition &position) {
  auto current = position;
  auto left = readPattern(source, current);
  if (!left || left->type == JS_NODE_TYPE::ERROR) {
    return left;
  }
  auto node = _allocator->create<JSAssigmentExpressionNode>();
  node->left = left;
  left->addParent(node);
  while (skipInvisible(source, current)) {
  }
  auto err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  JSNode *token = nullptr;
  if (!checkSymbol({L"=", L"+=", L"-=", L"**=", L"*=", L"/=", L"%=", L"<<=",
                    L">>=", L">>>=", L"&=", L"|=", L"^=", L"&&=", L"||=",
                    LR"(??=)"},
                   source, current, &token)) {
    _allocator->dispose(node);
    return nullptr;
  }
  node->opt = token;
  token->addParent(node);
  err = checkAssigment(left, source);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  while (skipInvisible(source, current)) {
  }
  err = readComments(source, current, node->comments);
  if (err) {
    _allocator->dispose(node);
    return err;
  }
  auto right = readExpression2(source, current);
  if (!right) {
    _allocator->dispose(node);
    return createError(L"Invalid or unexpected token", source, current);
  }
  if (right->type == JS_NODE_TYPE::ERROR) {
    _allocator->dispose(node);
    return right;
  }
  node->right = right;
  right->addParent(node);
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

JSNode *JSParser::readExpression2(const std::wstring &source,
                                  JSPosition &position) {
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

JSNode *JSParser::readExpression1(const std::wstring &source,
                                  JSPosition &position) {
  auto base = position;
  auto node = readExpression2(source, position);
  if (node && node->type != JS_NODE_TYPE::ERROR) {
    auto current = position;
    auto n = _allocator->create<JSBinaryExpressionNode>();
    while (skipInvisible(source, current)) {
    }
    auto err = readComments(source, current, n->comments);
    if (err) {
      _allocator->dispose(n);
      _allocator->dispose(node);
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
        _allocator->dispose(n);
        _allocator->dispose(node);
        return err;
      }
      auto right = readExpression1(source, current);
      if (!right) {
        _allocator->dispose(n);
        return createError(L"Invalid or unexpected token", source, current);
      }
      if (right->type == JS_NODE_TYPE::ERROR) {
        _allocator->dispose(n);
        return right;
      }
      n->right = right;
      right->addParent(n);
      n->location = getLocation(source, base, current);
      position = current;
      node = n;
    } else {
      _allocator->dispose(n);
    }
  }
  return node;
}

JSNode *JSParser::readExpression(const std::wstring &source,
                                 JSPosition &position) {
  auto node = readExpression1(source, position);
  return node;
}

JSNode *JSParser::readComment(const std::wstring &source, JSPosition &position,
                              bool *newline) {
  if (source[position.offset] == '/') {
    if (source[position.offset + 1] == '/') {
      auto current = position;
      while (!isLineTerminator(source[current.offset])) {
        current.offset++;
      }
      auto node = _allocator->create<JSCommentLiteralNode>();
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
        if (isLineTerminator(source[current.offset])) {
          if (newline) {
            *newline = true;
          }
        }
        if (!source[current.offset]) {
          return createError(L"Invalid or unexpected token", source, current);
        }
        current.offset++;
      }
      auto node = _allocator->create<JSMultilineCommentLiteralNode>();
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }
  return nullptr;
}