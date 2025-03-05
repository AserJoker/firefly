#include "script/engine/JSFunction.hpp"
#include "script/engine/JSFunctionType.hpp"
#include "script/util/JSSingleton.hpp"
JSFunction::JSFunction(
    JSAllocator *allocator, const std::wstring &name, const std::wstring &path,
    size_t address, const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSCallable(allocator, name, closure,
                 JSSingleton::instance<JSFunctionType>(allocator)),
      _path(path), _address(address) {}
