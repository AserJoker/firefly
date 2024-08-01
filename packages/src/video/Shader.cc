#include "video/Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
using namespace firefly;
using namespace firefly::video;
Shader::Shader(const std::map<ShaderType, std::string> &sources) {
  _handle = glCreateProgram();
  for (auto &[type, source] : sources) {
    uint32_t h = glCreateShader((GLenum)type);
    const char *str = source.c_str();
    glShaderSource(h, 1, &str, NULL);
    glCompileShader(h);
    int res;
    glGetShaderiv(h, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
      int len;
      glGetShaderiv(h, GL_INFO_LOG_LENGTH, &len);
      char *buf = (char *)::operator new(len + 1);
      glGetShaderInfoLog(h, len, 0, buf);
      std::string error = buf;
      delete[] buf;
      throw std::runtime_error(error);
    }
    glAttachShader(_handle, h);
  }
  glLinkProgram(_handle);
  int res;
  glGetProgramiv(_handle, GL_LINK_STATUS, &res);
  if (res == GL_FALSE) {
    int len;
    glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &len);
    char *buf = (char *)::operator new(len + 1);
    glGetProgramInfoLog(_handle, len, 0, buf);
    std::string error = buf;
    delete[] buf;
    throw std::runtime_error(error);
  }
}
void Shader::enable() { glUseProgram(_handle); }
Shader::~Shader() { glDeleteProgram(_handle); }
void Shader::setUniform(const std::string &name, float value) {
  glUniform1f(glGetUniformLocation(_handle, name.c_str()), value);
}
void Shader::setUniform(const std::string &name, int value) {
  glUniform1d(glGetUniformLocation(_handle, name.c_str()), value);
}
void Shader::setUniform(const std::string &name, bool value) {
  glUniform1d(glGetUniformLocation(_handle, name.c_str()), value);
}
void Shader::setUniform(const std::string &name, const glm::vec2 &value) {
  glUniform2fv(glGetUniformLocation(_handle, name.c_str()), 1,
               glm::value_ptr(value));
}
void Shader::setUniform(const std::string &name, const glm::vec3 &value) {
  glUniform3fv(glGetUniformLocation(_handle, name.c_str()), 1,
               glm::value_ptr(value));
}
void Shader::setUniform(const std::string &name, const glm::vec4 &value) {
  glUniform4fv(glGetUniformLocation(_handle, name.c_str()), 1,
               glm::value_ptr(value));
}
void Shader::setUniform(const std::string &name, const glm::mat3x3 &value) {
  glUniformMatrix3fv(glGetUniformLocation(_handle, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}
void Shader::setUniform(const std::string &name, const glm::mat4x4 &value) {
  glUniformMatrix4fv(glGetUniformLocation(_handle, name.c_str()), 1, GL_FALSE,
                     &value[0][0]);
}