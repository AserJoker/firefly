#include "script/JSCompiler.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include <array>
#include <codecvt>
#include <locale>
#include <stdexcept>
#include <string>
using namespace firefly;
using namespace firefly::script;

static std::array WSC = {0x9, 0xb, 0xc, 0x20, 0xa0, 0xfeff};
static std::array LTC = {0xa, 0xd, 0x2028, 0x2029};

core::AutoPtr<JSCompiler::Node> JSCompiler::compile(const std::string &filename,
                                                    const std::string &source) {
  Position position = {0, 0, 0};
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  auto root = readProgram(filename, converter.from_bytes(source), position);
  return root;
}

std::string JSCompiler::formatException(const std::string &message,
                                        const std::string &filename,
                                        const std::wstring &source,
                                        Position position) {
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring line;
  std::wstring spaceLine;
  auto index = position.offset - position.column;
  while (source[index] != '\n' && source[index] != '\0') {
    if (source[index] != '\r') {
      line += source[index];
      if (index == position.offset) {
        spaceLine += L'^';
      } else {
        spaceLine += L' ';
      }
    }
    index++;
  }
  if (source[position.offset] == '\r' || source[position.offset] == '\n' ||
      source[position.offset] == '\0') {
    spaceLine += L'^';
  }

  return fmt::format("{}: \n{}\n{}\n  at {}:{}:{}", message,
                     converter.to_bytes(line), converter.to_bytes(spaceLine),
                     filename, position.line + 1, position.column + 1);
}

JSCompiler::Location JSCompiler::getLocation(const std::wstring &source,
                                             Position start, Position end) {
  Location loc;
  loc.start = start;
  loc.end = end;
  loc.source = loc.getSource(source);
  return loc;
}

bool JSCompiler::skipWhiteSpace(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto &chr = source[position.offset];
  if (std::find(WSC.begin(), WSC.end(), chr) == WSC.end()) {
    return false;
  }
  for (;;) {
    auto &chr = source[position.offset];
    if (std::find(WSC.begin(), WSC.end(), chr) != WSC.end()) {
      position.offset++;
      position.column++;
    } else {
      break;
    }
  }
  return true;
}

bool JSCompiler::skipComment(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto token = readCommentToken(filename, source, position);
  if (token != nullptr) {
    position = token->location.end;
    position.offset++;
    position.column++;
    return true;
  }
  return false;
}

bool JSCompiler::skipLineTerminatior(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position) {
  auto &chr = source[position.offset];
  if (std::find(LTC.begin(), LTC.end(), chr) == LTC.end()) {
    return false;
  }
  for (;;) {
    auto &chr = source[position.offset];
    if (source[position.offset] == '\r' &&
        source[position.offset + 1] == '\n') {
      position.offset += 2;
      position.line++;
      position.column = 0;
    } else if (std::find(LTC.begin(), LTC.end(), chr) != LTC.end()) {
      position.offset++;
      position.line++;
      position.column = 0;
    } else {
      break;
    }
  }
  return true;
}

bool JSCompiler::skipSemi(const std::string &filename,
                          const std::wstring &source, Position &position) {
  auto &chr = source[position.offset];
  if (chr == ';') {
    position.offset++;
    position.column++;
    return true;
  }
  return false;
}

void JSCompiler::skipInvisible(const std::string &filename,
                               const std::wstring &source, Position &position) {
  while (skipComment(filename, source, position) ||
         skipWhiteSpace(filename, source, position) ||
         skipLineTerminatior(filename, source, position)) {
    ;
  }
}

