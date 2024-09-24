#pragma once
#include <glad/glad.h>
namespace firefly::gl {
enum class ALPHA_FUNC {
  ALWAYS = GL_ALWAYS,
  NEVER = GL_NEVER,
  LESS = GL_LESS,
  LEQUAL = GL_LEQUAL,
  EQUAL = GL_EQUAL,
  GEQUAL = GL_GEQUAL,
  NOTEQUAL = GL_NOTEQUAL
};
}