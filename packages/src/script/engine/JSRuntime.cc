#include "script/engine/JSRuntime.hpp"
#include "script/engine/JSVirtualMachine.hpp"
#include <codecvt>

JSRuntime::JSRuntime(int argc, char **argv) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
  for (int index = 0; index < argc; index++) {
    _args.push_back(convert.from_bytes(argv[index]));
  }
}

JSRuntime::~JSRuntime() {
  if (_vm) {
    getAllocator()->dispose(_vm);
    _vm = nullptr;
  }
  if (_generator) {
    _allocator->dispose(_generator);
    _generator = nullptr;
  }
  if (_parser) {
    _allocator->dispose(_parser);
    _parser = nullptr;
  }
  if (_logger) {
    _allocator->dispose(_logger);
    _logger = nullptr;
  }
  if (_allocator) {
    _allocator->dispose();
    _allocator = nullptr;
  }
}

JSParser *JSRuntime::getParser() {
  if (_parser == nullptr) {
    _parser = getAllocator()->create<JSParser>();
  }
  return _parser;
}

void JSRuntime::setParser(JSParser *parser) {
  if (_parser == parser) {
    return;
  }
  if (_parser) {
    getAllocator()->dispose(_parser);
  }
  _parser = parser;
}

JSCodeGenerator *JSRuntime::getGenerator() {
  if (_generator == nullptr) {
    _generator = getAllocator()->create<JSCodeGenerator>();
  }
  return _generator;
}

void JSRuntime::setGenerator(JSCodeGenerator *generator) {
  if (_generator == generator) {
    return;
  }
  if (_generator) {
    getAllocator()->dispose(_generator);
  }
  _generator = generator;
}

void JSRuntime::setVirtualMachine(JSVirtualMachine *vm) {
  if (_vm == vm) {
    return;
  }
  if (_vm) {
    getAllocator()->dispose(_vm);
  }
  _vm = vm;
}

JSVirtualMachine *JSRuntime::getVirtualMachine() {
  if (_vm == nullptr) {
    _vm = getAllocator()->create<JSVirtualMachine>();
  }
  return _vm;
}

JSLogger *JSRuntime::getLogger() {
  if (_logger == nullptr) {
    _logger = getAllocator()->create<JSLogger>();
  }
  return _logger;
}

void JSRuntime::setLogger(JSLogger *logger) {
  if (logger == _logger) {
    return;
  }
  if (_logger) {
    getAllocator()->dispose(_logger);
  }
  _logger = logger;
}
JSAllocator *JSRuntime::getAllocator() {
  if (!_allocator) {
    _allocator = new JSAllocator{};
  }
  return _allocator;
}

void JSRuntime::setAllocator(JSAllocator *allocator) {
  if (_allocator == allocator) {
    return;
  }
  if (_allocator) {
    _allocator->dispose();
  }
  _allocator = allocator;
}

bool JSRuntime::hasProgram(const std::wstring &path) const {
  return _programs.contains(path);
}

JSProgram &JSRuntime::getProgram(const std::wstring &path) {
  auto &program = _programs[path];
  if (program.filename.empty()) {
    program.filename = path;
    program.allocator = _allocator;
  }
  return program;
}

JSProgram &JSRuntime::compile(const std::wstring &path,
                              const std::wstring &source,
                              const JS_EVAL_TYPE &type) {
  if (_programs.contains(path)) {
    _programs.erase(path);
  }
  auto &program = getProgram(path);
  auto node = getParser()->parse(source, type);
  if (node->type == JS_NODE_TYPE::ERROR) {
    program.error = node->cast<JSErrorNode>();
    return program;
  }
  auto err = getGenerator()->resolve(source, node, program);
  getAllocator()->dispose(node);
  if (err) {
    program.error = err->cast<JSErrorNode>();
  }
  return program;
}