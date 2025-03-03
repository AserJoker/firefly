#pragma once
#include "../util/JSAllocator.hpp"
#include "JSType.hpp"
#include <string>
class JSBase {
private:
  JSAllocator *_allocator;
  size_t _ref;

  JS_TYPE _type;

public:
  JSBase(JSAllocator *allocator, const JS_TYPE &type);

  virtual ~JSBase() = default;

  inline JSAllocator *getAllocator() { return _allocator; }

  inline size_t addRef() { return ++_ref; }

  inline size_t dispose() { return --_ref; }

  void release();

  inline const JS_TYPE &getType() const { return _type; }

  inline std::wstring getTypeName() const {
    switch (_type) {
    case JS_TYPE::UNDEFINED:
      return L"undefined";
    case JS_TYPE::NUMBER:
      return L"number";
    case JS_TYPE::STRING:
      return L"string";
    case JS_TYPE::BOOLEAN:
      return L"boolean";
    case JS_TYPE::BIGINT:
      return L"bigint";
    case JS_TYPE::OBJECT:
      return L"object";
    case JS_TYPE::FUNCTION:
      return L"function";
    case JS_TYPE::SYMBOL:
      return L"symbol";
    case JS_TYPE::CLASS:
      return L"function";
    case JS_TYPE::EXCEPTION:
      return L"exception";
    case JS_TYPE::INTERRUPT:
      return L"interrupt";
    case JS_TYPE::INTERNAL:
      return L"internal";
    case JS_TYPE::UNINITIALIZED:
      return L"uninitialized";
    }
  }

  virtual inline JSBase *toString() = 0;

  virtual inline JSBase *clone() = 0;

  template <class T> T *cast() { return dynamic_cast<T *>(this); }
};