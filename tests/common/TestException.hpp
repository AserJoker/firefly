#pragma once
#include "core/Type.hpp"
#include <CUnit/CUnit.h>
#include <stdexcept>
using namespace firefly;
class TestException : public std::runtime_error {
private:
  int _error;

public:
  TestException(const core::String_t &message = CU_get_error_msg(),
                int error = CU_get_error())
      : std::runtime_error(message), _error(error) {}
  int getErrorCode() const { return _error; }
};