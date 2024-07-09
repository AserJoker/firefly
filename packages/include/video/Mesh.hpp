#pragma once
#include "VertexBuffer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/ElementBuffer.hpp"
#include "video/Shader.hpp"
#include "video/VertexArray.hpp"
#include <glm/glm.hpp>
#include <initializer_list>
#include <vector>
namespace firefly::video {
struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};
class Mesh : public core::Object {
private:
  core::AutoPtr<VertexArray> _vertex;
  Material _material;

public:
  Mesh(const core::AutoPtr<VertexArray> &vertex,
       const Material &material = {.shininess = 0});
  Mesh(const VertexAttributeTable &attrs, const core::AutoPtr<Buffer> &data,
       const core::AutoPtr<ElementBuffer> &elements,
       const Material &material = {.shininess = 0});
  Mesh(const VertexAttributeTable &attrs, const core::AutoPtr<Buffer> &data,
       const Material &material = {.shininess = 0});
  template <class T>
  Mesh(const VertexAttributeTable &attrs, const std::vector<T> &data,
       const Material &materal = {.shininess = 0})
      : Mesh(attrs, new VertexBuffer<T>(data), materal) {}
  template <class T>
  Mesh(const VertexAttributeTable &attrs, const std::vector<T> &data,
       const std::vector<Face> &elements,
       const Material &materal = {.shininess = 0})
      : Mesh(attrs, new VertexBuffer<T>(data), new ElementBuffer(elements),
             materal) {}
  template <class T>
  Mesh(const VertexAttributeTable &attrs, const std::initializer_list<T> &data,
       const Material &materal = {.shininess = 0})
      : Mesh(attrs, new VertexBuffer<T>(data), materal) {}
  template <class T>
  Mesh(const VertexAttributeTable &attrs, const std::initializer_list<T> &data,
       const std::vector<Face> &elements,
       const Material &materal = {.shininess = 0})
      : Mesh(attrs, new VertexBuffer<T>(data), new ElementBuffer(elements),
             materal) {}
  void draw(core::AutoPtr<Shader> &shader);
  const core::AutoPtr<VertexArray> getVertexArray() const;
  core::AutoPtr<VertexArray> getVertexArray();
};
} // namespace firefly::video