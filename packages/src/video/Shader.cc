#include "video/Shader.hpp"
#include <fmt/core.h>
#include <map>
#include <stdexcept>
#include <vector>
using namespace firefly;
using namespace firefly::video;
Shader::Shader(const std::map<ShaderType, std::string> &sources) : _handle(0) {
  std::vector<uint32_t> shaders;
  _handle = glCreateProgram();
  int success;
  char infoLog[512];
  for (auto &[type, source] : sources) {
    uint32_t shader = glCreateShader((uint32_t)type);
    if (!shader) {
      throw std::runtime_error(
          fmt::format("Failed to create shader: 0x{:x}", glGetError()));
    }
    const char *s = source.c_str();
    glShaderSource(shader, 1, &s, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      throw std::runtime_error(
          fmt::format("Failed to compile shader:\n\t{}", infoLog));
    }
    glAttachShader(_handle, shader);
    shaders.push_back(shader);
  }
  glLinkProgram(_handle);
  glGetProgramiv(_handle, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(_handle, 512, NULL, infoLog);
    throw std::runtime_error(
        fmt::format("Failed to link shader:\n\t{}", infoLog));
  }
  for (auto &shader : shaders) {
    glDeleteShader(shader);
  }
}
Shader::~Shader() { glDeleteProgram(_handle); }
void Shader::use() { glUseProgram(_handle); }