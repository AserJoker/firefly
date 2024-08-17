#include "gl/Program.hpp"
#include "core/AutoPtr.hpp"
#include "gl/OpenGLException.hpp"
#include "gl/Shader.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
Program::Program() {
  _handle = glCreateProgram();
  CHECK_OPENGL("Failed to create program");
}
Program::~Program() { glDeleteProgram(_handle); }
void Program::attach(core::AutoPtr<Shader> shader) {
  glAttachShader(_handle, shader->getHandle());
  CHECK_OPENGL("Failed to attach shader");
}
void Program::detach(core::AutoPtr<Shader> shader) {
  glDetachShader(_handle, shader->getHandle());
  CHECK_OPENGL("Failed to detach shader");
}
void Program::link() {
  glLinkProgram(_handle);
  CHECK_OPENGL("Failed to link program");
}
void Program::use() {
  glUseProgram(_handle);
  CHECK_OPENGL("Failed to use program");
}