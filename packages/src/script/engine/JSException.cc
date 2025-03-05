#include "script/engine/JSException.hpp"
#include "script/engine/JSExceptionType.hpp"
#include "script/util/JSSingleton.hpp"
JSException::JSException(JSAllocator *allocator, const TYPE &type,
                         const std::wstring &message,
                         const std::vector<JSStackFrame> &stack)
    : JSBase(allocator, JSSingleton::instance<JSExceptionType>(allocator)),
      _message(message), _type(type), _stack(stack){};
