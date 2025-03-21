#pragma once

#include "JSObject.hpp"
#include "JSType.hpp"
#include "JSValue.hpp"
#include "script/engine/JSAtom.hpp"
#include "script/engine/JSValue.hpp"
#include <functional>
class JSContext;
using JS_NATIVE =
    std::function<JSValue *(JSContext *, JSValue *, std::vector<JSValue *>)>;

class JSCallable : public JSObject {
private:
  std::unordered_map<std::wstring, JSAtom *> _closure;

  std::wstring _name;

  bool _globalContext;

  JSAtom *_self;

  JSAtom *_clazz;

public:
  JSCallable(JSAllocator *allocator, const std::wstring &name,
             const std::unordered_map<std::wstring, JSAtom *> &closure,
             JSType *type);
  inline const std::unordered_map<std::wstring, JSAtom *> &getClosure() const {
    return _closure;
  }

  inline bool isGlobalContext() const { return _globalContext; }

  inline void setGlobalContext(bool context) { _globalContext = context; }

  inline const std::wstring &getName() const { return _name; }

  inline void setName(const std::wstring &name) { _name = name; }

  inline void setClosure(const std::wstring &name, JSAtom *atom) {
    _closure[name] = atom;
  }

  inline JSAtom *getClosure(const std::wstring &name) {
    if (_closure.contains(name)) {
      return _closure.at(name);
    }
    return nullptr;
  }

  void setSelf(JSAtom *self);

  JSAtom *getSelf();

  void setClass(JSAtom *clazz);

  JSAtom *getClass();
};