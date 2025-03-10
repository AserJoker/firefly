#pragma once

#include "JSBase.hpp"
#include "JSStackFrame.hpp"
#include <vector>
class JSException : public JSBase {
public:
  enum class TYPE { ERROR, INTERNAL, REFERENCE, SYNTAX, TYPE };

private:
  std::wstring _message;
  TYPE _type{TYPE::ERROR};
  std::vector<JSStackFrame> _stack;

public:
  JSException(JSAllocator *allocator, const TYPE &type,
              const std::wstring &message,
              const std::vector<JSStackFrame> &stack);

  inline const TYPE &getType() const { return _type; }

  inline std::wstring getTypeName() const {
    switch (_type) {
    case TYPE::ERROR:
      return L"Error";
    case TYPE::INTERNAL:
      return L"InternalError";
    case TYPE::REFERENCE:
      return L"ReferenceError";
    case TYPE::SYNTAX:
      return L"SyntaxError";
    case TYPE::TYPE:
      return L"TypeError";
    };
    return L"Error";
  }

  inline const std::wstring &getMessage() const { return _message; };

  inline const std::vector<JSStackFrame> &getStack() const { return _stack; }
};
