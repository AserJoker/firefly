#pragma once
#include "script/util/JSAllocator.hpp"
#include "script/util/JSRef.hpp"
#include <cstdint>
class JSContext;
class JSValue;
class JSType : public JSRef {
private:
  int32_t _priority;

public:
  JSType(JSAllocator *allocator, int32_t priority)
      : JSRef(allocator), _priority(priority){};

  inline const int32_t &getPriority() const { return _priority; }

  inline void setPriority(int32_t priority) { _priority = priority; }

  virtual ~JSType() = default;

  virtual const wchar_t *getTypeName() const { return L"internal"; };

  virtual JSValue *toString(JSContext *ctx, JSValue *value) const = 0;

  virtual JSValue *toNumber(JSContext *ctx, JSValue *value) const = 0;

  virtual JSValue *toBoolean(JSContext *ctx, JSValue *value) const = 0;

  virtual JSValue *clone(JSContext *ctx, JSValue *value) const = 0;

  virtual JSValue *pack(JSContext *ctx, JSValue *value) const = 0;

  virtual JSValue *equal(JSContext *ctx, JSValue *value,
                         JSValue *another) const = 0;

public:
  template <class T> const T *cast() const {
    return dynamic_cast<const T *>(this);
  }

  template <class T> T *cast() { return dynamic_cast<T *>(this); }
};