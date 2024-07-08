#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/Shader.hpp"
#include "video/VertexArray.hpp"
#include <glm/glm.hpp>
#include <vector>
namespace firefly::video {
struct Face {
  int indices[3];
};
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
  Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data,
       const std::vector<Face> &faces,
       const Material &material = {.shininess = 0});
  Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data,
       const Material &material = {.shininess = 0});
  void draw(core::AutoPtr<Shader> &shader);
  const core::AutoPtr<VertexArray> getVertexArray() const;
  core::AutoPtr<VertexArray> getVertexArray();
};
} // namespace firefly::video