#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/BufferUsage.hpp"
#include "video/Geometry.hpp"
#include "video/Shader.hpp"
using namespace firefly;
using namespace firefly::video;
void Renderer::setShader(const core::AutoPtr<Shader> &shader) {
  _shader = shader;
  _shader->use();
}
void Renderer::render(core::AutoPtr<Geometry> geometry) {
  auto &indices = geometry->getIndices();
  auto &attributes = geometry->getAttributes();
  if (!geometry->hasAttribute("position")) {
    return;
  }
  auto index = 0;
  if (!_vaos.contains(geometry->getIdentity())) {
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    uint32_t indexBuffer = 0;
    if (_buffers.contains(geometry->getIndices()->getIdentity())) {
      indexBuffer = _buffers.at(geometry->getIndices()->getIdentity());
    } else {
      glGenBuffers(1, &indexBuffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   indices->getData().size() * sizeof(uint32_t),
                   indices->getData().data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBindVertexArray(0);
    _vaos[geometry->getIdentity()] = vao;
  }
  auto vao = _vaos[geometry->getIdentity()];
  glBindVertexArray(vao);
  for (auto &[name, attr] : attributes) {
    if (!_buffers.contains(attr->getIdentity())) {
      uint32_t buffer = 0;
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, attr->getData().size() * sizeof(float),
                   attr->getData().data(), (GLenum)attr->getUsage());
      attr->clearUpdateRangeList();
      _buffers[attr->getIdentity()] = buffer;
    }
    if (!attr->getUpdateRangeList().empty()) {
      auto buffer = _buffers.at(attr->getIdentity());
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      for (auto &[start, count] : attr->getUpdateRangeList()) {
        glBufferSubData(GL_ARRAY_BUFFER, start * attr->getItemSize(),
                        count * attr->getItemSize() * sizeof(float),
                        attr->getData().data() + start);
      }
      attr->clearUpdateRangeList();
    }
  }
  if (geometry->isNeedUpdate()) {
    for (auto &[name, attr] : attributes) {
      auto buffer = _buffers.at(attr->getIdentity());
      auto shader = _shader->getHandle();
      auto index = glGetAttribLocation(shader, name.c_str());
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glVertexAttribPointer(index, attr->getItemSize(),
                            (GLenum)attr->getDataType(), attr->isNormalized(),
                            attr->getItemSize() * sizeof(float), 0);
      glEnableVertexAttribArray(index);
    }
    geometry->setNeedUpdate(false);
  }
  glDrawElements(GL_TRIANGLES, indices->getData().size(), GL_UNSIGNED_INT, 0);
}