#include "script/engine/JSNumber.hpp"
#include "script/engine/JSString.hpp"
#include <sstream>
JSNumber::JSNumber(JSAllocator *allocator, double value)
    : JSBase(allocator, JS_TYPE::NUMBER), _value(value) {}
JSBase *JSNumber::toString() {
  std::wstringstream ss;
  ss << _value;
  return getAllocator()->create<JSString>(ss.str());
}
JSBase *JSNumber::clone() { return getAllocator()->create<JSNumber>(_value); };