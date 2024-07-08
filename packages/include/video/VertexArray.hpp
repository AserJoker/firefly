#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Buffer.hpp"
#include <map>
#include <vector>
namespace firefly::video {
enum class VertexValueType {
  BYTE = 0,
  UNSIGNED_BYTE,
  SHORT,
  UNSIGNED_SHORT,
  INT,
  UNSIGNED_INT,
  FLOAT
};
struct VertexAttribute {
  VertexValueType type;
  uint32_t size;
};
using VertexAttributeTable = std::vector<VertexAttribute>;

constexpr const VertexAttribute POSITION_XYZ = {VertexValueType::FLOAT, 3};
constexpr const VertexAttribute NORMAL_XYZ = {VertexValueType::FLOAT, 3};
constexpr const VertexAttribute TEXCROOD_UV = {VertexValueType::FLOAT, 2};
constexpr const VertexAttribute COLOR_RGB = {VertexValueType::FLOAT, 3};

class VertexArray : public core::Object {
private:
  std::map<BufferType, core::AutoPtr<Buffer>> _buffers;
  uint32_t _handle;
  VertexAttributeTable _attributes;

public:
  VertexArray(const VertexAttributeTable &attrs,
              const std::vector<core::AutoPtr<Buffer>> &buffers);
  ~VertexArray();
  void bind() const;
  void unbind() const;
  const VertexAttributeTable &getVertexAttributeTable() const;
  const core::AutoPtr<Buffer> getBuffer(const BufferType &type) const;
  core::AutoPtr<Buffer> getBuffer(const BufferType &type);
};
} // namespace firefly::video