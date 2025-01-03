#include "script/JSCompiler.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include <algorithm>
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
                                             Position &start, Position &end) {
  Location loc;
  loc.start = start;
  loc.end = end;
  loc.end.offset--;
  loc.end.column--;
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
  auto node = readComment(filename, source, position);
  if (node != nullptr) {
    position = node->location.end;
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
                               const std::wstring &source, Position &position,
                               bool *isNewline) {
  auto current = position;
  while (skipLineTerminatior(filename, source, position) ||
         skipComment(filename, source, position) ||
         skipWhiteSpace(filename, source, position)) {
    ;
  }
  if (isNewline) {
    *isNewline = position.line != current.line;
  }
}

void JSCompiler::skipNewLine(const std::string &filename,
                             const std::wstring &source, Position &position,
                             bool *isNewline) {
  auto current = position;
  while (skipComment(filename, source, position) ||
         skipWhiteSpace(filename, source, position) ||
         skipLineTerminatior(filename, source, position) ||
         skipSemi(filename, source, position)) {
  }
  if (isNewline) {
    *isNewline = position.line != current.line;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
      current.offset++;
      current.column++;
      token->location = getLocation(source, position, current);
      position = current;
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
          current.column++;
          current.offset++;
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
      current.offset++;
      current.column++;
      token->location = getLocation(source, position, current);
      position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readBigIntToken(const std::string &filename,
                            const std::wstring &source, Position &position) {

  auto current = position;
  auto &chr = source[current.offset];
  if ((chr >= '0' && chr <= '9')) {
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
      for (;;) {
        auto &chr = source[current.offset];
        if (chr >= '0' && chr <= '9') {
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
    auto flag = source[current.offset];
    if (flag == 'n') {
      current.offset++;
    } else {
      return nullptr;
    }
    current.offset--;
    current.column += current.offset - position.offset;
    core::AutoPtr<Token> token = new Token;
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
  current.offset--;
  current.column += current.offset - position.offset;
  current.offset++;
  token->location = getLocation(source, position, current);
  position = current;
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
  current.offset--;
  current.column += current.offset - position.offset;
  current.offset++;
  token->location = getLocation(source, position, current);
  position = current;
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
  current.offset--;
  current.column += current.offset - position.offset;
  current.offset++;
  token->location = getLocation(source, position, current);
  position = current;
  return token;
}

core::AutoPtr<JSCompiler::Token>
JSCompiler::readIdentifierToken(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  // TODO: unicode support
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
    offset--;
    if (found) {
      current.offset = offset;
      current.column += current.offset - position.offset;
      core::AutoPtr token = new Token();
      current.offset++;
      current.column++;
      token->location = getLocation(source, position, current);
      position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
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
    current.offset++;
    current.column++;
    token->location = getLocation(source, position, current);
    position = current;
    return token;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readProgram(const std::string &filename, const std::wstring &source,
                        Position &position) {
  auto current = position;
  auto interpreter = readInterpreterDirective(filename, source, current);
  core::AutoPtr program = new JSCompiler::Program();
  program->type = NodeType::PROGRAM;
  program->interpreter = interpreter;
  auto directive = readDirective(filename, source, current);
  while (directive != nullptr) {
    program->directives.pushBack(directive);
    directive = readDirective(filename, source, current);
  }
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
    node = readEmptyStatement(filename, source, position);
  }
  if (!node) {
    node = readLabelStatement(filename, source, position);
  }
  if (!node) {
    node = readBlockStatement(filename, source, position);
  }
  if (!node) {
    node = readImportDeclaration(filename, source, position);
  }
  if (!node) {
    node = readExportDeclaration(filename, source, position);
  }
  if (!node) {
    node = readDebuggerStatement(filename, source, position);
  }
  if (!node) {
    node = readWhileStatement(filename, source, position);
  }
  if (!node) {
    node = readDoWhileStatement(filename, source, position);
  }
  if (!node) {
    node = readYieldStatement(filename, source, position);
  }
  if (!node) {
    node = readReturnStatement(filename, source, position);
  }
  if (!node) {
    node = readBreakStatement(filename, source, position);
  }
  if (!node) {
    node = readContinueStatement(filename, source, position);
  }
  if (!node) {
    node = readIfStatement(filename, source, position);
  }
  if (!node) {
    node = readSwitchStatement(filename, source, position);
  }
  if (!node) {
    node = readTryStatement(filename, source, position);
  }
  if (!node) {
    node = readForOfStatement(filename, source, position);
  }

  if (!node) {
    node = readForInStatement(filename, source, position);
  }

  if (!node) {
    node = readForStatement(filename, source, position);
  }

  // if (!node) {
  //   node = readWithStatement(filename,source,position);
  // }
  if (node != nullptr) {
    return node;
  }
  if (!node) {
    node = readVariableDeclaration(filename, source, position);
  }
  if (node == nullptr) {
    node = readExpressionStatement(filename, source, position);
  }
  if (node != nullptr) {
    bool isNewLine = false;
    auto current = position;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine && source[current.offset] != '\0') {
      auto token = readSymbolToken(filename, source, current);
      if (token == nullptr || (!token->location.startsWith(source, L";") &&
                               !token->location.startsWith(source, L"}"))) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      } else {
        if (token != nullptr && token->location.startsWith(source, L";")) {
          position = current;
        }
      }
    }
  }
  return node;
}

JSCompiler::NodeArray JSCompiler::readStatements(const std::string &filename,
                                                 const std::wstring &source,
                                                 Position &position) {
  NodeArray result;
  skipNewLine(filename, source, position);
  auto statement = readStatement(filename, source, position);
  while (statement != nullptr) {
    result.pushBack(statement);
    statement = readStatement(filename, source, position);
    skipNewLine(filename, source, position);
  }
  return result;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readDebuggerStatement(const std::string &filename,
                                  const std::wstring &source,
                                  JSCompiler::Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (identifier != nullptr &&
      identifier->location.startsWith(source, L"debugger")) {
    core::AutoPtr node = new DebuggerStatement;
    node->type = NodeType::STATEMENT_DEBUGGER;
    node->level = 0;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readWhileStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"while")) {
    auto node = new WhileStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_WHILE;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->condition = readExpression(filename, source, current);
    if (!node->condition) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->body = readStatement(filename, source, current);
    if (!node->body) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readDoWhileStatement(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"do")) {
    core::AutoPtr node = new DoWhileStatement;
    node->type = NodeType::STATEMENT_DO_WHILE;
    node->body = readStatement(filename, source, current);
    if (!node->body) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token == nullptr || !token->location.startsWith(source, L"while")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->condition = readExpression(filename, source, current);
    if (!node->condition) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readForStatement(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"for")) {
    core::AutoPtr node = new ForStatement;
    node->type = NodeType::STATEMENT_FOR_IN;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->init = readExpression(filename, source, current);
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L";")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->condition = readExpression(filename, source, current);
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L";")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->update = readExpression(filename, source, current);
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->body = readStatement(filename, source, current);
    if (!node->body) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readForInStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"for")) {
    core::AutoPtr node = new ForInStatement;
    node->type = NodeType::STATEMENT_FOR_IN;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr) {
      if (token->location.startsWith(source, L"const")) {
        node->kind = DeclarationKind::CONST;
      } else if (token->location.startsWith(source, L"let")) {
        node->kind = DeclarationKind::LET;
      } else if (token->location.startsWith(source, L"var")) {
        node->kind = DeclarationKind::VAR;
      } else {
        node->kind = DeclarationKind::UNKNOWN;
        current = backup;
      }
    }
    node->declaration = readIdentifierLiteral(filename, source, current);
    if (!node->declaration) {
      node->declaration = readObjectPattern(filename, source, current);
    }
    if (!node->declaration) {
      node->declaration = readArrayDeclaration(filename, source, current);
    }
    if (node->declaration != nullptr) {
      skipInvisible(filename, source, current);
      token = readIdentifierToken(filename, source, current);
      if (token != nullptr && token->location.startsWith(source, L"in")) {
        node->expression = readExpression(filename, source, current);
        if (!node->expression) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        skipInvisible(filename, source, current);
        token = readSymbolToken(filename, source, current);
        if (!token || !token->location.startsWith(source, L")")) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->body = readStatement(filename, source, current);
        if (!node->body) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readForOfStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"for")) {
    core::AutoPtr node = new ForOfStatement;
    node->type = NodeType::STATEMENT_FOR_OF;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr) {
      if (token->location.startsWith(source, L"const")) {
        node->kind = DeclarationKind::CONST;
      } else if (token->location.startsWith(source, L"let")) {
        node->kind = DeclarationKind::LET;
      } else if (token->location.startsWith(source, L"var")) {
        node->kind = DeclarationKind::VAR;
      } else {
        node->kind = DeclarationKind::UNKNOWN;
        current = backup;
      }
    }
    node->declaration = readIdentifierLiteral(filename, source, current);
    if (!node->declaration) {
      node->declaration = readObjectPattern(filename, source, current);
    }
    if (!node->declaration) {
      node->declaration = readArrayDeclaration(filename, source, current);
    }
    if (node->declaration != nullptr) {
      skipInvisible(filename, source, current);
      token = readIdentifierToken(filename, source, current);
      if (token != nullptr && token->location.startsWith(source, L"of")) {
        node->expression = readExpression(filename, source, current);
        if (!node->expression) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        skipInvisible(filename, source, current);
        token = readSymbolToken(filename, source, current);
        if (!token || !token->location.startsWith(source, L")")) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->body = readStatement(filename, source, current);
        if (!node->body) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readEmptyStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L";")) {
    core::AutoPtr node = new EmptyStatement;
    node->type = NodeType::STATEMENT_EMPTY;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readYieldStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"yield")) {
    core::AutoPtr node = new YieldStatement;
    node->type = NodeType::STATEMENT_YIELD;
    node->level = 0;
    auto isNewLine = false;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine) {
      node->value = readExpression(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readReturnStatement(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"return")) {
    core::AutoPtr node = new YieldStatement;
    node->type = NodeType::STATEMENT_YIELD;
    node->level = 0;
    auto isNewLine = false;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine) {
      node->value = readExpression(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readThrowStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"throw")) {
    core::AutoPtr node = new ThrowStatement;
    node->type = NodeType::STATEMENT_THROW;
    node->level = 0;
    auto isNewLine = false;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine) {
      node->value = readExpression(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readBreakStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"break")) {
    core::AutoPtr node = new BreakStatement;
    node->type = NodeType::STATEMENT_BREAK;
    node->level = 0;
    auto isNewLine = false;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine) {
      node->label = readIdentifierLiteral(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readContinueStatement(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"continue")) {
    core::AutoPtr node = new BreakStatement;
    node->type = NodeType::STATEMENT_CONTINUE;
    node->level = 0;
    auto isNewLine = false;
    skipInvisible(filename, source, current, &isNewLine);
    if (!isNewLine) {
      node->label = readIdentifierLiteral(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readLabelStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto label = readIdentifierLiteral(filename, source, current);
  if (label != nullptr) {
    auto token = readSymbolToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L":")) {
      core::AutoPtr node = new LabelStatement;
      node->type = NodeType::STATEMENT_LABEL;
      node->label = label;
      node->statement = readStatement(filename, source, current);
      if (!node->statement) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readIfStatement(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"if")) {
    core::AutoPtr node = new IfStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_IF;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->condition = readExpression(filename, source, current);
    if (!node->condition) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->alternate = readStatement(filename, source, current);
    if (!node->alternate) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"else")) {
      node->consequent = readStatement(filename, source, current);
      if (!node->consequent) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readSwitchStatement(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"switch")) {
    core::AutoPtr node = new SwitchStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_SWITCH;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"(")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->expression = readExpression(filename, source, current);
    if (!node->expression) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"{")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto case_ = readSwitchCaseStatement(filename, source, current);
    while (case_ != nullptr) {
      node->cases.pushBack(case_);
      case_ = readSwitchCaseStatement(filename, source, current);
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"}")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readSwitchCaseStatement(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new SwitchCaseStatement;
    node->type = NodeType::STATEMENT_SWITCH_CASE;
    node->level = 0;
    if (token->location.startsWith(source, L"case")) {
      skipInvisible(filename, source, current);
      node->match = readExpression(filename, source, current);
      if (!node->match) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else if (!token->location.startsWith(source, L"default")) {
      return nullptr;
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L":")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto backup = current;
    skipInvisible(filename, source, current);
    auto token = readIdentifierToken(filename, source, current);
    if (token != nullptr && !token->location.startsWith(source, L"case") &&
        !token->location.startsWith(source, L"default")) {
      current = backup;
      skipNewLine(filename, source, current);
      auto statement = readStatement(filename, source, current);
      while (statement != nullptr) {
        node->statements.pushBack(statement);
        backup = current;
        skipInvisible(filename, source, current);
        auto token = readIdentifierToken(filename, source, current);
        if (token != nullptr &&
            (token->location.startsWith(source, L"case") ||
             token->location.startsWith(source, L"default"))) {
          current = backup;
          break;
        } else {
          current = backup;
        }
        skipNewLine(filename, source, current);
        statement = readStatement(filename, source, current);
        skipNewLine(filename, source, current);
      }
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readTryStatement(const std::string &filename,
                             const std::wstring &source, Position &position) {

  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"try")) {
    core::AutoPtr node = new TryStatement;
    node->type = NodeType::STATEMENT_TRY;
    node->try_ = readBlockStatement(filename, source, current);
    if (!node->try_) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->catch_ = readTryCatchStatement(filename, source, current);
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"finally")) {
      node->finally = readBlockStatement(filename, source, current);
      if (!node->finally) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else {
      current = backup;
    }
    if (!node->catch_ && !node->finally) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readTryCatchStatement(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"catch")) {
    core::AutoPtr node = new TryCatchStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_TRY_CATCH;
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    if (token->location.startsWith(source, L"(")) {
      node->binding = readIdentifierLiteral(filename, source, current);
      if (!node->binding) {
        node->binding = readObjectPattern(filename, source, current);
      }
      if (!node->binding) {
        node->binding = readArrayPattern(filename, source, current);
      }
      if (!node->binding) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L")")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else {
      current = backup;
    }
    node->statement = readBlockStatement(filename, source, current);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExpressionStatement(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  return readExpressions(filename, source, position);
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readValue(const std::string &filename, const std::wstring &source,
                      Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto node = readGroupExpression(filename, source, current);
  if (!node) {
    node = readBooleanLiteral(filename, source, current);
  }
  if (!node) {
    node = readStringLiteral(filename, source, current);
  }
  if (!node) {
    node = readBigIntLiteral(filename, source, current);
  }
  if (!node) {
    node = readNumberLiteral(filename, source, current);
  }
  if (!node) {
    node = readNullLiteral(filename, source, current);
  }
  if (!node) {
    node = readUndefinedLiteral(filename, source, current);
  }
  if (!node) {
    node = readRegexLiteral(filename, source, current);
  }
  if (!node) {
    node = readTemplateLiteral(filename, source, current);
  }
  if (!node) {
    node = readNewExpression(filename, source, current);
  }
  if (!node) {
    node = readFunctionDeclaration(filename, source, current);
  }
  if (!node) {
    node = readClassDeclaration(filename, source, current);
  }
  if (!node) {
    node = readIdentifierLiteral(filename, source, current);
  }
  if (!node) {
    node = readArrayDeclaration(filename, source, current);
  }
  if (!node) {
    node = readObjectDeclaration(filename, source, current);
  }
  if (node != nullptr) {
    for (;;) {
      auto expr = readMemberExpression(filename, source, current);
      if (!expr) {
        expr = readCallExpression(filename, source, current);
      }
      if (expr != nullptr) {
        expr.cast<BinaryExpression>()->left = node;
        node = expr;
        node->location = getLocation(source, position, current);
      } else {
        break;
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readRValue(const std::string &filename, const std::wstring &source,
                       Position &position, int level) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto node = readValue(filename, source, current);
  if (!node) {
    node = readUnaryExpression(filename, source, current);
  }
  if (!node) {
    node = readVoidExpression(filename, source, current);
  }
  if (!node) {
    node = readDeleteExpression(filename, source, current);
  }
  if (!node) {
    node = readTypeofExpression(filename, source, current);
  }
  if (!node) {
    node = readAwaitExpression(filename, source, current);
  }
  if (node != nullptr) {
    for (;;) {
      if (level < 3) {
        break;
      }
      auto expr = readUpdateExpression(filename, source, current);
      if (level >= 17 && expr == nullptr) {
        expr = readAssigmentExpression(filename, source, current);
      }
      if (level >= 9 && expr == nullptr) {
        expr = readInstanceOfExpression(filename, source, current);
      }
      if (level >= 9 && expr == nullptr) {
        expr = readInExpression(filename, source, current);
      }
      if (level >= 4 && expr == nullptr) {
        auto tmp = current;
        expr = readBinaryExpression(filename, source, current);
        if (expr != nullptr && expr->level > level) {
          current = tmp;
          expr = nullptr;
        }
      }
      if (level >= 16 && expr == nullptr) {
        expr = readConditionExpression(filename, source, current);
      }
      if (expr != nullptr) {
        expr.cast<BinaryExpression>()->left = node;
        node = expr;
        node->location = getLocation(source, position, current);
      } else {
        break;
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readDecorator(const std::string &filename,
                          const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"@")) {
    core::AutoPtr node = new Decorator;
    node->type = NodeType::DECORATOR;
    node->expression = readValue(filename, source, current);
    if (!node->expression) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExpression(const std::string &filename,
                           const std::wstring &source, Position &position) {
  return readRValue(filename, source, position, 999);
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExpressions(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto expr = readExpression(filename, source, current);
  if (!expr) {
    return nullptr;
  }
  auto next = current;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L",")) {
    auto right = readExpressions(filename, source, current);
    if (!right) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new BinaryExpression;
    node->opt = L",";
    node->level = 18;
    node->left = expr;
    node->right = right;
    node->type = NodeType::EXPRESSION_BINARY;
    node->location = getLocation(source, position, current);
    expr = node;
    next = current;
  }
  current = next;
  position = current;
  return expr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readBlockStatement(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipNewLine(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"{")) {
    core::AutoPtr node = new BlockStatement;
    node->level = 0;
    node->type = NodeType::STATEMENT_BLOCK;
    skipNewLine(filename, source, current);
    auto directive = readDirective(filename, source, current);
    while (directive != nullptr) {
      node->directives.pushBack(directive);
      directive = readDirective(filename, source, current);
    }
    skipNewLine(filename, source, current);
    auto statement = readStatement(filename, source, current);
    while (statement != nullptr) {
      node->body.pushBack(statement);
      statement = readStatement(filename, source, current);
      skipNewLine(filename, source, current);
    }
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"}")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, position));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
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
    auto backup = current;
    auto token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L";")) {
      current = backup;
    }
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
    auto src = source.substr(token->location.start.offset,
                             token->location.end.offset -
                                 token->location.start.offset + 1);
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
  if (tag != nullptr) {
    node->tag = source.substr(tag->location.start.offset,
                              tag->location.end.offset -
                                  tag->location.start.offset + 1);
  }
  if (temp != nullptr) {
    auto src = source.substr(temp->location.start.offset,
                             temp->location.end.offset -
                                 temp->location.start.offset + 1);
    node->quasis.pushBack(src.substr(1, src.length() - 2));
    node->location = getLocation(source, position, current);
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
JSCompiler::readBigIntLiteral(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readBigIntToken(filename, source, current);
  if (token != nullptr) {
    core::AutoPtr node = new BigIntLiteral;
    node->location = token->location;
    node->type = NodeType::LITERAL_BIGINT;
    node->level = -2;
    node->value = source.substr(token->location.start.offset,
                                token->location.end.offset -
                                    token->location.start.offset);
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
  node->value = token->location.startsWith(source, L"true");
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
JSCompiler::readPrivateName(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"#")) {
    token = readIdentifierToken(filename, source, current);
    if (!token) {
      return nullptr;
    }
    core::AutoPtr node = new PrivateName();
    node->value = token->location.getSource(source);
    node->location = getLocation(source, position, current);
    node->level = -2;
    node->type = NodeType::PRIVATE_NAME;
    position = current;
    return node;
  }
  return nullptr;
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
      {},
      {}};
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  for (size_t level = 0; level < operators.size(); level++) {
    for (auto &opt : operators[level]) {
      if (token->location.startsWith(source, opt)) {
        core::AutoPtr node = new BinaryExpression;
        node->location = token->location;
        node->level = level;
        node->opt = opt;
        node->type = NodeType::EXPRESSION_BINARY;
        node->right = readRValue(filename, source, current, level);
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readAssigmentExpression(const std::string &filename,
                                    const std::wstring &source,
                                    JSCompiler::Position &position) {
  const core::Array<std::wstring> operators = {
      {L"=", L"+=", L"-=", L"**=", L"*=", L"/=", L"%=", L">>>=", L"<<=", L">>=",
       L"&&=", L"||=", L"&=", L"^=", L"|=", LR"(??=)"}};
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  for (auto &opt : operators) {
    if (token->location.startsWith(source, opt)) {
      core::AutoPtr node = new BinaryExpression;
      node->level = 17;
      node->opt = opt;
      node->type = NodeType::EXPRESSION_ASSIGMENT;
      node->right = readRValue(filename, source, current, 17);
      node->location = getLocation(source, position, current);
      position = current;
      return node;
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
  if (token->location.startsWith(source, L"?")) {
    auto next = current;
    auto trusy = readExpression(filename, source, current);
    if (!trusy) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L":")) {
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

    node->location = getLocation(source, position, current);
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
    if (token->location.startsWith(source, L"++") ||
        token->location.startsWith(source, L"--")) {
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
      node->right = readRValue(filename, source, current, 4);
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
  if (token != nullptr && token->location.startsWith(source, L"(")) {
    auto expr = readExpressions(filename, source, current);
    if (!expr) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new GroupExpression;
    node->expression = expr;
    node->level = 0;
    node->type = NodeType::EXPRESSION_GROUP;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readMemberExpression(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }

  if (token->location.startsWith(source, L".")) {
    auto identifier = readIdentifierLiteral(filename, source, current);
    if (!identifier) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new MemberExpression;
    node->level = 1;
    node->type = NodeType::EXPRESSION_MEMBER;
    node->location = getLocation(source, position, current);
    node->right = identifier;
    position = current;
    return node;
  }

  if (token->location.startsWith(source, L"?.")) {
    auto next = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (token != nullptr) {
      if (token->location.startsWith(source, L"(")) {
        current = next;
        auto field = readCallExpression(filename, source, current)
                         .cast<CallExpression>();
        if (!field) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        core::AutoPtr node = new OptionalCallExpression;
        node->level = field->level;
        node->type = NodeType::EXPRESSION_OPTIONAL_CALL;
        node->arguments = field->arguments;
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      } else if (token->location.startsWith(source, L"[")) {
        current = next;
        auto field = readMemberExpression(filename, source, current)
                         .cast<ComputedMemberExpression>();
        if (!field) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        core::AutoPtr node = new OptionalComputedMemberExpression;
        node->level = 1;
        node->type = NodeType::EXPRESSION_OPTIONAL_COMPUTED_MEMBER;
        node->right = field->right;
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
    core::AutoPtr node = new OptionalMemberExpression;
    node->level = 1;
    node->type = NodeType::EXPRESSION_OPTIONAL_MEMBER;
    node->location = getLocation(source, position, current);
    auto identifier = readIdentifierLiteral(filename, source, current);
    if (!identifier) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->right = identifier;
    position = current;
    return node;
  }
  if (token->location.startsWith(source, L"[")) {
    auto field = readExpressions(filename, source, current);
    if (!field) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"]")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new ComputedMemberExpression;
    node->level = 1;
    node->type = NodeType::EXPRESSION_COMPUTED_MEMBER;
    node->right = field;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readCallExpression(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"(")) {
    core::AutoPtr node = new CallExpression;
    node->type = NodeType::EXPRESSION_CALL;
    node->level = 1;
    auto arg = readRestExpression(filename, source, current);
    if (!arg) {
      arg = readExpression(filename, source, current);
    }
    while (arg != nullptr) {
      node->arguments.pushBack(arg);
      auto next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      if (token->location.startsWith(source, L")")) {
        current = next;
        break;
      } else if (token->location.startsWith(source, L",")) {
        arg = readRestExpression(filename, source, current);
        if (!arg) {
          arg = readExpression(filename, source, current);
        }
      } else {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    if (!token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readRestExpression(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  if (token->location.startsWith(source, L"...")) {
    core::AutoPtr node = new BinaryExpression;
    node->level = 19;
    node->opt = L"...";
    node->type = NodeType::EXPRESSION_BINARY;
    node->right = readRValue(filename, source, current, 999);
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readAwaitExpression(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"await")) {
    core::AutoPtr node = new AwaitExpression;
    node->type = NodeType::EXPRESSION_AWAIT;
    node->level = 4;
    node->location = getLocation(source, position, current);
    node->right = readRValue(filename, source, current, 4);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readTypeofExpression(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"typeof")) {
    core::AutoPtr node = new TypeofExpression;
    node->type = NodeType::EXPRESSION_TYPEOF;
    node->level = 4;
    node->location = getLocation(source, position, current);
    node->right = readRValue(filename, source, current, 4);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readVoidExpression(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"void")) {
    core::AutoPtr node = new VoidExpression;
    node->type = NodeType::EXPRESSION_VOID;
    node->level = 4;
    node->location = getLocation(source, position, current);
    node->right = readRValue(filename, source, current, 4);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readNewExpression(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"new")) {
    core::AutoPtr node = new NewExpression;
    node->type = NodeType::EXPRESSION_NEW;
    node->level = 2;
    node->location = getLocation(source, position, current);
    node->right = readRValue(filename, source, current, 2);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readDeleteExpression(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"delete")) {
    core::AutoPtr node = new DeleteExpression;
    node->type = NodeType::EXPRESSION_DELETE;
    node->level = 4;
    node->location = getLocation(source, position, current);
    node->right = readRValue(filename, source, current, 4);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readInExpression(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"in")) {
    core::AutoPtr node = new BinaryExpression;
    node->location = token->location;
    node->level = 9;
    node->opt = L"in";
    node->type = NodeType::EXPRESSION_BINARY;
    node->right = readRValue(filename, source, current, 9);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readInstanceOfExpression(const std::string &filename,
                                     const std::wstring &source,
                                     Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"instanceof")) {
    core::AutoPtr node = new BinaryExpression;
    node->location = token->location;
    node->level = 9;
    node->opt = L"instanceof";
    node->type = NodeType::EXPRESSION_BINARY;
    node->right = readRValue(filename, source, current, 9);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readParameter(const std::string &filename,
                          const std::wstring &source, Position &position) {
  core::AutoPtr rest = readRestPattern(filename, source, position);
  if (rest != nullptr) {
    return rest;
  }
  auto current = position;
  skipInvisible(filename, source, current);
  auto node = new Parameter;
  node->level = 0;
  node->type = NodeType::DECLARATION_PARAMETER;
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (!identifier) {
    identifier = readObjectPattern(filename, source, current);
  }
  if (!identifier) {
    identifier = readArrayPattern(filename, source, current);
  }
  if (identifier != nullptr) {
    node->identifier = identifier;
  } else {
    return nullptr;
  }
  auto next = current;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    return nullptr;
  }
  if (token->location.startsWith(source, L"=")) {
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
  if (token->location.startsWith(source, L",") ||
      token->location.startsWith(source, L")")) {
    current = next;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readArrowFunctionDeclaration(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto async = readIdentifierToken(filename, source, current);
  core::AutoPtr node = new ArrowFunctionDeclaration();
  if (async == nullptr || !async->location.startsWith(source, L"async")) {
    async = nullptr;
    current = position;
    node->async = false;
  } else {
    node->async = true;
  }
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"(")) {
    auto param = readParameter(filename, source, current);
    while (param != nullptr) {
      node->arguments.pushBack(param);
      skipInvisible(filename, source, current);
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (!token) {
        return nullptr;
      }
      if (!token->location.startsWith(source, L",")) {
        if (token->location.startsWith(source, L")")) {
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
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      return nullptr;
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"=>")) {
      return nullptr;
    }
    auto next = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (token != nullptr) {
      current = next;
      if (token->location.startsWith(source, L"{")) {
        node->body = readBlockStatement(filename, source, current);
      } else {
        node->body = readExpression(filename, source, current);
      }
    } else {
      node->body = readExpression(filename, source, current);
    }
    node->level = -2;
    node->type = NodeType::DECLARATION_ARROW_FUNCTION;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  auto param = readIdentifierLiteral(filename, source, current);
  if (param != nullptr) {
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"=>")) {
      node->arguments.pushBack(param);
      auto next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (token != nullptr) {
        current = next;
        if (token->location.startsWith(source, L"{")) {
          node->body = readBlockStatement(filename, source, current);
        } else {
          node->body = readExpression(filename, source, current);
        }
      } else {
        node->body = readExpression(filename, source, current);
      }
      node->level = -2;
      node->type = NodeType::DECLARATION_ARROW_FUNCTION;
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readFunctionDeclaration(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto next = current;
  auto async = readIdentifierToken(filename, source, current);
  if (async == nullptr || !async->location.startsWith(source, L"async")) {
    async = nullptr;
    current = next;
  }
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"function")) {
    skipInvisible(filename, source, current);
    auto next = current;
    auto generator = readSymbolToken(filename, source, current);
    if (generator == nullptr || !generator->location.startsWith(source, L"*")) {
      generator = nullptr;
      current = next;
    }
    core::AutoPtr node = new FunctionDeclaration;
    node->level = -2;
    node->type = NodeType::DECLARATION_FUNCTION;
    if (async != nullptr) {
      node->async = true;
    } else {
      node->async = false;
    }
    if (generator != nullptr) {
      node->generator = true;
    } else {
      node->generator = false;
    }
    node->identifier = readIdentifierLiteral(filename, source, current);
    skipInvisible(filename, source, current);
    auto token = readSymbolToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"(")) {
      auto param = readParameter(filename, source, current);
      while (param != nullptr) {
        node->arguments.pushBack(param);
        skipInvisible(filename, source, current);
        auto next = current;
        token = readSymbolToken(filename, source, current);
        if (!token) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        if (!token->location.startsWith(source, L",")) {
          if (token->location.startsWith(source, L")")) {
            current = next;
            break;
          } else {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
        }
        param = readParameter(filename, source, current);
        if (!param) {
          break;
        }
      }
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L")")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      skipInvisible(filename, source, current);
      node->body = readBlockStatement(filename, source, current);
      if (!node->body) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readArrayDeclaration(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"[")) {
    core::AutoPtr node = new ArrayDeclaration;
    node->level = -2;
    node->type = NodeType::DECLARATION_ARRAY;
    auto item = readRestExpression(filename, source, current);
    if (item == nullptr) {
      item = readExpression(filename, source, current);
    }
    for (;;) {
      auto next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      if (token->location.startsWith(source, L"]")) {
        if (item != nullptr) {
          node->items.pushBack(item);
        }
        current = next;
        break;
      }
      if (!token->location.startsWith(source, L",")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->items.pushBack(item);
      item = readRestExpression(filename, source, current);
      if (item == nullptr) {
        item = readExpression(filename, source, current);
      }
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    if (!token->location.startsWith(source, L"]")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}
core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectDeclaration(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"{")) {
    core::AutoPtr node = new ObjectDeclaration;
    node->level = -2;
    node->type = NodeType::DECLARATION_OBJECT;
    auto item = readObjectProperty(filename, source, current);
    for (;;) {
      auto next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      if (token->location.startsWith(source, L"}")) {
        if (item != nullptr) {
          node->properties.pushBack(item);
        }
        current = next;
        break;
      }
      if (!token->location.startsWith(source, L",") || !item) {
        current = next;
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->properties.pushBack(item);
      item = readObjectProperty(filename, source, current);
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    if (!token->location.startsWith(source, L"}")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectProperty(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto rest = readRestExpression(filename, source, position);
  if (rest != nullptr) {
    return rest;
  }
  auto method = readObjectMethod(filename, source, position);
  if (method != nullptr) {
    return method;
  }
  method = readObjectAccessor(filename, source, position);
  if (method != nullptr) {
    return method;
  }
  auto current = position;
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (!identifier) {
    identifier = readStringLiteral(filename, source, current);
  }
  if (!identifier) {
    identifier = readMemberExpression(filename, source, current);
    if (identifier != nullptr &&
        identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
      identifier = identifier.cast<ComputedMemberExpression>()->right;
    } else {
      current = position;
      identifier = nullptr;
    }
  }
  if (!identifier) {
    return nullptr;
  }
  core::AutoPtr node = new ObjectProperty;
  node->identifier = identifier;
  auto next = current;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L":")) {
    auto impl = readExpression(filename, source, current);
    if (!impl) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->implement = impl;
    next = current;
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
  }
  if (token != nullptr) {
    if (token->location.startsWith(source, L",") ||
        token->location.startsWith(source, L"}")) {
      current = next;
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }
  throw std::runtime_error(
      formatException("Unexcepted token", filename, source, current));
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectMethod(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  auto next = current;

  skipInvisible(filename, source, current);
  auto async = readIdentifierLiteral(filename, source, current);
  if (async != nullptr && async->location.startsWith(source, L"async")) {
    next = current;
  } else {
    current = next;
    async = nullptr;
  }

  skipInvisible(filename, source, current);
  auto generator = readSymbolToken(filename, source, current);
  if (!generator || !generator->location.startsWith(source, L"*")) {
    current = next;
    generator = nullptr;
  } else {
    next = current;
  }

  auto identifier = readIdentifierLiteral(filename, source, current);
  if (!identifier) {
    identifier = readStringLiteral(filename, source, current);
  }

  if (!identifier) {
    identifier = readMemberExpression(filename, source, current);
    if (identifier != nullptr &&
        identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
      identifier = identifier.cast<ComputedMemberExpression>()->right;
    } else {
      current = position;
      identifier = nullptr;
    }
  }
  if (!identifier) {
    if (!generator) {
      identifier = async;
      async = nullptr;
    }
    if (!identifier) {
      return nullptr;
    }
  }
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"(")) {
    core::AutoPtr node = new ObjectMethod;
    node->async = async != nullptr;
    node->generator = generator != nullptr;
    node->identifier = identifier;
    node->level = 0;
    node->type = NodeType::OBJECT_METHOD;
    auto param = readParameter(filename, source, current);
    while (param != nullptr) {
      node->arguments.pushBack(param);
      skipInvisible(filename, source, current);
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (!token) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      if (!token->location.startsWith(source, L",")) {
        if (token->location.startsWith(source, L")")) {
          current = next;
          break;
        } else {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
      }
      param = readParameter(filename, source, current);
      if (!param) {
        break;
      }
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    node->body = readBlockStatement(filename, source, current);
    if (!node->body) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectAccessor(const std::string &filename,
                               const std::wstring &source, Position &position) {
  auto current = position;

  skipInvisible(filename, source, current);
  auto accessor = readIdentifierLiteral(filename, source, current);
  if (accessor == nullptr || (!accessor->location.startsWith(source, L"get") &&
                              !accessor->location.startsWith(source, L"set"))) {
    return nullptr;
  }

  auto identifier = readIdentifierLiteral(filename, source, current);
  if (!identifier) {
    identifier = readStringLiteral(filename, source, current);
  }

  if (!identifier) {
    identifier = readMemberExpression(filename, source, current);
    if (identifier != nullptr &&
        identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
      identifier = identifier.cast<ComputedMemberExpression>()->right;
    } else {
      current = position;
      identifier = nullptr;
    }
  }
  if (!identifier) {
    return nullptr;
  }
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"(")) {
    core::AutoPtr node = new ObjectAccessor;
    node->kind = accessor->location.startsWith(source, L"get")
                     ? AccessorKind::GET
                     : AccessorKind::SET;
    node->identifier = identifier;
    node->level = 0;
    node->type = NodeType::OBJECT_ACCESSOR;
    auto param = readParameter(filename, source, current);
    while (param != nullptr) {
      node->arguments.pushBack(param);
      skipInvisible(filename, source, current);
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (!token) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      if (!token->location.startsWith(source, L",")) {
        if (token->location.startsWith(source, L")")) {
          current = next;
          break;
        } else {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
      }
      param = readParameter(filename, source, current);
      if (!param) {
        break;
      }
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L")")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    node->body = readBlockStatement(filename, source, current);
    if (!node->body) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readClassDeclaration(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto current = position;
  core::AutoPtr node = new ClassDeclaration;
  node->level = -2;
  node->type = NodeType::DECLARATION_CLASS;
  auto decorator = readDecorator(filename, source, current);
  while (decorator != nullptr) {
    node->decorators.pushBack(decorator);
    decorator = readDecorator(filename, source, current);
  }
  auto backup = current;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"export")) {
    current = backup;
    auto exportNode = readExportDeclaration(filename, source, current)
                          .cast<ExportDeclaration>();
    if (!exportNode || exportNode->items.size() == 0 ||
        exportNode->items[0]->type != NodeType::DECLARATION_CLASS) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto clazz = exportNode->items[0].cast<ClassDeclaration>();
    clazz->decorators = node->decorators;
    exportNode->location = getLocation(source, position, current);
    position = current;
    return exportNode;
  }
  if (token != nullptr && token->location.startsWith(source, L"class")) {
    node->identifier = readIdentifierLiteral(filename, source, current);
    auto next = current;
    skipInvisible(filename, source, current);
    auto token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"extends")) {
      next = current;
      skipInvisible(filename, source, current);
      node->extends = readValue(filename, source, current);
      if (!node->extends) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      next = current;
    } else {
      current = next;
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"{")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipNewLine(filename, source, current);
    auto item = readClassProperty(filename, source, current);
    while (item != nullptr) {
      skipNewLine(filename, source, current);
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (token != nullptr && token->location.startsWith(source, L"}")) {
        if (item != nullptr) {
          node->properties.pushBack(item);
        }
        current = next;
        break;
      } else {
        current = next;
      }
      node->properties.pushBack(item);
      item = readClassProperty(filename, source, current);
    }
    skipNewLine(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    if (!token->location.startsWith(source, L"}")) {
      skipInvisible(filename, source, current);
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readStaticBlock(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipNewLine(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"static")) {
    auto statement = readBlockStatement(filename, source, current);
    if (statement != nullptr) {
      statement->type = NodeType::STATIC_BLOCK;
      statement->location = getLocation(source, position, current);
      position = current;
      return statement;
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readClassMethod(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  core::AutoPtr node = new ClassMethod;
  node->type = NodeType::CLASS_METHOD;
  node->level = 0;
  auto decorator = readDecorator(filename, source, current);
  while (decorator != nullptr) {
    node->decorators.pushBack(decorator);
    decorator = readDecorator(filename, source, current);
  }
  auto backup = current;
  skipInvisible(filename, source, current);
  auto static_ = readIdentifierLiteral(filename, source, current);
  if (static_ != nullptr) {
    if (!static_->location.startsWith(source, L"static")) {
      static_ = nullptr;
      current = backup;
    }
  }
  backup = current;
  skipInvisible(filename, source, current);
  auto async = readIdentifierLiteral(filename, source, current);
  if (async != nullptr) {
    if (!async->location.startsWith(source, L"async")) {
      async = nullptr;
      current = backup;
    }
  }
  backup = current;
  skipInvisible(filename, source, current);
  auto generator = readSymbolToken(filename, source, current);
  if (generator != nullptr) {
    if (!generator->location.startsWith(source, L"*")) {
      generator = nullptr;
      current = backup;
    }
  }
  backup = current;
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (!identifier) {
    identifier = readPrivateName(filename, source, current);
  }
  if (!identifier) {
    identifier = readStringLiteral(filename, source, current);
  }
  if (!identifier) {
    backup = current;
    identifier = readMemberExpression(filename, source, current);
    if (identifier != nullptr &&
        identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
      identifier = identifier.cast<ComputedMemberExpression>()->right;
    } else {
      identifier = nullptr;
      current = backup;
    }
  }
  if (!identifier) {
    if (!generator) {
      identifier = async;
      async = nullptr;
      if (!identifier) {
        identifier = static_;
        static_ = nullptr;
      }
    }
  }
  if (identifier != nullptr) {

    skipInvisible(filename, source, current);
    auto token = readSymbolToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"(")) {
      node->static_ = static_ != nullptr;
      node->async = async != nullptr;
      node->generator = generator != nullptr;
      node->identifier = identifier;
      auto param = readParameter(filename, source, current);
      while (param != nullptr) {
        node->arguments.pushBack(param);
        skipInvisible(filename, source, current);
        auto next = current;
        token = readSymbolToken(filename, source, current);
        if (!token) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        if (!token->location.startsWith(source, L",")) {
          if (token->location.startsWith(source, L")")) {
            current = next;
            break;
          } else {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
        }
        param = readParameter(filename, source, current);
        if (!param) {
          break;
        }
      }
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L")")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      skipInvisible(filename, source, current);
      node->body = readBlockStatement(filename, source, current);
      if (!node->body) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
  }

  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readClassAccessor(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  core::AutoPtr node = new ClassAccessor;
  node->type = NodeType::CLASS_ACCESSOR;
  node->level = 0;
  auto decorator = readDecorator(filename, source, current);
  while (decorator != nullptr) {
    node->decorators.pushBack(decorator);
    decorator = readDecorator(filename, source, current);
  }
  skipInvisible(filename, source, current);
  auto static_ = readIdentifierLiteral(filename, source, current);
  if (static_ != nullptr) {
    if (!static_->location.startsWith(source, L"static")) {
      static_ = nullptr;
      current = position;
    }
  }
  skipInvisible(filename, source, current);
  auto accessor = readIdentifierToken(filename, source, current);
  if (accessor != nullptr && (accessor->location.startsWith(source, L"get") ||
                              accessor->location.startsWith(source, L"set"))) {
    auto identifier = readIdentifierLiteral(filename, source, current);
    if (!identifier) {
      identifier = readPrivateName(filename, source, current);
    }
    if (!identifier) {
      identifier = readStringLiteral(filename, source, current);
    }
    if (!identifier) {
      auto backup = current;
      identifier = readMemberExpression(filename, source, current);
      if (identifier != nullptr &&
          identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
        identifier = identifier.cast<ComputedMemberExpression>()->right;
      } else {
        identifier = nullptr;
        current = backup;
      }
    }
    if (identifier != nullptr) {
      skipInvisible(filename, source, current);
      auto token = readSymbolToken(filename, source, current);
      if (token != nullptr && token->location.startsWith(source, L"(")) {
        node->static_ = static_ != nullptr;
        node->kind = accessor->location.startsWith(source, L"get")
                         ? AccessorKind::GET
                         : AccessorKind::SET;
        node->identifier = identifier;
        auto param = readParameter(filename, source, current);
        while (param != nullptr) {
          node->arguments.pushBack(param);
          skipInvisible(filename, source, current);
          auto next = current;
          token = readSymbolToken(filename, source, current);
          if (!token) {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
          if (!token->location.startsWith(source, L",")) {
            if (token->location.startsWith(source, L")")) {
              current = next;
              break;
            } else {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }
          }
          param = readParameter(filename, source, current);
          if (!param) {
            break;
          }
        }
        skipInvisible(filename, source, current);
        token = readSymbolToken(filename, source, current);
        if (!token || !token->location.startsWith(source, L")")) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        skipInvisible(filename, source, current);
        node->body = readBlockStatement(filename, source, current);
        if (!node->body) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->location = getLocation(source, position, current);
        position = current;
        return node;
      }
    }
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readClassProperty(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto rest = readRestExpression(filename, source, position);
  if (rest != nullptr) {
    return rest;
  }
  auto sblock = readStaticBlock(filename, source, position);
  if (sblock != nullptr) {
    return sblock;
  }
  auto method = readClassMethod(filename, source, position);
  if (method != nullptr) {
    return method;
  }
  auto accessor = readClassAccessor(filename, source, position);
  if (accessor != nullptr) {
    return accessor;
  }
  auto current = position;
  core::AutoPtr node = new ClassProperty;
  node->type = NodeType::CLASS_PROPERTY;
  node->level = 0;
  auto decorator = readDecorator(filename, source, current);
  while (decorator != nullptr) {
    node->decorators.pushBack(decorator);
    decorator = readDecorator(filename, source, current);
  }
  auto backup = current;
  skipNewLine(filename, source, current);
  auto staticFlag = readIdentifierLiteral(filename, source, current);
  if (staticFlag != nullptr) {
    if (!staticFlag->location.startsWith(source, L"static")) {
      current = backup;
      staticFlag = nullptr;
    }
  }
  node->identifier = readIdentifierLiteral(filename, source, current);
  node->static_ = staticFlag != nullptr;
  if (!node->identifier) {
    node->identifier = readPrivateName(filename, source, current);
  }
  if (!node->identifier) {
    node->identifier = readStringLiteral(filename, source, current);
  }
  if (!node->identifier) {
    auto backup = current;
    node->identifier = readMemberExpression(filename, source, current);
    if (node->identifier != nullptr &&
        node->identifier->type == NodeType::EXPRESSION_COMPUTED_MEMBER) {
      node->identifier =
          node->identifier.cast<ComputedMemberExpression>()->right;
    } else {
      node->identifier = nullptr;
      current = backup;
    }
  }
  if (!node->identifier && staticFlag != nullptr) {
    node->identifier = staticFlag;
    staticFlag = nullptr;
  }
  if (!node->identifier) {
    return nullptr;
  }
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"=")) {
    node->value = readExpression(filename, source, current);
    if (!node->value) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readVariableDeclarator(const std::string &filename,
                                   const std::wstring &source,
                                   Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (identifier == nullptr) {
    identifier = readArrayPattern(filename, source, current);
  }
  if (identifier == nullptr) {
    identifier = readObjectPattern(filename, source, current);
  }
  if (!identifier) {
    return nullptr;
  }
  core::AutoPtr node = new VariableDeclarator;
  node->identifier = identifier;
  node->type = NodeType::VARIABLE_DECLARATOR;
  node->level = 0;
  auto next = current;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (!token) {
    token = readIdentifierToken(filename, source, current);
  }
  if (token != nullptr && token->location.startsWith(source, L"=")) {
    auto value = readRValue(filename, source, current, 999);
    if (!value) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->value = value;
  } else {
    current = next;
  }
  node->location = getLocation(source, position, current);
  position = current;
  return node;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readVariableDeclaration(const std::string &filename,
                                    const std::wstring &source,
                                    Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto kind = readIdentifierToken(filename, source, current);
  if (kind != nullptr) {
    core::AutoPtr node = new VariableDeclaration;
    node->type = NodeType::VARIABLE_DECLARATION;
    node->level = 0;
    if (kind->location.startsWith(source, L"var")) {
      node->kind = DeclarationKind::VAR;
    } else if (kind->location.startsWith(source, L"let")) {
      node->kind = DeclarationKind::LET;
    } else if (kind->location.startsWith(source, L"const")) {
      node->kind = DeclarationKind::CONST;
    } else {
      return nullptr;
    }
    auto declarator = readVariableDeclarator(filename, source, current);
    if (!declarator) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    while (declarator != nullptr) {
      node->declarations.pushBack(declarator);
      auto next = current;
      skipInvisible(filename, source, current);
      auto token = readSymbolToken(filename, source, current);
      if (token == nullptr) {
        current = next;
        break;
      }
      if (!token->location.startsWith(source, L",")) {
        current = next;
        break;
      }
      declarator = readVariableDeclarator(filename, source, current);
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readRestPattern(const std::string &filename,
                            const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"...")) {
    auto identifier = readObjectPattern(filename, source, current);
    if (!identifier) {
      identifier = readArrayPattern(filename, source, current);
    }
    if (!identifier) {
      identifier = readIdentifierLiteral(filename, source, current);
    }
    if (!identifier) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    auto node = new RestPatternItem;
    node->type = NodeType::PATTERN_REST_ITEM;
    node->level = 0;
    node->identifier = identifier;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectPatternItem(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto rest = readRestPattern(filename, source, position);
  if (rest != nullptr) {
    return rest;
  }
  auto current = position;
  skipInvisible(filename, source, current);
  auto next = current;
  auto identifier = readIdentifierLiteral(filename, source, current);
  core::AutoPtr node = new ObjectPatternItem;
  node->type = NodeType::PATTERN_OBJECT_ITEM;
  node->level = 0;
  if (!identifier) {
    identifier = readStringLiteral(filename, source, current);
  }
  if (!identifier) {
    identifier = readMemberExpression(filename, source, current);
    if (!identifier ||
        identifier->type != NodeType::EXPRESSION_COMPUTED_MEMBER) {
      identifier = nullptr;
      current = next;
    } else {
      identifier = identifier.cast<ComputedMemberExpression>()->right;
    }
  }
  if (!identifier) {
    throw std::runtime_error(
        formatException("Unexcepted token", filename, source, current));
  }
  node->identifier = identifier;
  next = current;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr) {
    if (token->location.startsWith(source, L":")) {
      auto match = readIdentifierLiteral(filename, source, current);
      if (!match) {
        match = readObjectPattern(filename, source, current);
      }
      if (!match) {
        match = readArrayPattern(filename, source, current);
      }
      if (!match) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->match = match;
      next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
    }
  }
  if (token != nullptr) {
    if (token->location.startsWith(source, L"=")) {
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
  }
  if (token != nullptr) {
    if (token->location.startsWith(source, L",") ||
        token->location.startsWith(source, L"}")) {
      current = next;
      node->location = getLocation(source, position, current);
      position = current;
      return node;
    }
    current = next;
  }
  throw std::runtime_error(
      formatException("Unexcepted token", filename, source, current));
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readObjectPattern(const std::string &filename,
                              const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"{")) {
    core::AutoPtr node = new ObjectPattern;
    node->level = -2;
    node->type = NodeType::PATTERN_OBJECT;
    auto item = readObjectPatternItem(filename, source, current);
    while (item != nullptr) {
      node->items.pushBack(item);
      auto next = current;
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token) {
        return nullptr;
      }
      if (token->location.startsWith(source, L"}")) {
        current = next;
        break;
      }
      if (!token->location.startsWith(source, L",")) {
        return nullptr;
      }
      item = readObjectPatternItem(filename, source, current);
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      return nullptr;
    }
    if (!token->location.startsWith(source, L"}")) {
      return nullptr;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readArrayPatternItem(const std::string &filename,
                                 const std::wstring &source,
                                 Position &position) {
  auto rest = readRestPattern(filename, source, position);
  if (rest != nullptr) {
    return rest;
  }
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readObjectPattern(filename, source, current);
  if (!identifier) {
    identifier = readArrayPattern(filename, source, current);
  }
  if (!identifier) {
    identifier = readIdentifierLiteral(filename, source, current);
  }
  if (identifier != nullptr) {
    core::AutoPtr node = new ArrayPatternItem;
    node->identifier = identifier;
    node->type = NodeType::PATTERN_ARRAY_ITEM;
    node->level = 0;
    auto next = current;
    skipInvisible(filename, source, current);
    auto token = readSymbolToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"=")) {
      auto value = readExpression(filename, source, current);
      if (value == nullptr) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->value = value;
      next = current;
    }
    current = next;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readArrayPattern(const std::string &filename,
                             const std::wstring &source, Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"[")) {
    core::AutoPtr node = new ArrayPattern;
    node->level = -2;
    node->type = NodeType::PATTERN_ARRAY;
    auto item = readArrayPatternItem(filename, source, current);
    for (;;) {
      skipInvisible(filename, source, current);
      if (source[current.offset] == '\0') {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      auto next = current;
      token = readSymbolToken(filename, source, current);
      if (!token) {
        return nullptr;
      }
      if (token->location.startsWith(source, L"]")) {
        if (item != nullptr) {
          node->items.pushBack(item);
        }
        current = next;
        break;
      }
      if (!token->location.startsWith(source, L",")) {
        return nullptr;
      }
      node->items.pushBack(item);
      item = readArrayPatternItem(filename, source, current);
    }
    skipInvisible(filename, source, current);
    token = readSymbolToken(filename, source, current);
    if (!token) {
      return nullptr;
    }
    if (!token->location.startsWith(source, L"]")) {
      return nullptr;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readComment(const std::string &filename, const std::wstring &source,
                        Position &position) {
  auto current = position;
  while (skipWhiteSpace(filename, source, current) ||
         skipLineTerminatior(filename, source, current)) {
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

core::AutoPtr<JSCompiler::Node>
JSCompiler::readImportSpecifier(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (identifier == nullptr) {
    identifier = readStringLiteral(filename, source, current);
  }
  if (identifier != nullptr) {
    core::AutoPtr node = new ImportSpecifier;
    node->level = 0;
    node->type = NodeType::IMPORT_SPECIFIER;
    node->identifier = identifier;
    auto backup = current;
    skipInvisible(filename, source, current);
    auto token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"as")) {
      skipInvisible(filename, source, current);
      node->alias = readIdentifierLiteral(filename, source, current);
      if (!node->alias) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else if (identifier->type == NodeType::LITERAL_STRING) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readImportDefaultSpecifier(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (identifier != nullptr) {
    core::AutoPtr node = new ImportDefaultSpecifier;
    node->level = 0;
    node->type = NodeType::IMPORT_DEFAULT;
    node->identifier = identifier;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readImportNamespaceSpecifier(const std::string &filename,
                                         const std::wstring &source,
                                         Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"*")) {
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L"as")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    skipInvisible(filename, source, current);
    auto identifier = readIdentifierLiteral(filename, source, current);
    if (!identifier) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    core::AutoPtr node = new ImportNamespaceSpecifier;
    node->level = 0;
    node->type = NodeType::IMPORT_NAMESPACE;
    node->alias = identifier;
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}
core::AutoPtr<JSCompiler::Node>
JSCompiler::readImportAttriabue(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto key = readIdentifierLiteral(filename, source, current);
  if (!key) {
    key = readStringLiteral(filename, source, current);
  }
  if (key != nullptr) {
    core::AutoPtr node = new ImportAttribute;
    node->type = NodeType::IMPORT_ATTARTUBE;
    node->key = key;
    skipInvisible(filename, source, current);
    auto token = readSymbolToken(filename, source, current);
    if (!token || !token->location.startsWith(source, L":")) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->value = readStringLiteral(filename, source, current);
    if (!node->value) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readImportDeclaration(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"import")) {
    core::AutoPtr node = new ImportDeclaration;
    node->level = 0;
    node->type = NodeType::IMPORT_DECLARATION;
    skipInvisible(filename, source, current);
    auto src = readStringLiteral(filename, source, current);
    if (src != nullptr) {
      node->source = src;
    } else {
      auto specifier = readImportNamespaceSpecifier(filename, source, current);
      if (specifier != nullptr) {
        node->items.pushBack(specifier);
      } else {
        specifier = readImportDefaultSpecifier(filename, source, current);
        if (specifier != nullptr) {
          node->items.pushBack(specifier);
          skipInvisible(filename, source, current);
          token = readSymbolToken(filename, source, current);
          if (token != nullptr && !token->location.startsWith(source, L",")) {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
        }
        auto backup = current;
        skipInvisible(filename, source, current);
        token = readSymbolToken(filename, source, current);
        if (token != nullptr && token->location.startsWith(source, L"{")) {
          auto specifier = readImportSpecifier(filename, source, current);
          for (;;) {
            skipInvisible(filename, source, current);
            auto token = readSymbolToken(filename, source, current);
            if (!token) {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }
            if (token->location.startsWith(source, L",")) {
              if (!specifier) {
                throw std::runtime_error(formatException(
                    "Unexcepted token", filename, source, current));
              }
              node->items.pushBack(specifier);
            } else if (token->location.startsWith(source, L"}")) {
              if (specifier != nullptr) {
                node->items.pushBack(specifier);
              }
              break;
            } else {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }

            specifier = readImportSpecifier(filename, source, current);
          }
        } else {
          current = backup;
        }
        specifier = readImportNamespaceSpecifier(filename, source, current);
        if (specifier != nullptr) {
          node->items.pushBack(specifier);
        }
      }
      skipInvisible(filename, source, current);
      token = readIdentifierToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L"from")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      src = readStringLiteral(filename, source, current);
      if (!src) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->source = src;
    }
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr && (token->location.startsWith(source, L"assert") ||
                             token->location.startsWith(source, L"with"))) {
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L"{")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      auto attribute = readImportAttriabue(filename, source, current);
      while (attribute != nullptr) {
        node->attributes.pushBack(attribute);
        auto backup = current;
        skipInvisible(filename, source, current);
        token = readSymbolToken(filename, source, current);
        if (token == nullptr) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        if (token->location.startsWith(source, L"}")) {
          current = backup;
          break;
        }
        if (!token->location.startsWith(source, L",")) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        attribute = readImportAttriabue(filename, source, current);
      }
      skipInvisible(filename, source, current);
      token = readSymbolToken(filename, source, current);
      if (!token || !token->location.startsWith(source, L"}")) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExportSpecifier(const std::string &filename,
                                const std::wstring &source,
                                Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto identifier = readIdentifierLiteral(filename, source, current);
  if (identifier != nullptr) {
    core::AutoPtr node = new ExportSpecifier;
    node->level = 0;
    node->type = NodeType::EXPORT_SPECIFIER;
    node->identifier = identifier;
    auto backup = current;
    skipInvisible(filename, source, current);
    auto token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"as")) {
      auto alias = readIdentifierLiteral(filename, source, current);
      if (!alias) {
        alias = readStringLiteral(filename, source, current);
      }
      if (!alias) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      node->alias = alias;
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExportDefaultSpecifier(const std::string &filename,
                                       const std::wstring &source,
                                       Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"default")) {
    core::AutoPtr node = new ExportDefaultSpecifier;
    node->level = 0;
    node->type = NodeType::EXPORT_DEFAULT;
    node->value = readExpression(filename, source, current);
    if (!node->value) {
      throw std::runtime_error(
          formatException("Unexcepted token", filename, source, current));
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExportAllSpecifier(const std::string &filename,
                                   const std::wstring &source,
                                   Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readSymbolToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"*")) {
    core::AutoPtr node = new ExportAllSpecifier;
    node->level = 0;
    node->type = NodeType::EXPORT_ALL;
    auto backup = current;
    skipInvisible(filename, source, current);
    token = readIdentifierToken(filename, source, current);
    if (token != nullptr && token->location.startsWith(source, L"as")) {
      node->alias = readIdentifierLiteral(filename, source, current);
      if (!node->alias) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
    } else {
      current = backup;
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}

core::AutoPtr<JSCompiler::Node>
JSCompiler::readExportDeclaration(const std::string &filename,
                                  const std::wstring &source,
                                  Position &position) {
  auto current = position;
  skipInvisible(filename, source, current);
  auto token = readIdentifierToken(filename, source, current);
  if (token != nullptr && token->location.startsWith(source, L"export")) {
    core::AutoPtr node = new ExportDeclaration;
    node->level = 0;
    node->type = NodeType::EXPORT_DECLARATION;
    auto specifier = readExportDefaultSpecifier(filename, source, current);
    if (!specifier) {
      auto declaration = readClassDeclaration(filename, source, current)
                             .cast<ClassDeclaration>();
      if (declaration != nullptr && declaration->identifier == nullptr) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      specifier = declaration;
    }
    if (!specifier) {
      auto declaration = readFunctionDeclaration(filename, source, current)
                             .cast<FunctionDeclaration>();
      if (declaration != nullptr && declaration->identifier == nullptr) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      }
      specifier = declaration;
    }
    if (!specifier) {
      specifier = readVariableDeclaration(filename, source, current);
    }
    if (specifier != nullptr) {
      node->items.pushBack(specifier);
    } else {
      specifier = readExportAllSpecifier(filename, source, current);
      if (specifier != nullptr) {
        node->items.pushBack(specifier);
      } else {
        skipInvisible(filename, source, current);
        auto token = readSymbolToken(filename, source, current);
        if (!token || !token->location.startsWith(source, L"{")) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        specifier = readExportSpecifier(filename, source, current);
        for (;;) {
          skipInvisible(filename, source, current);
          auto token = readSymbolToken(filename, source, current);
          if (!token) {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
          if (token->location.startsWith(source, L",")) {
            if (!specifier) {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }
            node->items.pushBack(specifier);
          } else if (token->location.startsWith(source, L"}")) {
            if (specifier != nullptr) {
              node->items.pushBack(specifier);
            }
            break;
          } else {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
          specifier = readExportSpecifier(filename, source, current);
        }
      }
      auto backup = current;
      skipInvisible(filename, source, current);
      token = readIdentifierToken(filename, source, current);
      if (token != nullptr && token->location.startsWith(source, L"from")) {
        auto src = readStringLiteral(filename, source, current);
        if (!src) {
          throw std::runtime_error(
              formatException("Unexcepted token", filename, source, current));
        }
        node->source = src;
        auto backup = current;
        skipInvisible(filename, source, current);
        token = readIdentifierToken(filename, source, current);
        if (token != nullptr &&
            (token->location.startsWith(source, L"assert") ||
             token->location.startsWith(source, L"with"))) {
          skipInvisible(filename, source, current);
          token = readSymbolToken(filename, source, current);
          if (!token || !token->location.startsWith(source, L"{")) {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
          auto attribute = readImportAttriabue(filename, source, current);
          while (attribute != nullptr) {
            node->attributes.pushBack(attribute);
            auto backup = current;
            skipInvisible(filename, source, current);
            token = readSymbolToken(filename, source, current);
            if (token == nullptr) {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }
            if (token->location.startsWith(source, L"}")) {
              current = backup;
              break;
            }
            if (!token->location.startsWith(source, L",")) {
              throw std::runtime_error(formatException(
                  "Unexcepted token", filename, source, current));
            }
            attribute = readImportAttriabue(filename, source, current);
          }
          skipInvisible(filename, source, current);
          token = readSymbolToken(filename, source, current);
          if (!token || !token->location.startsWith(source, L"}")) {
            throw std::runtime_error(
                formatException("Unexcepted token", filename, source, current));
          }
        } else {
          current = backup;
        }
      } else if (node->items.size() &&
                 node->items[0]->type == NodeType::EXPORT_ALL) {
        throw std::runtime_error(
            formatException("Unexcepted token", filename, source, current));
      } else {
        current = backup;
      }
    }
    node->location = getLocation(source, position, current);
    position = current;
    return node;
  }
  return nullptr;
}