#include "video/Mesh.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;
Mesh::Mesh() {
  glGenVertexArrays(1, &_vertexArray);
  glGenBuffers(1, &_vertexBuffer);
  glGenBuffers(1, &_elementBuffer);
  _elements = 0;
}
Mesh::~Mesh() {
  glDeleteBuffers(1, &_elementBuffer);
  glDeleteBuffers(1, &_vertexBuffer);
  glDeleteVertexArrays(1, &_vertexArray);
}
void Mesh::draw() {
  glBindVertexArray(_vertexArray);
  glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
}