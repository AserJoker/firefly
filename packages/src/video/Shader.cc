#include "video/Shader.hpp"
#include "exception/Exception.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
using namespace firefly;
using namespace firefly::video;

Shader::Shader(const std::unordered_map<uint32_t, std::string> &sources,
               const uint32_t &handle)
    : _handle(handle) {
  if (!_handle) {
    _handle = glCreateProgram();
  }
  int success;
  for (auto &[type, source] : sources) {
    auto shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      int32_t len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
      char *buf = new char[len];
      glGetShaderInfoLog(shader, len, NULL, buf);
      std::string msg = buf;
      delete[] buf;
      glDeleteShader(shader);
      throw exception::RuntimeException<"ShaderCompile">(msg);
    }
    glAttachShader(_handle, shader);
    glDeleteShader(shader);
  }
  glLinkProgram(_handle);
  glGetProgramiv(_handle, GL_LINK_STATUS, &success);
  if (!success) {
    int32_t len;
    glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len);
    char *buf = new char[len];
    glGetProgramInfoLog(_handle, len, NULL, buf);
    std::string msg = buf;
    delete[] buf;
    throw exception::RuntimeException<"ShaderLink">(msg);
  }
}

Shader::~Shader() {
  if (_handle) {
    glDeleteProgram(_handle);
  }
}

void Shader::set(const std::string &name, const glm::mat3 &value) {
  auto ptr = glm::value_ptr(value);
  auto loc = glGetUniformLocation(_handle, name.c_str());
  glUniformMatrix3fv(loc, 1, GL_TRUE, ptr);
}

void Shader::set(const std::string &name, const glm::mat4 &value) {
  auto ptr = glm::value_ptr(value);
  auto loc = glGetUniformLocation(_handle, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_TRUE, ptr);
}

const void Shader::use() const { glUseProgram(_handle); }
const uint32_t &Shader::getHandle() const { return _handle; }