#pragma once

#include "JSAtom.hpp"
#include "JSType.hpp"
#include <map>
#include <string>
#include <unordered_map>
class JSContext;
struct JSField {
  bool configurable{};
  bool enumable{};

  JSAtom *value{};
  bool writable{};

  JSAtom *getter{};
  JSAtom *setter{};
};

class JSObject : public JSBase {
private:
  std::map<JSAtom *, JSField> _fields;
  std::unordered_map<std::wstring, JSField> _privateFields;
  bool _sealed;
  bool _frozen;
  bool _extensible;
  JSAtom *_prototype;
  JSAtom *_constructor;

public:
  JSObject(JSAllocator *allocator, JSType *type = nullptr);

  inline JSAtom *getPrototype() { return _prototype; }

  inline const JSAtom *getPrototype() const { return _prototype; }

  inline void setPrototype(JSAtom *prototype) { _prototype = prototype; }

  inline JSAtom *getConstructor() { return _constructor; }

  inline const JSAtom *getConstructor() const { return _constructor; }

  inline void setConstructor(JSAtom *constructor) {
    _constructor = constructor;
  }

  inline const std::map<JSAtom *, JSField> &getFields() const {
    return _fields;
  }

  inline std::map<JSAtom *, JSField> &getFields() { return _fields; }

  inline const std::unordered_map<std::wstring, JSField> &
  getPrivateFields() const {
    return _privateFields;
  }

  inline std::unordered_map<std::wstring, JSField> &getPrivateFields() {
    return _privateFields;
  }

  inline bool isSealed() const { return _sealed; }

  inline bool isFrozen() const { return _frozen; }

  inline bool isExtensible() const { return _extensible; }

  inline void setSealed(bool value) { _sealed = value; }

  inline void setFrozen(bool value) { _frozen = value; }

  inline void setExtensible(bool value) { _extensible = value; }
};