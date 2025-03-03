#include "script/engine/JSUndefined.hpp"
#include "script/engine/JSString.hpp"
JSBase *JSUndefined::toString() {
  return getAllocator()->create<JSString>(L"undefined");
}
JSBase *JSUndefined::clone() { return this; }
