#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/VertexArray.hpp"
#include <vector>
namespace firefly::video {
class Mesh : public core::Object {
private:
  core::AutoPtr<VertexArray> _vertex;

public:
  Mesh(const core::AutoPtr<VertexArray> &vertex);
  Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data,
       const std::vector<Face> &faces);
  Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data);
  void draw();
  const core::AutoPtr<VertexArray> getVertexArray() const;
  core::AutoPtr<VertexArray> getVertexArray();
};
} // namespace firefly::video