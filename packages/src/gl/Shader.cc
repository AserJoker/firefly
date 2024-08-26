#include "gl/Shader.hpp"
#include "exception/Exception.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
using namespace firefly;
using namespace firefly::gl;

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

const void Shader::use() const { glUseProgram(_handle); }
const uint32_t &Shader::getHandle() const { return _handle; }
void Shader::setValue(const std::string &name, const bool &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform1i(location, value);
}
void Shader::setValue(const std::string &name, const int32_t &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform1i(location, value);
}
void Shader::setValue(const std::string &name, const uint32_t &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform1ui(location, value);
}
void Shader::setValue(const std::string &name, const float &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform1f(location, value);
}
void Shader::setValue(const std::string &name, const double &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform1d(location, value);
}
void Shader::setValue(const std::string &name, const glm::vec2 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform2fv(location, 1, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::vec3 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform3fv(location, 1, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::vec4 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniform4fv(location, 1, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat2 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat3 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat4 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat2x3 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat3x2 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat3x4 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setValue(const std::string &name, const glm::mat4x3 &value) {
  if (!_locations.contains(name)) {
    _locations[name] = glGetUniformLocation(_handle, name.c_str());
  }
  auto &location = _locations.at(name);
  glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}