#include "script/engine/JSException.hpp"
#include "script/engine/JSString.hpp"
#include <sstream>
JSException::JSException(JSAllocator *allocator, const TYPE &type,
                         const std::wstring &message,
                         const std::vector<JSStackFrame> &stack)
    : JSBase(allocator, JS_TYPE::EXCEPTION), _message(message), _type(type),
      _stack(stack){};
JSBase *JSException ::toString() {
  std::wstringstream ss;
  ss << getTypeName() << L": ";
  ss << _message << std::endl;
  for (auto it = _stack.rbegin(); it != _stack.rend(); it++) {
    auto funcname = it->position.funcname;
    if (funcname.empty()) {
      funcname = L"anonymous";
    }
    if (it->filename.empty()) {
      ss << L"  at " << funcname << L"(<internal>)";
    } else {
      ss << L"  at " << funcname << L"(" << it->filename << L":"
         << it->position.line << L":" << it->position.column << L")\n";
    }
  }
  return getAllocator()->create<JSString>(ss.str());
}
JSBase *JSException::clone() { return this; }