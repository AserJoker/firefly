#include "script/engine/JSSymbol.hpp"
#include "script/engine/JSBase.hpp"
#include "script/engine/JSSymbolType.hpp"
#include "script/util/JSAllocator.hpp"
#include "script/util/JSSingleton.hpp"
#include <string>

JSSymbol::JSSymbol(JSAllocator *allocator, const std::wstring &description)
    : JSBase(allocator, JSSingleton::instance<JSSymbolType>(allocator)),
      _description(description) {}