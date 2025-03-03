#include "script/engine/JSArray.hpp"
#include "script/engine/JSContext.hpp"
#include "script/engine/JSString.hpp"
#include <sstream>


JSArray::JSArray(JSAllocator *allocator) : JSObject(allocator) {}

JSBase *JSArray::toString() {
  std::wstringstream ss;
  for (size_t index = 0; index < _length; index++) {
    if (_items.contains(index)) {
      auto str = _items[index]->getData()->toString();
      str->addRef();
      ss << str->cast<JSString>()->getValue();
      str->dispose();
    }
    if (index != _length - 1) {
      ss << L",";
    }
  }
  return getAllocator()->create<JSString>(ss.str());
}
std::vector<std::wstring> JSArray::getKeys(JSContext *ctx) {
  std::vector<std::wstring> keys;
  for (auto &[key, _] : _items) {
    keys.push_back(std::to_wstring(key));
  }
  return keys;
}
JSValue *JSArray::getIndex(JSContext *ctx, JSValue *self, size_t index) {
  if (_items.contains(index)) {
    return ctx->getScope()->createValue(_items.at(index));
  }
  return ctx->createUndefined();
}

JSValue *JSArray::setIndex(JSContext *ctx, JSValue *self, size_t index,
                           JSValue *value) {
  if (_items.contains(index)) {
    if (isFrozen()) {
      return ctx->createException(JSException::TYPE::TYPE,
                                  L"Cannot assign to read only property '" +
                                      std::to_wstring(index) + L"' of object");
    }
    self->getAtom()->removeChild(_items.at(index));
    self->getAtom()->addChild(value->getAtom());
  } else {
    if (!isExtensible() || isFrozen() || isSealed()) {
      return ctx->createException(JSException::TYPE::TYPE,
                                  L"Cannot add property " +
                                      std::to_wstring(index) +
                                      L", object is not extensible");
    }
    self->getAtom()->addChild(value->getAtom());
  }
  _items[index] = value->getAtom();
  if (index >= _length) {
    _length = index + 1;
  }
  return nullptr;
}