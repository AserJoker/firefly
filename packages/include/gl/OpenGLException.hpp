#pragma once
#include "ErrorType.hpp"
#include "exception/Exception.hpp"
#include <fmt/core.h>
#include <glad/glad.h>
namespace firefly::gl {
class OpenGLException : public exception::Exception {
private:
  static std::string format(const std::string &message, ERROR_TYPE error) {
    std::string err;
    switch (error) {
    case ERROR_TYPE::NO_ERROR:
      break;
    case ERROR_TYPE::INVALID_ENUM:
      err = "GL_INVALID_ENUM";
      break;
    case ERROR_TYPE::INVALID_VALUD:
      err = "GL_INVALID_VALUD";
      break;
    case ERROR_TYPE::INVALID_OPERATION:
      err = "GL_INVALID_OPERATION";
      break;
    case ERROR_TYPE::INVALID_FRAMEBUFFER_OPERATION:
      err = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
    case ERROR_TYPE::OUT_OF_MEMORY:
      err = "GL_OUT_OF_MEMORY";
      break;
    case ERROR_TYPE::STACK_UNDERFLOW:
      err = "GL_INVALID_ENUM";
      break;
    case ERROR_TYPE::STACK_OVERFLOW:
      err = "GL_STACK_OVERFLOW";
      break;
    }
    return fmt::format("{}: {}", message, err);
  }

public:
  OpenGLException(
      const std::string &message, const ERROR_TYPE error,
      const std::source_location &current = std::source_location::current())
      : Exception("OpenGLException", format(message, error), current) {}
};
#define CHECK_OPENGL(msg)                                                      \
  while (auto error = glGetError()) {                                          \
    throw OpenGLException(msg, (ERROR_TYPE)error);                             \
  }
} // namespace firefly::gl