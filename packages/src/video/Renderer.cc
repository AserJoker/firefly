#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
#include "gl/VertexArray.hpp"
#include "video/Attribute.hpp"
#include "video/Geometry.hpp"
#include "video/Object3D.hpp"
using namespace firefly;
using namespace firefly::video;

Renderer::Renderer() {}
void Renderer::syncToBackend(core::AutoPtr<Geometry> &geometry) {
  auto vao =
      geometry->getMetadata("gl::vertex_array_object").cast<gl::VertexArray>();
  if (!vao) {
    vao = new gl::VertexArray();
    geometry->setMetadata("gl::vertex_array_object", vao);
  }
  for (auto &[_, attr] : geometry->getAttributes()) {
    auto buf = attr->getMetadata("gl::buffer").cast<gl::Buffer>();
    if (!buf) {
      buf = new gl::Buffer(attr->getUsage());
      buf->setData(attr->getData().size(), attr->getData().data());
      attr->setMetadata("gl::buffer", buf);
      attr->clearUpdateRangeList();
    }
    if (buf->getVersion() != attr->getVersion()) {
      for (auto &[start, count] : attr->getUpdateRangeList()) {
        buf->write(start * attr->getDataTypeSize() * attr->getItemSize(),
                   count * attr->getDataTypeSize() * attr->getItemSize(),
                   attr->getData().data() + (start * attr->getDataTypeSize()));
      }
      attr->clearUpdateRangeList();
      buf->setVersion(attr->getVersion());
    }
  }
  auto indices = geometry->getIndices();
  auto buf = indices->getMetadata("gl::buffer").cast<gl::Buffer>();
  if (!buf) {
    buf = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
    buf->setData(indices->getData().size() * sizeof(uint32_t),
                 indices->getData().data());
    indices->setMetadata("gl::buffer", buf);
    indices->clearUpdateRangeList();
  }
  if (buf->getVersion() != indices->getVersion()) {
    for (auto &[start, count] : indices->getUpdateRangeList()) {
      buf->write(start * sizeof(uint32_t), count * sizeof(uint32_t),
                 indices->getData().data() + (start * sizeof(uint32_t)));
    }
    indices->clearUpdateRangeList();
    buf->setVersion(indices->getVersion());
  }
  if (vao->getVersion() != geometry->getVersion()) {
    gl::VertexArray::bind(vao);
    gl::Buffer::bind(gl::BUFFER_TARGET::ELEMENT_ARRAY, buf);
    for (auto &[name, attr] : geometry->getAttributes()) {
      auto buf = attr->getMetadata("gl::buffer").cast<gl::Buffer>();
      gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY, buf);
      vao->setAttribute(name, attr->getDataType(), attr->getItemSize(),
                        attr->isNormalized(),
                        attr->getItemSize() * attr->getDataTypeSize(), 0);
      vao->enableAttribute(name);
    }
    vao->setVersion(geometry->getVersion());
  }
}

void Renderer::renderGeometry(core::AutoPtr<Geometry> &geometry) {
  if (!geometry) {
    return;
  }
  syncToBackend(geometry);
  auto vao =
      geometry->getMetadata("gl::vertex_array_object").cast<gl::VertexArray>();
  gl::VertexArray::bind(vao);
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
void Renderer::renderGeometryInstance(core::AutoPtr<Geometry> &geometry,
                                      const uint32_t &count) {
  if (!geometry) {
    return;
  }
  syncToBackend(geometry);
  auto vao =
      geometry->getMetadata("gl::vertex_array_object").cast<gl::VertexArray>();
  gl::VertexArray::bind(vao);
  auto renderRanges = geometry->getRenderRanges();
  if (renderRanges.empty()) {
    glDrawElementsInstanced(GL_TRIANGLES,
                            geometry->getIndices()->getData().size(),
                            GL_UNSIGNED_INT, 0, count);
  } else {
    for (auto &[start, count] : renderRanges) {
      glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT,
                              (void *)(sizeof(uint32_t) * start), count);
    }
  }
}

void Renderer::setMatrial(const core::AutoPtr<Material> &mat) {
  
}
void Renderer::render(core::AutoPtr<Camera> camera,
                      core::AutoPtr<Object3D> root) {
  if (!root->isVisible()) {
    return;
  }

  if (!_shader) {
    return;
  }

  root->draw(this);
  for (auto &child : root->getChildren()) {
    child->draw(this);
  }
}

core::AutoPtr<gl::Shader> Renderer::getShader() { return _shader; }

void Renderer::setShader(const core::AutoPtr<gl::Shader> &shader) {
  if (_shader != shader) {
    _shader = shader;
    if (_shader != nullptr) {
      _shader->use();
    }
  }
}