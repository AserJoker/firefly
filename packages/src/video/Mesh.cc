#include "video/Mesh.hpp"
#include "core/AutoPtr.hpp"
#include "video/Buffer.hpp"
#include "video/VertexArray.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::video;
Mesh::Mesh(const core::AutoPtr<VertexArray> &vertex) : _vertex(vertex) {}
Mesh::Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data,
           const std::vector<Face> &faces) {
  auto vertex = new Buffer(data.size() * sizeof(float), (void *)data.data());
  auto indices =
      new Buffer(faces.size() * sizeof(Face), (void *)faces.data(),
                 BufferType::ELEMENT_ARRAY, BufferUsage::STATIC_DRAW);
  _vertex = new VertexArray(attrs, {vertex, indices});
}
Mesh::Mesh(const VertexAttributeTable &attrs, const std::vector<float> &data) {
  auto vertex = new Buffer(data.size() * sizeof(float), (void *)data.data());
  _vertex = new VertexArray(attrs, {vertex});
}
void Mesh::draw() {
  _vertex->bind();
  auto indexbuffer = _vertex->getBuffer(BufferType::ELEMENT_ARRAY);
  if (!indexbuffer) {
    auto vertexbuffer = _vertex->getBuffer(BufferType::ARRAY);
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