#include "script/engine/JSCallable.hpp"
#include "script/engine/JSString.hpp"
JSCallable::JSCallable(
    JSAllocator *allocator, const std::wstring &name,
    const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSObject(allocator, JS_TYPE::FUNCTION), _closure(closure), _name(name),
      _globalContext(false){};
      
JSBase *JSCallable ::toString() {
  auto name = _name;
  if (name.empty()) {
    name = L"anonymous";
  }
  return getAllocator()->create<JSString>(L"[Function " + name + L"]");
}