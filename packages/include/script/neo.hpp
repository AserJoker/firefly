#ifndef _NEO_JS_HEADER_
#define _NEO_JS_HEADER_
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace neo {
class JSRuntime;
class JSContext;
class JSValue;
class JSScope;
class JSAtom;
class JSValue {
private:
  JSAtom *_atom;
  std::wstring _name;

public:
  JSValue(JSAtom *atom, const std::wstring &name = L"")
      : _atom(atom), _name(name) {
    std::wcout << "JSValue: " << _name << " created" << std::endl;
  }

  ~JSValue() { std::wcout << "JSValue: " << _name << " disposed" << std::endl; }

  JSAtom *getAtom() { return _atom; }

  const JSAtom *getAtom() const { return _atom; }

  void setAtom(JSAtom *atom) { _atom = atom; }

  const std::wstring &getName() const { return _name; }
};

enum class JS_TYPE {
  UNDEFINED,
  NUMBER,
  STRING,
  BOOLEAN,
  BIGINT,
  OBJECT,
  FUNCTION,
  SYMBOL
};

class JSAtom {
private:
  std::vector<JSAtom *> _parents;
  std::vector<JSAtom *> _children;
  JS_TYPE _type;
  std::any _data;

public:
  JSAtom(JSAtom *parent, const JS_TYPE &type, const std::any &data)
      : _type(type), _data(data) {
    if (parent) {
      parent->addChild(this);
    }
    std::wcout << "JSAtom created: 0x" << std::hex << (uintptr_t)this
               << std::endl;
  }

  JSAtom() {}

  ~JSAtom() {
    if (_data.has_value()) {
      std::wcout << "JSAtom disposed: 0x" << std::hex << (uintptr_t)this
                 << std::endl;
    }
    for (auto child : _children) {
      removeChild(child);
    }
    for (auto parent : _parents) {
      parent->removeChild(this);
    }
  }

  void addChild(JSAtom *child) {
    _children.push_back(child);
    child->_parents.push_back(this);
  }

  void removeChild(JSAtom *child) {
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
      _children.erase(it);
    }
    it = std::find(child->_parents.begin(), child->_parents.end(), this);
    if (it != child->_parents.end()) {
      child->_parents.erase(it);
    }
    child->destory();
  }

  void destory() {
    std::vector<JSAtom *> cache = {};
    std::vector<JSAtom *> workflow = {this};
    while (!workflow.empty()) {
      auto item = *workflow.begin();
      workflow.pop_back();
      if (std::find(cache.begin(), cache.end(), item) == cache.end()) {
        if (!item->_data.has_value()) {
          return;
        }
        for (auto parent : item->_parents) {
          workflow.push_back(parent);
        }
        cache.push_back(item);
      }
    }
    delete this;
  }

  const std::any &getData() const { return _data; }

  void setData(const std::any &data) { _data = data; }

  const JS_TYPE &getType() const { return _type; }
};

class JSScope {
private:
  JSScope *_parent;
  JSAtom *_root;
  std::vector<JSScope *> _children;
  std::unordered_map<std::wstring, JSValue *> _namedVariables;
  std::vector<JSValue *> _variables;

public:
  JSScope(JSScope *parent = nullptr) : _parent(parent) {
    if (_parent) {
      _parent->_children.push_back(this);
    }
    _root = new JSAtom();
  }

  ~JSScope() {
    _parent = nullptr;
    for (auto child : _children) {
      delete child;
    }
    _children.clear();
    for (auto variable : _variables) {
      delete variable;
    }
    _variables.clear();
    _namedVariables.clear();
    delete _root;
    _root = nullptr;
  }

  JSScope *getParent() { return _parent; }

  void removeChild(JSScope *scope) {
    auto it = std::find(_children.begin(), _children.end(), scope);
    if (it != _children.end()) {
      _children.erase(it);
    }
  }

  JSAtom *createAtom(const JS_TYPE &type, const std::any &data) {
    return new JSAtom(_root, type, data);
  }

  JSValue *createValue(const std::wstring &name, JSAtom *atom) {
    _root->addChild(atom);
    JSValue *value = nullptr;
    if (!name.empty() && _namedVariables.contains(name)) {
      value = _namedVariables.at(name);
      value->setAtom(atom);
    } else {
      value = new JSValue(atom, name);
      _variables.push_back(value);
      if (!name.empty()) {
        _namedVariables[name] = value;
      }
    }
    return value;
  }

