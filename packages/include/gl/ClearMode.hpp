#pragma once
#include <glad/glad.h>
namespace firefly::gl {
enum CLEAR_MODE {
  COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT,
  DEPTH_BUFFER_BIT = GL_DEPTH_BUFFER_BIT,
  STECIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT
};
};