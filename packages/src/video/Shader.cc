#include "video/Shader.hpp"
#include "core/AutoPtr.hpp"
#include <fmt/core.h>
#include <map>
#include <stdexcept>
#include <vector>
using namespace firefly;
using namespace firefly::video;
Shader::Shader(
    const std::map<ShaderType, core::AutoPtr<runtime::Resource>> &sources)
    : _handle(0) {
  std::vector<uint32_t> shaders;
  _handle = glCreateProgram();
  int success;
  char infoLog[512];
  for (auto &[type, res] : sources) {
    uint32_t shader = glCreateShader((uint32_t)type);
    if (!shader) {
      throw std::runtime_error(
          fmt::format("Failed to create shader: 0x{:x}", glGetError()));
    }
    core::AutoPtr resource = res;
    auto data = resource->read();
    std::string str((char *)data->getData(), data->getSize());
    const char *s = str.c_str();
    glShaderSource(shader, 1, &s, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      throw std::runtime_error(
          fmt::format("Failed to compile shader:\n{}", infoLog));
    }
    glAttachShader(_handle, shader);
    shaders.push_back(shader);
  }
  glLinkProgram(_handle);
  glGetProgramiv(_handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_handle, 512, NULL, infoLog);
    throw std::runtime_error(
        fmt::format("Failed to link shader:\n{}", infoLog));
  }
  for (auto &shader : shaders) {
    glDeleteShader(shader);
  }
}
Shader::~Shader() { glDeleteProgram(_handle); }
void Shader::use() const { glUseProgram(_handle); }

void Shader::setValue(const std::string &name, const int32_t &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform1i(location, val);
}
void Shader::setValue(const std::string &name, const uint32_t &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform1ui(location, val);
}
void Shader::setValue(const std::string &name, const float &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform1f(location, val);
}
void Shader::setValue(const std::string &name, const glm::vec2 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform2fv(location, 1, &val[0]);
}
void Shader::setValue(const std::string &name, const float &x, const float &y) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform2f(location, x, y);
}
void Shader::setValue(const std::string &name, const glm::vec3 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform3fv(location, 1, &val[0]);
}

void Shader::setValue(const std::string &name, const float &x, const float &y,
                      const float &z) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform3f(location, x, y, z);
}
void Shader::setValue(const std::string &name, const glm::vec4 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform4fv(location, 1, &val[0]);
}
void Shader::setValue(const std::string &name, const float &x, const float &y,
                      const float &z, const float &w) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniform4f(location, x, y, z, w);
}
void Shader::setValue(const std::string &name, const glm::mat2 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniformMatrix2fv(location, 1, GL_FALSE, &val[0][0]);
}
void Shader::setValue(const std::string &name, const glm::mat3 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniformMatrix3fv(location, 1, GL_FALSE, &val[0][0]);
}
void Shader::setValue(const std::string &name, const glm::mat4 &val) {
  auto location = glGetUniformLocation(_handle, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
}