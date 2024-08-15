#pragma once
#include <glad/glad.h>
namespace firefly::gl {
enum class ELEMENT_TYPE {
  UBYTE = GL_UNSIGNED_BYTE,
  USHORT = GL_UNSIGNED_SHORT,
  UINT = GL_UNSIGNED_INT
};
}