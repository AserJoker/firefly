#pragma once
#include "JSBase.hpp"
#include "JSEvalContext.hpp"
#include "JSInterruptType.hpp"
#include "script/engine/JSScope.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
class JSInterrupt : public JSBase {
private:
  JSEvalContext _ectx;
  JSScope *_scope;
  JSAtom *_value;

public:
  JSInterrupt(JSAllocator *allocator, const JSEvalContext &ectx, JSScope *scope,
              JSAtom *value)
      : JSBase(allocator, JSSingleton::instance<JSInterruptType>(allocator)),
        _ectx(ectx), _scope(scope), _value(value) {}

  inline JSEvalContext &getEvalContext() { return _ectx; }

  inline JSScope *getScope() { return _scope; }

  inline void setScope(JSScope *scope) { _scope = scope; }

  inline JSAtom *getValue() { return _value; }

  inline void setValue(JSAtom *value) { _value = value; }
};