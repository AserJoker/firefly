#include "gl/Shader.hpp"
#include "gl/OpenGLException.hpp"
#include "gl/ShaderType.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Shader::Shader(SHADER_TYPE type) { _handle = glCreateShader((GLenum)type); }
Shader::~Shader() { glDeleteShader(_handle); }
void Shader::setSource(const std::string &source) {
  const char *str = source.c_str();
  glShaderSource(_handle, 1, &str, 0);
  CHECK_OPENGL("Failed set shader source");
}
void Shader::compile() {
  glCompileShader(_handle);
  CHECK_OPENGL("Failed compile shader");
}
std::string Shader::getSource() {
  int32_t length = 0;
  glGetShaderiv(_handle, GL_SHADER_SOURCE_LENGTH, &length);
  char *buf = new char[length + 1];
  glGetShaderSource(_handle, length + 1, &length, buf);
  std::string result = buf;
  delete[] buf;
  return result;
}
std::string Shader::getInfoLog() {
  int32_t len = 0;
  glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &len);
  char *buf = new char[len + 1];
  glGetShaderInfoLog(_handle, len + 1, &len, buf);
  std::string result = buf;
  delete[] buf;
  return result;
}
SHADER_TYPE Shader::getType() {
  int32_t type;
  glGetShaderiv(_handle, GL_SHADER_TYPE, &type);
  return (SHADER_TYPE)type;
}
bool Shader::isDeleted() {
  int32_t status;
  glGetShaderiv(_handle, GL_DELETE_STATUS, &status);
  return status;
}
bool Shader::isCompileSuccess() {
  int32_t status;
  glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);
  return status;
}
uint32_t Shader::getHandle() { return _handle; }