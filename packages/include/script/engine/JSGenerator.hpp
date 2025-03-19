#pragma once
#include "JSObject.hpp"
#include "script/engine/JSAtom.hpp"
#include "script/engine/JSScope.hpp"
#include "script/util/JSAllocator.hpp"
#include <vector>
class JSGenerator : public JSObject {
private:
  JSAtom *_function{};

  JSAtom *_self{};

  std::vector<JSAtom *> _args{};

  JSAtom *_interrupt{};

  JSScope *_scope{};

public:
  JSGenerator(JSAllocator *allocator) : JSObject(allocator){};

  void setFunctionCall(JSAtom *function, JSAtom *self,
                       const std::vector<JSAtom *> &args) {
    _function = function;
    _self = self;
    _args = args;
  };

  JSAtom *getFunction() { return _function; };

  JSAtom *getSelf() { return _self; };

  std::vector<JSAtom *> &getArgs() { return _args; };

  void setInterrupt(JSAtom *interrupt) { _interrupt = interrupt; }

  JSAtom *getInterrupt() { return _interrupt; }

  JSScope *getScope() { return _scope; }

  void setScope(JSScope *scope) { _scope = scope; }
};