#include "script/engine/JSGeneratorFunction.hpp"
#include "script/engine/JSFunction.hpp"
#include "script/engine/JSGeneratorFunctionType.hpp"
#include "script/util/JSSingleton.hpp"

JSGeneratorFunction::JSGeneratorFunction(
    JSAllocator *allocator, const std::wstring &name, const std::wstring &path,
    size_t address, const std::unordered_map<std::wstring, JSAtom *> &closure)
    : JSFunction(allocator, name, path, address, closure,
                 JSSingleton::instance<JSGeneratorFunctionType>(allocator)) {}