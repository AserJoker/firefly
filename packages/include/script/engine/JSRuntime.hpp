#pragma once
#include "../compiler/JSCodeGenerator.hpp"
#include "../compiler/JSParser.hpp"
#include "../util/JSLogger.hpp"
class JSVirtualMachine;
class JSRuntime {
private:
  JSParser *_parser{};

  JSCodeGenerator *_generator{};

  JSVirtualMachine *_vm{};

  JSLogger *_logger{};

  JSAllocator *_allocator{};

  std::unordered_map<std::wstring, JSProgram> _programs;

  std::vector<std::wstring> _args;

public:
  JSRuntime(int argc, char **argv);

  ~JSRuntime();

  JSParser *getParser();

  void setParser(JSParser *parser);

  JSCodeGenerator *getGenerator();

  void setGenerator(JSCodeGenerator *generator);

  JSVirtualMachine *getVirtualMachine();

  void setVirtualMachine(JSVirtualMachine *vm);

  JSLogger *getLogger();

  void setLogger(JSLogger *logger);

  JSAllocator *getAllocator();

  void setAllocator(JSAllocator *allocator);

  const std::vector<std::wstring> &getArgs() const { return _args; }

  void enableFeature(const std::wstring &feature) {}

  void disableFeature(const std::wstring &feature) {}

  inline bool isSupportFeature(const std::wstring &feature) { return false; }

  const JSProgram &getProgram(const std::wstring &path) const;

  bool hasProgram(const std::wstring &path) const;

  JSProgram &getProgram(const std::wstring &path);

  JSProgram &compile(const std::wstring &path, const std::wstring &source,
                     const JS_EVAL_TYPE &type = JS_EVAL_TYPE::PROGRAM);
};