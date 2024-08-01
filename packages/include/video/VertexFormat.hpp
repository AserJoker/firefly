#pragma once
#include <cstdint>
#include <glad/glad.h>
namespace firefly::video {
struct VertexFormat {
  int32_t type;
  uint32_t stride;
  uint32_t size;
  static VertexFormat Position3D;
  static VertexFormat Normalize3D;
  static VertexFormat Texture2D;
  static VertexFormat ColorRGBA;
};
inline VertexFormat VertexFormat::Position3D = {
    .type = GL_FLOAT, .stride = sizeof(float), .size = 3};
inline VertexFormat VertexFormat::Normalize3D = {
    .type = GL_FLOAT, .stride = sizeof(float), .size = 3};
inline VertexFormat VertexFormat::Texture2D = {
    .type = GL_FLOAT, .stride = sizeof(float), .size = 2};
inline VertexFormat VertexFormat::ColorRGBA = {
    .type = GL_FLOAT, .stride = sizeof(float), .size = 4};
} // namespace firefly::video