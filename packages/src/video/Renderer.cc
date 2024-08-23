#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/BufferUsage.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Object3D.hpp"
#include "video/Shader.hpp"
using namespace firefly;
using namespace firefly::video;

void Renderer::setShader(const core::AutoPtr<Shader> &shader) {
  _shader = shader;
  _shader->use();
}

void Renderer::syncToBackend(core::AutoPtr<Geometry> &geometry) {
  if (!_vaos.contains(geometry->getIdentity())) {
    RenderObject obj;
    glGenVertexArrays(1, &obj.vao);
    obj.version = 0;
    obj.ibuffer.version = 0;
    obj.ibuffer.size = 0;
    glGenBuffers(1, &obj.ibuffer.buffer);
    glBindVertexArray(obj.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibuffer.buffer);
    _vaos[geometry->getIdentity()] = obj;
    geometry->defer([this](const std::string &identity) -> void {
      if (_vaos.contains(identity)) {
        auto &vao = _vaos.at(identity);
        for (auto &[_, buf] : vao.buffers) {
          glDeleteBuffers(1, &buf.buffer);
        }
        glDeleteBuffers(1, &vao.ibuffer.buffer);
        glDeleteVertexArrays(1, &vao.vao);
        _vaos.erase(identity);
      }
    });
  }
  RenderObject &obj = _vaos.at(geometry->getIdentity());
  auto &indices = geometry->getIndices();
  if (obj.ibuffer.version != geometry->getIndices()->getVersion()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibuffer.buffer);
    if (obj.ibuffer.size != indices->getData().size()) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   indices->getData().size() * sizeof(uint32_t),
                   indices->getData().data(), GL_STATIC_DRAW);
      obj.ibuffer.size = indices->getData().size();
    } else {
      for (auto &[start, count] : indices->getUpdateRangeList()) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, start * sizeof(uint32_t),
                        count * sizeof(uint32_t),
                        indices->getData().data() + start);
      }
    }
    obj.ibuffer.version = indices->getVersion();
  }
  auto buffers = obj.buffers;
  obj.buffers.clear();
  for (auto &[_, attr] : geometry->getAttributes()) {
    if (!buffers.contains(attr->getIdentity())) {
      RenderBuffer buf;
      buf.size = attr->getData().size();
      buf.version = attr->getVersion();
      glGenBuffers(1, &buf.buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buf.buffer);
      glBufferData(GL_ARRAY_BUFFER, attr->getData().size() * sizeof(float),
                   attr->getData().data(), (GLenum)attr->getUsage());
      attr->clearUpdateRangeList();
      buffers[attr->getIdentity()] = buf;
    }
    auto &buf = buffers[attr->getIdentity()];
    if (buf.version != attr->getVersion()) {
      glBindBuffer(GL_ARRAY_BUFFER, buf.buffer);
      if (buf.size != attr->getData().size()) {
        glBufferData(GL_ARRAY_BUFFER, attr->getData().size() * sizeof(float),
                     attr->getData().data(), (GLenum)attr->getUsage());
        buf.size = attr->getData().size();
      } else {
        for (auto &[start, count] : attr->getUpdateRangeList()) {
          glBufferSubData(GL_ARRAY_BUFFER,
                          start * attr->getItemSize() * sizeof(float),
                          count * attr->getItemSize() * sizeof(float),
                          attr->getData().data() + start);
        }
      }
      attr->clearUpdateRangeList();
      buf.version = attr->getVersion();
    }
    obj.buffers[attr->getIdentity()] = buf;
    buf.buffer = 0;
  }
  for (auto &[_, buf] : buffers) {
    if (buf.buffer != 0) {
      glDeleteBuffers(1, &buf.buffer);
      buf.buffer = 0;
    }
  }
  if (obj.version != geometry->getVersion()) {
    glBindVertexArray(obj.vao);
    for (auto &[index, attr] : geometry->getAttributes()) {
      glBindBuffer(GL_ARRAY_BUFFER, obj.buffers[attr->getIdentity()].buffer);
      glVertexAttribPointer(index, attr->getItemSize(),
                            (GLenum)attr->getDataType(), attr->isNormalized(),
                            attr->getItemSize() * sizeof(float), 0);
      glEnableVertexAttribArray(index);
    }
    obj.version = geometry->getVersion();
  }
}

void Renderer::renderGeometry(core::AutoPtr<Geometry> &geometry) {
  if (!geometry) {
    return;
  }
  syncToBackend(geometry);
  auto vao = _vaos.at(geometry->getIdentity()).vao;
  glBindVertexArray(vao);
  auto renderRanges = geometry->getRenderRanges();
  if (renderRanges.empty()) {
    glDrawElements(GL_TRIANGLES, geometry->getIndices()->getData().size(),
                   GL_UNSIGNED_INT, 0);
  } else {
    for (auto &[start, count] : renderRanges) {
      glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT,
                     (void *)(sizeof(uint32_t) * start));
    }
  }
}

void Renderer::render(core::AutoPtr<Object3D> root) {
  if (!root->isVisible()) {
    return;
  }
  root->draw(this);
  for (auto &child : root->getChildren()) {
    child->draw(this);
  }
}