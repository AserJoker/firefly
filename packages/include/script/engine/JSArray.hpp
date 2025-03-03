#pragma once
#include "JSObject.hpp"
#include <cstddef>
#include <unordered_map>
class JSArray : public JSObject {
private:
  std::unordered_map<size_t, JSAtom *> _items;

  size_t _length{};

public:
  JSArray(JSAllocator *allocator);

  inline std::unordered_map<size_t, JSAtom *> &getItems() { return _items; }

  inline const std::unordered_map<size_t, JSAtom *> &getItems() const {
    return _items;
  }

  inline size_t &getLength() { return _length; }

  inline size_t getLength() const { return _length; }

public:
  JSBase *toString() override;

  std::vector<std::wstring> getKeys(JSContext *ctx) override;

  JSValue *getIndex(JSContext *ctx, JSValue *self, size_t index) override;

  JSValue *setIndex(JSContext *ctx, JSValue *self, size_t index,
                    JSValue *value) override;
};