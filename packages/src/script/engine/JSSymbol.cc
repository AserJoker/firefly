#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSString.hpp"

JSBase *JSSymbol::clone() { return this; }

JSBase *JSSymbol::toString() {
  return getAllocator()->create<JSString>(L"Symbol(" + _description + L")");
}