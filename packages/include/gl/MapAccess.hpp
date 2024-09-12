#pragma once
#include <glad/glad.h>
namespace firefly::gl {
enum class MAP_ACCESS {
  READ_ONLY = GL_READ_ONLY,
  WRITE_ONLY = GL_WRITE_ONLY,
  READ_WRITE = GL_READ_WRITE
};
}