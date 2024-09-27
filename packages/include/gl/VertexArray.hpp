#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/DataType.hpp"

namespace firefly::gl {
class VertexArray : public core::Object {
private:
  uint32_t _handle;

public:
  VertexArray(uint32_t handle = 0);
  ~VertexArray() override;
  void setAttribute(uint32_t index, DATA_TYPE dtype, uint32_t size,
                    bool normalized, uint32_t stride, uint32_t offset);
  void enableAttribute(uint32_t index);
  void disableAttribute(uint32_t index);
  void setVertexAttribDivisor(uint32_t index, uint32_t divisor = 1);

public:
  static void bind(const core::AutoPtr<VertexArray> &vao);
};
} // namespace firefly::gl