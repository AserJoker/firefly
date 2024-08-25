#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/DataType.hpp"
namespace firefly::gl {
class VertexArray : public core::Object {
private:
  uint32_t _handle;

public:
  VertexArray(const uint32_t &handle = 0);
  ~VertexArray() override;
  void setAttribute(const uint32_t &index, DATA_TYPE dtype,
                    const uint32_t &size, const bool &normalized,
                    const uint32_t &stride, const uint32_t &offset);
  void enableAttribute(const uint32_t &index);
  void disableAttribute(const uint32_t &index);

public:
  static void bind(const core::AutoPtr<VertexArray> &vao);
};
} // namespace firefly::gl