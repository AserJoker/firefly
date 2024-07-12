#include "video/Mesh.hpp"
#include "core/AutoPtr.hpp"
#include "video/VBuffer.hpp"
#include "video/VertexArray.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::video;
Mesh::Mesh(const core::AutoPtr<VertexArray> &vertex, const Material &material)
    : _vertex(vertex), _material(material) {}
Mesh::Mesh(const VertexAttributeTable &attrs,
           const core::AutoPtr<VBuffer> &data,
           const core::AutoPtr<ElementBuffer> &elements,
           const Material &material)
    : _material(material) {
  _vertex = new VertexArray(attrs, {data, elements});
}
Mesh::Mesh(const VertexAttributeTable &attrs,
           const core::AutoPtr<VBuffer> &data, const Material &material)
    : _material(material) {
  _vertex = new VertexArray(attrs, {data});
}
void Mesh::draw(core::AutoPtr<Shader> &shader) const {
  _vertex->bind();
  auto indexbuffer = _vertex->getBuffer(VBufferType::ELEMENT_ARRAY);
  if (!indexbuffer) {
    auto vertexbuffer = _vertex->getBuffer(VBufferType::ARRAY);
    if (!vertexbuffer) {
      return;
    }
    glDrawArrays(GL_TRIANGLES, 0, vertexbuffer->getSize());
  } else {
    glDrawElements(GL_TRIANGLES, indexbuffer->getSize(), GL_UNSIGNED_INT, 0);
  }
}

const core::AutoPtr<VertexArray> Mesh::getVertexArray() const {
  return _vertex;
};
core::AutoPtr<VertexArray> Mesh::getVertexArray() { return _vertex; };

Material &Mesh::getMaterial() { return _material; }
const Material &Mesh::getMaterial() const { return _material; }