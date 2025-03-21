#include "script/engine/JSFunction.hpp"
#include "script/engine/JSFunctionType.hpp"
#include "script/engine/JSType.hpp"
#include "script/util/JSSingleton.hpp"
JSFunction::JSFunction(
    JSAllocator *allocator, const std::wstring &name, const std::wstring &path,
    size_t address, const std::unordered_map<std::wstring, JSAtom *> &closure,
    JSType *type)
    : JSCallable(allocator, name, closure,
                 type != nullptr
                     ? type
                     : JSSingleton::instance<JSFunctionType>(allocator)),
      _path(path), _address(address) {}