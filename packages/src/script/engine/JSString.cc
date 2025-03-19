#include "script/engine/JSString.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSStringType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
#include <string>

JSString::JSString(JSAllocator *allocator, const std::wstring &value)
    : JSBase(allocator, JSSingleton::instance<JSStringType>(allocator)),
      _value(value) {}