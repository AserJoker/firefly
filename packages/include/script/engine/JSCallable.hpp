#pragma once

#include "JSObject.hpp"
#include "JSValue.hpp"
#include <functional>
class JSContext;
using JS_NATIVE =
    std::function<JSValue *(JSContext *, JSValue *, std::vector<JSValue *>)>;

class JSCallable : public JSObject {
private:
  std::unordered_map<std::wstring, JSAtom *> _closure;
  std::wstring _name;
  bool _globalContext;

public:
  JSCallable(JSAllocator *allocator, const std::wstring &name,
             const std::unordered_map<std::wstring, JSAtom *> &closure);
  inline const std::unordered_map<std::wstring, JSAtom *> &getClosure() const {
    return _closure;
  }

  inline bool isGlobalContext() const { return _globalContext; }

  inline void setGlobalContext(bool context) { _globalContext = context; }

  virtual JSValue *call(JSContext *ctx, JSValue *self,
                        const std::vector<JSValue *> args) = 0;

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

public:
  JSBase *toString() override;
};