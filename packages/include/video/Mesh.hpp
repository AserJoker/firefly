#pragma once
#include "VertexFormat.hpp"
#include "core/Object.hpp"
#include <cstdint>
#include <vector>
namespace firefly::video {
class Mesh : public core::Object {
private:
  uint32_t _vao;
  uint32_t _vbo;
  uint32_t _ebo;
  uint32_t _elements;
  std::vector<VertexFormat> _vf;

public:
  Mesh(const std::vector<VertexFormat> &format);
  void setVertexData(void *data, const uint32_t &size);
  void setElementData(uint32_t *data, const uint32_t &size);
  void lockVertexBuffer(void **ppBuffer, uint32_t *nSize);
  void unlockVertexBuffer();
  void lockElementBuffer(void **ppBuffer, uint32_t *nSize);
  void unlockElementBuffer();
  ~Mesh() override;
  void draw();
};
}; // namespace firefly::video