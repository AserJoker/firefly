#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/DataType.hpp"

namespace firefly::gl {
class VertexArray : public core::Object {
private:
  core::Unsigned_t _handle;

public:
  VertexArray(core::Unsigned_t handle = 0);
  ~VertexArray() override;
  void setAttribute(core::Unsigned_t index, DATA_TYPE dtype,
                    core::Unsigned_t size, core::Boolean_t normalized,
                    core::Unsigned_t stride, core::Unsigned_t offset);
  void enableAttribute(core::Unsigned_t index);
  void disableAttribute(core::Unsigned_t index);
  void setVertexAttribDivisor(core::Unsigned_t index,
                              core::Unsigned_t divisor = 1);

public:
  static void bind(const core::AutoPtr<VertexArray> &vao);
};
} // namespace firefly::gl