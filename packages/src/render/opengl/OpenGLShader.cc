#include "render/opengl/OpenGLShader.hpp"
#include "core/Exception.hpp"
#include <unordered_map>
using namespace firefly;
using namespace firefly::render;
OpenGLShader::OpenGLShader(
    const std::unordered_map<TYPE, std::string> &sources) {
  _handle = glCreateProgram();
  for (auto &[type, source] : sources) {
    auto handle = glCreateShader((GLenum)type);
    auto str = source.c_str();
    glShaderSource(handle, 1, &str, nullptr);
    glCompileShader(handle);
    int success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success) {
      int len = 0;
      glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len);
      char *buf = new char[len];
      glGetShaderInfoLog(handle, len + 1, &len, buf);
      std::string message(buf, len);
      delete[] buf;
      glDeleteShader(handle);
      glDeleteShader(_handle);
      throw core::Exception(
          fmt::format("Failed to compile shader:\n{}", message));
    }
    glAttachShader(_handle, handle);
  }
  glLinkProgram(_handle);
  int success = 0;
  glGetProgramiv(_handle, GL_COMPILE_STATUS, &success);
  if (!success) {
    int len = 0;
    glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len);
    char *buf = new char[len];
    glGetProgramInfoLog(_handle, len + 1, &len, buf);
    std::string message(buf, len);
    delete[] buf;
    glDeleteProgram(_handle);
    throw core::Exception(
        fmt::format("Failed to compile shader:\n{}", message));
  }
}

OpenGLShader::~OpenGLShader() {
  if (_handle) {
    glDeleteProgram(_handle);
  }
}

void OpenGLShader::use() { glUseProgram(_handle); }