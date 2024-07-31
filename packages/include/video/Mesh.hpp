#pragma once
#include "core/Object.hpp"
#include <cstdint>
namespace firefly::video {
class Mesh : public core::Object {
private:
  uint32_t _vertexArray;
  uint32_t _vertexBuffer;
  uint32_t _elementBuffer;
  uint32_t _elements;

public:
  Mesh();
  ~Mesh() override;
  void draw();
};
}; // namespace firefly::video