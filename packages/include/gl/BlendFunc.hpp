#pragma once
#include <glad/glad.h>
namespace firefly::gl {
enum class BLEND_FUNC {
  ZERO = GL_ZERO,
  ONE = GL_ONE,
  SRC_ALPHA = GL_SRC_ALPHA,
  DST_ALPHA = GL_DST_ALPHA,
  ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
  ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
};
};