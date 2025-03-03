#include "script/engine/JSString.hpp"

inline JSBase *JSString::toString() {
  return getAllocator()->create<JSString>(_value);
}

inline JSBase *JSString::clone() {
  return getAllocator()->create<JSString>(_value);
}