#pragma once
#include <CUnit/CUnit.h>
#include <stdexcept>
class TestException : public std::runtime_error {
private:
  int _error;

public:
  TestException(const std::string &message = CU_get_error_msg(),
                int error = CU_get_error())
      : std::runtime_error(message), _error(error) {}
  int getErrorCode() const { return _error; }
};