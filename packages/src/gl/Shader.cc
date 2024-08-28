#include "gl/Shader.hpp"
#include "gl/ShaderType.hpp"
using namespace firefly;
using namespace firefly::gl;
Shader::Shader(SHADER_TYPE type) { _handle = glCreateShader((GLenum)type); }
Shader::~Shader() { glDeleteShader(_handle); }
void Shader::setShaderSource(const std::string &source) {
  const char *s = source.c_str();
  glShaderSource(_handle, 1, &s, NULL);
}
bool Shader::compile() {
  glCompileShader(_handle);
  int status;
  glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);
  return status;
}

const std::string Shader::getInfoLog() const {
  int size;
  glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &size);
  char *buf = new char[size];
  glGetShaderInfoLog(_handle, size, NULL, buf);
  std::string result = buf;
  delete[] buf;
  return result;
}
const uint32_t &Shader::getHandle() const { return _handle; }