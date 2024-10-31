#include "gl/Shader.hpp"
#include "gl/ShaderType.hpp"
using namespace firefly;
using namespace firefly::gl;
Shader::Shader(SHADER_TYPE type) { _handle = glCreateShader((GLenum)type); }
Shader::~Shader() { glDeleteShader(_handle); }
void Shader::setShaderSource(const core::String_t &source) {
  core::CString_t s = source.c_str();
  glShaderSource(_handle, 1, &s, NULL);
}
core::Boolean_t Shader::compile() {
  glCompileShader(_handle);
  int status;
  glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);
  return status;
}

const core::String_t Shader::getInfoLog() const {
  int size;
  glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &size);
  char *buf = new char[size];
  glGetShaderInfoLog(_handle, size, NULL, buf);
  core::String_t result = buf;
  delete[] buf;
  return result;
}
const core::Unsigned_t &Shader::getHandle() const { return _handle; }