void JSCompiler::skipNewLine(const std::string &filename,
                             const std::wstring &source, Position &position) {
  while (skipComment(filename, source, position) ||
         skipWhiteSpace(filename, source, position) ||
         skipLineTerminatior(filename, source, position) ||
         skipSemi(filename, source, position)) {
    ;
  }
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readStringToken(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '\"' || chr == L'\'') {
    core::AutoPtr token = new Token;
    auto start = chr;
    auto mask = 0;
    for (;;) {
      auto &chr = source[current.offset];
      if (chr == '\0' || std::find(LTC.begin(), LTC.end(), chr) != LTC.end()) {
        current.column += current.offset - position.offset;
        throw std::runtime_error(
            formatException("Incomplete string", filename, source, current));
      }
      if (chr == '\\') {
        mask++;
      }
      if (chr == start && current.offset != position.offset && mask % 2 == 0) {
        break;
      }
      current.offset++;
    }
    current.column += current.offset - position.offset;
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readCommentToken(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '/') {
    current.offset++;
    if (source[current.offset] == '/') {
      for (;;) {
        if (source[current.offset] == '\0' ||
            std::find(LTC.begin(), LTC.end(), source[current.offset]) !=
                LTC.end()) {
          break;
        }
        current.offset++;
      }
      current.column += current.offset - position.offset;
      core::AutoPtr token = new Token();
      token->location = getLocation(source, position, current);
      return token;
    }
    if (source[current.offset] == '*') {
      current.offset++;
      for (;;) {
        auto &chr = source[current.offset];
        if (chr == '\0') {
          throw std::runtime_error(
              formatException("Incomplete comment", filename, source, current));
        }
        if (chr == '/' && source[current.offset - 1] == '*') {
          break;
        }
        if (std::find(LTC.begin(), LTC.end(), chr) != LTC.end()) {
          current.line++;
          current.column = 0;
        } else {
          current.column++;
        }
        current.offset++;
      }
      core::AutoPtr token = new Token();
      token->location = getLocation(source, position, current);
      return token;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readNumberToken(const std::string &filename,
                            const std::wstring &source, Position &position) {

  auto current = position;
  auto &chr = source[current.offset];
  if ((chr >= '0' && chr <= '9') ||
      (chr == '.' && source[current.offset + 1] >= '0' &&
       source[current.offset + 1] <= '9')) {
    if (chr == '0' && (source[current.offset + 1] == 'x' ||
                       source[current.offset + 1] == 'X')) {
      current.offset += 2;
      for (;;) {
        auto &chr = source[current.offset];
        if ((chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f') ||
            (chr >= 'A' && chr <= 'F')) {
          current.offset++;
        } else {
          break;
        }
      }
    } else if (chr == '0' && (source[current.offset + 1] == 'o' ||
                              source[current.offset + 1] == 'O')) {
      current.offset += 2;
      for (;;) {
        auto &chr = source[current.offset];
        if (chr >= '0' && chr <= '7') {
          current.offset++;
        } else {
          break;
        }
      }
    } else {
      bool dec = false;
      for (;;) {
        auto &chr = source[current.offset];
        if (chr == '.' && !dec) {
          dec = true;
          current.offset++;
        } else if (chr >= '0' && chr <= '9') {
          current.offset++;
        } else {
          break;
        }
      }
    }
    auto &chr = source[current.offset];
    if (chr == 'e' || chr == 'E') {
      current.offset++;
      for (;;) {
        auto &chr = source[current.offset];
        if (chr >= '0' && chr <= '9') {
          current.offset++;
        } else {
          break;
        }
      }
    }
    current.offset--;
    current.column += current.offset - position.offset;
    core::AutoPtr<Token> token = new Token;
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readRegexToken(const std::string &filename,
                           const std::wstring &source, Position &position) {
  static std::array regexFlags = {'d', 'g', 'i', 'm', 's', 'y'};
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '/') {
    current.offset++;
    auto mask = 0;
    for (;;) {
      auto &chr = source[current.offset];
      if (chr == '\\') {
        mask++;
      }
      if (chr == '/' && mask % 2 == 0) {
        break;
      }
      if (chr == '\0' || std::find(LTC.begin(), LTC.end(), chr) != LTC.end()) {
        current.column += current.offset - position.offset;
        throw std::runtime_error(
            formatException("Incomplete regex", filename, source, current));
      }
      current.offset++;
    }
    auto &chr = source[current.offset + 1];
    if (std::find(regexFlags.begin(), regexFlags.end(), chr) !=
        regexFlags.end()) {
      current.offset++;
      for (;;) {
        auto &chr = source[current.offset];
        if (std::find(regexFlags.begin(), regexFlags.end(), chr) ==
            regexFlags.end()) {
          current.offset--;
          break;
        }
        current.offset++;
      }
    }
    current.column += current.offset - position.offset;
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.column++;
    position.offset++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readBooleanToken(const std::string &filename,
                             const std::wstring &source, Position &position) {
  static wchar_t t[] = L"true";
  static wchar_t f[] = L"false";
  auto current = position;
  auto &chr = source[position.offset];
  wchar_t *p = nullptr;
  if (chr == 't') {
    p = &t[0];
  } else if (chr == 'f') {
    p = &f[0];
  } else {
    return nullptr;
  }
  while (*p) {
    auto &chr = source[current.offset];
    if (*p != chr) {
      return nullptr;
    }
    p++;
    current.offset++;
  }
  core::AutoPtr token = new Token();
  current.column += current.offset - position.offset;
  token->location = getLocation(source, position, current);
  position = current;
  position.column++;
  position.offset++;
  return token;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readNullToken(const std::string &filename,
                          const std::wstring &source, Position &position) {
  static wchar_t s[] = L"null";
  auto current = position;
  auto &chr = source[position.offset];
  if (chr != 'n') {
    return nullptr;
  }
  wchar_t *p = &s[0];
  while (*p) {
    auto &chr = source[current.offset];
    if (*p != chr) {
      return nullptr;
    }
    p++;
    current.offset++;
  }
  core::AutoPtr token = new Token();
  current.column += current.offset - position.offset;
  token->location = getLocation(source, position, current);
  position = current;
  position.column++;
  position.offset++;
  return token;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readUndefinedToken(const std::string &filename,
                               const std::wstring &source, Position &position) {
  static wchar_t s[] = L"undefined";
  auto current = position;
  auto &chr = source[position.offset];
  if (chr != 'u') {
    return nullptr;
  }
  wchar_t *p = &s[0];
  while (*p) {
    auto &chr = source[current.offset];
    if (*p != chr) {
      return nullptr;
    }
    p++;
    current.offset++;
  }
  core::AutoPtr token = new Token();
  current.column += current.offset - position.offset;
  token->location = getLocation(source, position, current);
  position = current;
  position.column++;
  position.offset++;
  return token;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readIdentifierToken(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') || chr == '_' ||
      chr == '$') {
    current.offset++;
    for (;;) {
      auto &chr = source[current.offset];
      if ((chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') ||
          chr == '_' || chr == '$' || (chr >= '0' && chr <= '9')) {
        current.offset++;
      } else {
        current.offset--;
        break;
      }
    }
    current.column += current.offset - position.offset;
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.column++;
    position.offset++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readSymbolToken(const std::string &filename,
                            const std::wstring &source, Position &position) {
  static const core::Array<std::wstring> operators = {
      L">>>=",   L"...", L"<<=", L">>>", L"===", L"!==", L"**=", L">>=", L"&&=",
      LR"(??=)", L"**",  L"==",  L"!=",  L"<<",  L">>",  L"<=",  L">=",  L"&&",
      L"||",     L"??",  L"++",  L"--",  L"+=",  L"-=",  L"*=",  L"/=",  L"%=",
      L"||=",    L"&=",  L"^=",  L"|=",  L"=>",  L"?.",  L"=",   L"*",   L"/",
      L"%",      L"+",   L"-",   L"<",   L">",   L"&",   L"^",   L"|",   L",",
      L"!",      L"~",   L"(",   L")",   L"[",   L"]",   L"{",   L"}",   L"@",
      L".",      L"?",   L":",   L";",
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
    offset--;
    if (found) {
      current.offset = offset;
      current.column += current.offset - position.offset;
      core::AutoPtr token = new Token();
      token->location = getLocation(source, position, current);
      position = current;
      position.column++;
      position.offset++;
      return token;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readTemplateToken(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '`') {
    current.offset++;
    int mask = 0;
    for (;;) {
      auto chr = source[current.offset];
      if (chr == '\\') {
        mask++;
      }
      if (chr == '`' && mask % 2 == 0) {
        break;
      }
      if (mask % 2 == 0 && chr == '{' && source[current.offset - 1] == '$') {
        return nullptr;
      }
      if (chr == '\0') {
        throw std::runtime_error(
            formatException("Incomplete string", filename, source, current));
      }
      if (chr == '\r') {
        current.offset++;
        chr = source[current.offset];
        if (chr == '\n') {
          current.offset++;
        }
        current.line++;
        current.column = 0;
      } else if (chr == '\n') {
        current.offset++;
        current.line++;
        current.column = 0;
      } else {
        current.offset++;
        current.column++;
      }
    }
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readTemplateStartToken(const std::string &filename,
                                   const std::wstring &source,
                                   Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '`') {
    current.offset++;
    int mask = 0;
    for (;;) {
      auto chr = source[current.offset];
      if (chr == '\\') {
        mask++;
      }
      if (mask % 2 == 0 && chr == '{' && source[current.offset - 1] == '$') {
        break;
      }
      if (mask % 2 == 0 && chr == '`') {
        return nullptr;
      }
      if (chr == '\0') {
        throw std::runtime_error(
            formatException("Incomplete string", filename, source, current));
      }
      if (chr == '\r') {
        current.offset++;
        chr = source[current.offset];
        if (chr == '\n') {
          current.offset++;
        }
        current.line++;
        current.column = 0;
      } else if (chr == '\n') {
        current.offset++;
        current.line++;
        current.column = 0;
      } else {
        current.offset++;
        current.column++;
      }
    }
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readTemplatePatternToken(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position) {

  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '}') {
    current.offset++;
    int mask = 0;
    for (;;) {
      auto chr = source[current.offset];
      if (chr == '\\') {
        mask++;
      }
      if (mask % 2 == 0 && chr == '{' && source[current.offset - 1] == '$') {
        break;
      }
      if (mask % 2 == 0 && chr == '`') {
        return nullptr;
      }
      if (chr == '\0') {
        throw std::runtime_error(
            formatException("Incomplete string", filename, source, current));
      }
      if (chr == '\r') {
        current.offset++;
        chr = source[current.offset];
        if (chr == '\n') {
          current.offset++;
        }
        current.line++;
        current.column = 0;
      } else if (chr == '\n') {
        current.offset++;
        current.line++;
        current.column = 0;
      } else {
        current.offset++;
        current.column++;
      }
    }
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readTemplateEndToken(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  auto &chr = source[current.offset];
  if (chr == '}') {
    current.offset++;
    int mask = 0;
    for (;;) {
      auto chr = source[current.offset];
      if (chr == '\\') {
        mask++;
      }
      if (mask % 2 == 0 && chr == '`') {
        break;
      }
      if (mask % 2 == 0 && chr == '{' && source[current.offset - 1] == '$') {
        return nullptr;
      }
      if (chr == '\0') {
        throw std::runtime_error(
            formatException("Incomplete string", filename, source, current));
      }
      if (chr == '\r') {
        current.offset++;
        chr = source[current.offset];
        if (chr == '\n') {
          current.offset++;
        }
        current.line++;
        current.column = 0;
      } else if (chr == '\n') {
        current.offset++;
        current.line++;
        current.column = 0;
      } else {
        current.offset++;
        current.column++;
      }
    }
    core::AutoPtr token = new Token();
    token->location = getLocation(source, position, current);
    position = current;
    position.offset++;
    position.column++;
    return token;
  }
  return nullptr;
}

bool JSCompiler::insertExpressionNode(core::AutoPtr<Node> &current,
                                      core::AutoPtr<Node> &child,
                                      bool &isComplete) {
  if (!current) {
    current = child;
    if (child->type == NodeType::EXPRESSION_UNARY) {
      isComplete = false;
    } else {
      isComplete = true;
    }
    return true;
  } else {
    auto next = child.cast<BinaryExpression>();
    auto root = current;
    if (root->level <= child->level) {
      if (!next) {
        return false;
      }
      if (next->type == NodeType::EXPRESSION_UNARY) {
        next->right = root;
      } else {
        next->left = root;
      }
      current = next;
    } else {
      auto r = root.cast<BinaryExpression>();
      while (r->right != nullptr && r->right->level > child->level) {
        r = r->right.cast<BinaryExpression>();
      }
      if (next != nullptr) {
        if (next->type == NodeType::EXPRESSION_UNARY) {
          next->right = r->right;
        } else {
          next->left = r->right;
        }
        r->right = next;
      } else {
        r->right = child;
      }
    }
    if (child->type == NodeType::EXPRESSION_BINARY ||
        child->type == NodeType::EXPRESSION_UNARY) {
      isComplete = false;
    } else {
      isComplete = true;
    }
    return true;
  }
  return false;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readProgram(const std::string &filename, const std::wstring &source,
                        Position &position) {
  auto current = position;
  auto interpreter = readInterpreterDirective(filename, source, current);
  core::Array<core::AutoPtr<Node>> directives;
  auto directive = readDirective(filename, source, current);
  while (directive != nullptr) {
    directives.pushBack(directive);
    directive = readDirective(filename, source, current);
  }
  core::AutoPtr program = new JSCompiler::Program();
  program->type = NodeType::PROGRAM;
  program->interpreter = interpreter;
  program->directives = directives;
  skipNewLine(filename, source, current);
  while (source[current.offset] != '\0') {
    auto statement = readStatement(filename, source, current);
    if (!statement) {
      break;
    }
    program->body.pushBack(statement);
    skipNewLine(filename, source, current);
  }
  skipNewLine(filename, source, current);
  if (source[current.offset] != '\0') {
    throw std::runtime_error(
        formatException("Unexcepted token", filename, source, current));
  }
  program->level = 0;
  program->location = getLocation(source, position, current);
  position = current;
  return program;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readStatement(const std::string &filename,
                          const std::wstring &source, Position &position) {
  core::AutoPtr<Node> node = nullptr;
  if (!node) {
    node = readBlockStatement(filename, source, position);
  }

  // if (!node) {
  //   node = readWhileStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readIfStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readSwitchStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readForOfStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readForInStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readForStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readDefStatement(filename,source,position);
  // }

  // if (!node) {
  //   node = readWithStatement(filename,source,position);
  // }

  if (node != nullptr) {
    return node;
  }
  return readExpressionStatement(filename, source, position);
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExpressionStatement(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto expr = readExpression(filename, source, current);
  if (!expr) {
    return nullptr;
  }
  skipInvisible(filename, source, current);
  if (source[current.offset] == '\0') {
    position = current;
    return expr;
  }
  auto next = current;
  auto token = readSymbolToken(filename, source, current);
  if (token == nullptr) {
    throw std::runtime_error(
        formatException("Unexcepted token", filename, source, current));
  }
  if (token->location.getSource(source) == L",") {
    auto right = readExpressionStatement(filename, source, current);
    if (!right) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new BinaryExpression;
    node->opt = L",";
    node->level = 18;
    node->left = expr;
    node->right = right;
    node->updateLocation(source);
    expr = node;
    position = current;
    return expr;
  } else if (token->location.getSource(source) == L";") {
    position = current;
    return expr;
  } else if (token->location.getSource(source) == L"}" ||
             token->location.getSource(source) == L"]" ||
             token->location.getSource(source) == L")") {
    current = next;
    position = current;
    return expr;
  } else {
    throw std::runtime_error(
        formatException("Unexcepted token", filename, source, current));
  }
}
core::AutoPtr<JSCompiler::Node>
JSCompiler::readBlockStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {

  auto current = position;
  skipNewLine(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.getSource(source) == L"{") {
    core::AutoPtr node = new BlockStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_BLOCK;
    skipNewLine(filename, source, current);
    auto statement = readStatement(filename, source, current);
    while (statement != nullptr) {
      node->body.pushBack(statement);
      statement = readStatement(filename, source, current);
      skipNewLine(filename, source, current);
    }
    token = readSymbolToken(filename, source, current);
    if (!token || token->location.getSource(source) != L"}") {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, position));
    }
    current.column--;
    current.offset--;
    node->location = getLocation(source, position, current);
    current.offset++;
    current.column++;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExpression(const std::string &filename,
                           const std::wstring &source, Position &position) {
  core::AutoPtr<Node> root;
  Position current = position;
  bool isComplete = true;
  for (;;) {
    bool newline = false;
    do {
      bool flag = true;
      while (flag) {
        flag = skipComment(filename, source, current);
        flag |= skipWhiteSpace(filename, source, current);
        bool currentNewline = skipLineTerminatior(filename, source, current);
        flag |= currentNewline;
        newline |= currentNewline;
      }
    } while (0);
    Position next = current;
    core::AutoPtr<Node> node;
    if (root == nullptr || !isComplete) {
      if (!node) {
        node = readStringLiteral(filename, source, next);
      }
      if (!node) {
        node = readTemplateLiteral(filename, source, next);
      }
      if (!node) {
        node = readNumberLiteral(filename, source, next);
      }
      if (!node) {
        node = readRegexLiteral(filename, source, next);
      }
      if (!node) {
        node = readBooleanLiteral(filename, source, next);
      }
      if (!node) {
        node = readNullLiteral(filename, source, next);
      }
      if (!node) {
        node = readUndefinedLiteral(filename, source, next);
      }
      if (!node) {
        node = readArrowFunctionDefinition(filename, source, next);
      }
      if (!node) {
        node = readIdentifierLiteral(filename, source, next);
      }
      if (!node) {
        node = readGroupExpression(filename, source, next);
      }
      // if (!node) {
      //   node = readArrayPattern(filename, source, next);
      // }
      // if (!node) {
      //   node = readObjectPattern(filename, source, next);
      // }
      // if (!node) {
      //   node = readFunctionDefinition(filename, source, next);
      // }
      // if (!node) {
      //   node = readClassDefinition(filename, source, next);
      // }
      // if (!node && root != nullptr) {
      //   node = readObjectDefinition(filename, source, next);
      // }
      // if (!node) {
      //   node = readArrayDefinition(filename, source, next);
      // }
      if (!node) {
        node = readUnaryExpression(filename, source, next);
      }
    } else if (isComplete) {
      if (!node) {
        node = readBinaryExpression(filename, source, next);
      }
      if (!node) {
        node = readUpdateExpression(filename, source, next);
      }
      if (!node) {
        node = readConditionExpression(filename, source, next);
      }
      // if (!node) {
      //   node = readOptionalMemberExpression(filename, source, next);
      // }
      // if (!node) {
      //   node = readMemberExpression(filename, source, next);
      // }
      // if (!node) {
      //   node = readCallExpression(filename, source, next);
      // }
    }
    if (!root && !node) {
      return nullptr;
    }
    if (!node && !isComplete) {
      next.column += next.offset - current.offset;
      throw std::runtime_error(
          formatException("Unexpected token", filename, source, next));
    }
    if (isComplete && !node) {

      if (newline || source[next.offset] == L'\0' ||
          source[next.offset] == L';' || source[next.offset] == L',' ||
          source[next.offset] == L'?' || source[next.offset] == L':' ||
          source[next.offset] == L'}' || source[next.offset] == L']' ||
          source[next.offset] == L')') {
        current = next;
        break;
      }
      next.column += next.offset - current.offset;
      throw std::runtime_error(
          formatException("Unexpected token", filename, source, next));
    }
    if (!insertExpressionNode(root, node, isComplete)) {
      if (!isComplete) {
        next.column += next.offset - current.offset;
        throw std::runtime_error(
            formatException("Unexpected token", filename, source, next));
      } else {
        break;
      }
    } else {
      current = next;
    }
  }
  position = current;
  auto r = root.cast<BinaryExpression>();
  if (r != nullptr) {
    r->updateLocation(source);
  }
  return root;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readInterpreterDirective(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position) {
  auto current = position;
  skipNewLine(filename, source, current);
  auto &chr = source[current.offset];
  if (chr == '#' && source[position.offset + 1] == '!') {
    for (;;) {
      auto &chr = source[current.offset];
      if (std::find(LTC.begin(), LTC.end(), chr) != LTC.end()) {
        current.offset--;
        break;
      }
      current.offset++;
    }
    current.column += current.offset - position.offset;
    core::AutoPtr node = new InterpreterDirective();
    node->type = NodeType::INTERPRETER_DIRECTIVE;
    node->level = 0;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readDirective(const std::string &filename,
                          const std::wstring &source, Position &position) {
  auto current = position;
  skipNewLine(filename, source, current);
  auto value = readStringToken(filename, source, current);
  if (value != nullptr) {
    core::AutoPtr node = new Directive();
    node->location = value->location;
    node->type = NodeType::DIRECTIVE;
    node->level = 0;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readStringLiteral(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readStringToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new StringLiteral;
    auto src = token->location.getSource(source);
    node->value = src.substr(1, src.length() - 2);
    node->location = token->location;
    node->type = NodeType::LITERAL_STRING;
    node->level = -2;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readTemplateLiteral(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto tag = readIdentifierToken(filename, source, current);
  auto temp = readTemplateToken(filename, source, current);
  core::AutoPtr node = new TemplateLiteral;
  node->type = NodeType::LITERAL_TEMPLATE;
  node->level = -2;
  if (temp != nullptr) {
    auto src = temp->location.getSource(source);
    node->quasis.pushBack(src.substr(1, src.length() - 2));
    node->location = getLocation(source, position, current);
    if (tag != nullptr) {
      node->tag = tag->location.getSource(source);
    }
    position = current;
    return node;
  }
  temp = readTemplateStartToken(filename, source, current);
  if (temp != nullptr) {
    auto src = temp->location.getSource(source);
    node->quasis.pushBack(src.substr(1, src.length() - 3));
    for (;;) {
      auto exp = readExpression(filename, source, current);
      if (!exp) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->expressions.pushBack(exp);
      auto part = readTemplatePatternToken(filename, source, current);
      if (part != nullptr) {
        auto src = part->location.getSource(source);
        node->quasis.pushBack(src.substr(1, src.length() - 3));
      } else {
        part = readTemplateEndToken(filename, source, current);
        if (!part) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        auto src = part->location.getSource(source);
        node->quasis.pushBack(src.substr(1, src.length() - 2));
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readNumberLiteral(const std::string &filename,
                              const std::wstring &src, Position &position) {
  auto current = position;
  skipInvisible(filename, src, current);
  auto token = readNumberToken(filename, src, current);
  if (token != nullptr) {
    core::AutoPtr node = new NumberLiteral;
    auto source = token->location.getSource(src).substr(
        0, token->location.getSource(src).length());
    if (source[0] == '0' && (source[1] == 'x' || source[1] == 'X')) {
      node->value = (double)std::stol(source, nullptr, 16);
    } else if (source[0] == '0' && (source[1] == 'o' && source[1] == 'O')) {
      node->value = (double)std::stol(source, nullptr, 8);
    } else {
      node->value = std::stold(source);
    }
    node->location = token->location;
    node->type = NodeType::LITERAL_NUMBER;
    node->level = -2;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readRegexLiteral(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readRegexToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new RegexLiteral;
    auto src = token->location.getSource(source);
    auto end = src.find_last_of('/');
    node->value = src.substr(1, end - 1);
    auto flags = src.substr(end + 1);
    for (auto &ch : flags) {
      if (ch == 'd')
        node->hasIndices = true;
      else if (ch == 'g')
        node->global = true;
      else if (ch == 'i')
        node->ignoreCase = true;
      else if (ch == 'm')
        node->multiline = true;
      else if (ch == 's')
        node->dotAll = true;
      else if (ch == 'y')
        node->sticky = true;
    }
    node->location = token->location;
    node->type = NodeType::LITERAL_REGEX;
    node->level = -2;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readBooleanLiteral(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readBooleanToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  core::AutoPtr node = new BooleanLiteral();
  node->value = token->location.getSource(source) == L"true";
  node->location = token->location;
  node->level = -2;
  node->type = NodeType::LITERAL_BOOLEAN;
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readUndefinedLiteral(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readUndefinedToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  core::AutoPtr node = new UndefinedLiteral();
  node->location = token->location;
  node->type = NodeType::LITERAL_UNDEFINED;
  node->level = -2;
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readNullLiteral(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readNullToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  core::AutoPtr node = new NullLiteral();
  node->location = token->location;
  node->type = NodeType::LITERAL_NULL;
  node->level = -2;
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readIdentifierLiteral(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  core::AutoPtr node = new IdentifierLiteral();
  node->value = token->location.getSource(source);
  node->location = token->location;
  node->level = -2;
  node->type = NodeType::LITERAL_IDENTITY;
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readBinaryExpression(const std::string &filename,
                                 const std::wstring &source,
                                 JSCompiler::Position &position) {
  const core::Array<core::Array<std::wstring>> operators = {
      {},
      {},
      {},
      {},
      {L"**"},
      {L"*", L"/", L"%"},
      {L"+", L"-"},
      {L">>>", L"<<", L">>"},
      {L"<=", L">=", L"<", L">"},
      {L"===", L"!==", L"==", L"!="},
      {L"&"},
      {L"^"},
      {L"|"},
      {L"&&"},
      {L"||"},
      {L"??"},
      {},
      {L"=", L"+=", L"-=", L"**=", L"*=", L"/=", L"%=", L">>>=", L"<<=", L">>=",
       L"&&=", L"||=", L"&=", L"^=", L"|=", LR"(??=)"},
      {}};
  auto current = position;
  skipWhiteSpace(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  for (size_t level = 0; level < operators.size(); level++) {
    for (auto &opt : operators[level]) {
      if (opt == token->location.getSource(source)) {
        core::AutoPtr node = new BinaryExpression;
        node->location = token->location;
        node->level = level;
        node->opt = opt;
        node->type = NodeType::EXPRESSION_BINARY;
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readConditionExpression(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  if (token->location.getSource(source) == L"?") {
    auto next = current;
    auto trusy = readExpression(filename, source, current);
    if (!trusy) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || token->location.getSource(source) != L":") {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto falsy = readExpression(filename, source, current);
    if (!falsy) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new ConditionExpression;
    node->level = 16;
    node->type = JSCompiler::NodeType::EXPRESSION_CONDITION;

    core::AutoPtr vnode = new BinaryExpression;
    vnode->level = -2;
    vnode->type = NodeType::EXPRESSION_BINARY;

    vnode->location = getLocation(source, next, current);
    vnode->left = trusy;
    vnode->right = falsy;

    node->right = vnode;

    current.offset--;
    current.column--;
    node->location = getLocation(source, position, current);
    current.offset++;
    current.column++;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readUpdateExpression(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr) {
    if (token->location.getSource(source) == L"++" ||
        token->location.getSource(source) == L"--") {
      core::AutoPtr node = new UpdateExpression;
      node->location = token->location;
      node->level = -2;
      node->type = NodeType::EXPRESSION_UPDATE;
      node->opt = token->location.getSource(source);
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readUnaryExpression(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  static core::Array<std::wstring> unarys = {L"!", L"~",  L"+",
                                             L"-", L"++", L"--"};
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr) {
    auto src = token->location.getSource(source);
    if (unarys.contains(src)) {
      core::AutoPtr node = new UnaryExpression;
      node->location = token->location;
      node->level = 4;
      node->type = NodeType::EXPRESSION_UNARY;
      node->opt = token->location.getSource(source);
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readGroupExpression(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {

  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.getSource(source) == L"(") {
    auto expr = readExpressionStatement(filename, source, current);
    if (!expr) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    token = readSymbolToken(filename, source, current);
    if (!token || token->location.getSource(source) != L")") {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new GroupExpression;
    node->expression = expr;
    node->level = 0;
    node->type = NodeType::EXPRESSION_GROUP;
    current.offset--;
    current.column--;
    node->location = getLocation(source, position, current);
    current.offset++;
    current.column++;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readParameter(const std::string &filename,
                          const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new Parameter;
    node->identifier = token->location.getSource(source);
    node->level = 0;
    node->type = NodeType::DEFINITION_PARAMETER;
    auto next = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      return nullptr;
    }
    if (token->location.getSource(source) == L"=") {
      auto value = readExpression(filename, source, current);
      if (!value) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->value = value;
      next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
    }
    if (token->location.getSource(source) == L"," ||
        token->location.getSource(source) == L")") {
      current = next;
      current.offset--;
      current.column--;
      node->location = getLocation(source, position, current);
      current.offset++;
      current.column++;
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readRestParameter(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.getSource(source) == L"...") {
    core::AutoPtr node = new Parameter;
    node->level = 0;
    node->type = NodeType::DEFINITION_PARAMETER;
    auto token = readIdentifierToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->identifier = token->location.getSource(source);
    current.offset--;
    current.column--;
    node->location = getLocation(source, position, current);
    current.offset++;
    current.column++;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readArrowFunctionDefinition(const std::string &filename,
                                        const std::wstring &source,
                                        Position &position) {

  auto current = position;
  skipInvisible(filename, source, current);
  auto async = readIdentifierToken(filename, source, current);
  core::AutoPtr node = new ArrowFunctionDefinition();
  if (async == nullptr || async->location.getSource(source) != L"async") {
    async = nullptr;
    current = position;
    node->async = false;
  } else {
    node->async = true;
  }
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.getSource(source) == L"(") {
    auto param = readParameter(filename, source, current);
    while (param != nullptr) {
      node->parameters.pushBack(param);
      skipInvisible(filename, source, current);
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (!token) {
        return nullptr;
      }
      if (token->location.getSource(source) != L",") {
        if (token->location.getSource(source) == L")") {
          current = next;
          break;
        } else {
          return nullptr;
        }
      }
      param = readParameter(filename, source, current);
      if (!param) {
        break;
      }
    }
    param = readRestParameter(filename, source, current);
    if (param != nullptr) {
      node->restParameter = param;
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || token->location.getSource(source) != L")") {
      return nullptr;
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || token->location.getSource(source) != L"=>") {
      return nullptr;
    }
    auto next = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (token != nullptr) {
      auto symbol = token->location.getSource(source);
      current = next;
      if (symbol == L"{") {
        node->body = readBlockStatement(filename, source, current);
      } else {
        node->body = readExpression(filename, source, current);
      }
    } else {
      node->body = readExpression(filename, source, current);
    }
    node->level = -2;
    node->type = NodeType::DEFINITION_ARROW_FUNCTION;
    current.offset--;
    current.column--;
    node->location = getLocation(source, position, current);
    current.offset++;
    current.column++;
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readComment(const std::string &filename, const std::wstring &source,
                        Position &position) {
  auto current = position;
  while (skipWhiteSpace(filename, source, position) ||
         skipLineTerminatior(filename, source, position)) {
    ;
  }
  auto token = readCommentToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new Comment();
    node->location = token->location;
    node->level = 0;
    node->type = NodeType::LITERAL_COMMENT;
    if (node->location.getSource(source)[1] == L'*') {
      node->type = NodeType::LITERAL_MULTILINE_COMMENT;
    }
    return node;
  }
  return nullptr;
}