  JSValue *createValue(const std::wstring &name, const JS_TYPE &type,
                       const std::any &val) {
    auto atom = createAtom(type, val);
    JSValue *value = nullptr;
    if (!name.empty() && _namedVariables.contains(name)) {
      value = _namedVariables.at(name);
      value->setAtom(atom);
    } else {
      value = new JSValue(atom, name);
      _variables.push_back(value);
      if (!name.empty()) {
        _namedVariables[name] = value;
      }
    }
    return value;
  }

  JSValue *queryValue(const std::wstring &name) {
    if (_namedVariables.contains(name)) {
      return _namedVariables.at(name);
    }
    return nullptr;
  }
};

using JS_NATIVE =
    std::function<JSValue *(JSContext *, JSValue *, std::vector<JSValue *>)>;

class JSObject {};

class JSArray : public JSObject {};

class JSCallable : public JSObject {
public:
  std::unordered_map<std::wstring, JSAtom *> _closure;

public:
  JSCallable(const std::unordered_map<std::wstring, JSAtom *> &closure)
      : _closure(closure) {};
  const std::unordered_map<std::wstring, JSAtom *> &getClosure() const {
    return _closure;
  }

  virtual JSValue *call(JSContext *ctx, JSValue *self,
                        const std::vector<JSValue *> args) = 0;
};

class JSNativeFunction : public JSCallable {
private:
  JS_NATIVE _native;

public:
  JSNativeFunction(const JS_NATIVE &native,
                   const std::unordered_map<std::wstring, JSAtom *> &closure)
      : JSCallable(closure), _native(native) {}
  JSValue *call(JSContext *ctx, JSValue *self,
                const std::vector<JSValue *> args) {
    return _native(ctx, self, args);
  }
};

class JSFunction : public JSCallable {};

class JSRuntime {
public:
  JSRuntime() {}
  ~JSRuntime() {}
};

class JSContext {
private:
  JSRuntime *_runtime;
  JSScope *_root;
  JSScope *_current;

public:
  JSContext(JSRuntime *runtime) : _runtime(runtime) {
    _root = new JSScope();
    _current = _root;
  }

  ~JSContext() {
    _current = nullptr;
    delete _root;
    _root = nullptr;
    _runtime = nullptr;
  }
  void pushScope() {
    std::cout << "push scope" << std::endl;
    _current = new JSScope(_current);
  }
  void popScope() {
    auto parent = _current->getParent();
    parent->removeChild(_current);
    delete _current;
    _current = parent;
    std::cout << "pop scope" << std::endl;
  }

  JSValue *createNumber(const std::wstring &name, double value) {
    return _current->createValue(name, JS_TYPE::NUMBER, value);
  }

  JSValue *createString(const std::wstring &name, const std::wstring &value) {
    return _current->createValue(name, JS_TYPE::STRING, value);
  }

  JSValue *createBoolean(const std::wstring &name, bool value) {
    return _current->createValue(name, JS_TYPE::BOOLEAN, value);
  }

  JSValue *createObject(const std::wstring &name) {
    return _current->createValue(name, JS_TYPE::OBJECT, JSObject{});
  }

  JSValue *createArray(const std::wstring &name) {
    return _current->createValue(name, JS_TYPE::OBJECT, JSArray{});
  }

  JSValue *createNativeFunction(
      const std::wstring &name, const JS_NATIVE &func,
      const std::unordered_map<std::wstring, JSValue *> &closure = {}) {
    std::unordered_map<std::wstring, JSAtom *> clo;
    for (auto &[n, val] : closure) {
      clo[n] = val->getAtom();
    }
    auto val = _current->createValue(name, JS_TYPE::FUNCTION,
                                     JSNativeFunction{func, clo});
    for (auto &[n, atom] : clo) {
      val->getAtom()->addChild(atom);
    }
    return val;
  }

  JSValue *call(JSValue *func, JSValue *self,
                const std::vector<JSValue *> args) {
    auto current = _current;
    pushScope();
    auto fn = const_cast<JSCallable *>(
        std::any_cast<JSCallable>(&func->getAtom()->getData()));
    for (auto &[key, val] : fn->getClosure()) {
      _current->createValue(key, val);
    }
    for (auto &arg : args) {
      _current->createValue(arg->getName(), arg->getAtom());
    }
    auto res = fn->call(this, self, args);
    auto result = current->createValue(res->getName(), res->getAtom());
    popScope();
    return result;
  }
};
} // namespace neo
#endif