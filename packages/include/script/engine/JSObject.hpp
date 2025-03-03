#pragma once

#include "JSAtom.hpp"
#include "JSValue.hpp"
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
  std::unordered_map<std::wstring, JSField> _fields;
  std::unordered_map<JSAtom *, JSField> _symboledFields;
  std::unordered_map<std::wstring, JSField> _privateFields;
  bool _sealed;
  bool _frozen;
  bool _extensible;
  JSAtom *_prototype;
  JSAtom *_constructor;

public:
  JSObject(JSAllocator *allocator, const JS_TYPE &type = JS_TYPE::OBJECT);

  inline JSAtom *getPrototype() { return _prototype; }

  inline const JSAtom *getPrototype() const { return _prototype; }

  inline void setPrototype(JSAtom *prototype) { _prototype = prototype; }

  inline JSAtom *getConstructor() { return _constructor; }

  inline const JSAtom *getConstructor() const { return _constructor; }

  inline void setConstructor(JSAtom *constructor) {
    _constructor = constructor;
  }

  inline const std::unordered_map<std::wstring, JSField> &getFields() const {
    return _fields;
  }

  inline std::unordered_map<std::wstring, JSField> &getFields() {
    return _fields;
  }

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

public:
  JSBase *toString() override;

  JSBase *clone() override;

  virtual std::vector<std::wstring> getKeys(JSContext *ctx) {
    std::vector<std::wstring> keys;
    for (auto &[key, field] : _fields) {
      if (field.enumable) {
        keys.push_back(key);
      }
    }
    return keys;
  };

  virtual JSValue *getField(JSContext *ctx, JSValue *self,
                            const std::wstring &name);

  virtual JSValue *setField(JSContext *ctx, JSValue *self,
                            const std::wstring &name, JSValue *value);

  virtual JSValue *getIndex(JSContext *ctx, JSValue *self, size_t index);

  virtual JSValue *setIndex(JSContext *ctx, JSValue *self, size_t index,
                            JSValue *value);

  virtual JSValue *getField(JSContext *ctx, JSValue *self, JSValue *name);

  virtual JSValue *setField(JSContext *ctx, JSValue *self, JSValue *name,
                            JSValue *value);
};