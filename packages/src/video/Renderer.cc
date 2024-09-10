#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Buffer.hpp"
#include "gl/BufferTarget.hpp"
#include "gl/BufferUsage.hpp"
#include "gl/DataType.hpp"
#include "gl/Program.hpp"
#include "gl/Texture2D.hpp"
#include "gl/TextureWrapMode.hpp"
#include "gl/VertexArray.hpp"
#include "video/Camera.hpp"
#include "video/Constant.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/RenderObject.hpp"
#include "video/UpdateRangeList.hpp"
using namespace firefly;
using namespace firefly::video;
Renderer::Renderer() { _constants = new Constant(); }
void Renderer::drawGeomeory(const core::AutoPtr<Geometry> &geometry) {
  auto &attributes = geometry->getAttributes();
  auto &indices = geometry->getIndices();
  for (auto &[_, attribute] : attributes) {
    auto glBuffer = attribute->getMetadata("gl::buffer").cast<gl::Buffer>();
    if (!glBuffer) {
      glBuffer = new gl::Buffer(attribute->isDynamic()
                                    ? gl::BUFFER_USAGE::DYNAMIC_DRAW
                                    : gl::BUFFER_USAGE::STATIC_DRAW);
      glBuffer->setData((uint32_t)attribute->getBuffer()->getSize(),
                        attribute->getBuffer()->getData());
      attribute->setMetadata("gl::buffer", glBuffer);
      glBuffer->setVersion(glBuffer->getVersion());
      auto updateRangeList = attribute->getMetadata("video::update_range_list")
                                 .cast<UpdateRangeList>();
      updateRangeList->clear();
    }
    if (glBuffer->getVersion() != attribute->getVersion()) {
      auto updateRangeList = attribute->getMetadata("video::update_range_list")
                                 .cast<UpdateRangeList>();
      for (auto &[start, size] : updateRangeList->getRanges()) {
        glBuffer->write(start, size,
                        (char *)attribute->getBuffer()->getData() + start);
      }
      updateRangeList->clear();
      glBuffer->setVersion(attribute->getVersion());
    }
  }
  auto glBuffer = indices->getMetadata("gl::buffer").cast<gl::Buffer>();
  if (!glBuffer) {
    glBuffer = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
    glBuffer->setData(indices->getIndicesCount() * sizeof(uint32_t),
                      indices->getIndices());
    indices->setMetadata("gl::buffer", glBuffer);
  }
  if (glBuffer->getVersion() != indices->getVersion()) {
    glBuffer->write(0, indices->getIndicesCount() * sizeof(uint32_t),
                    indices->getIndices());
    glBuffer->setVersion(indices->getVersion());
  }
  auto glVertexArray =
      geometry->getMetadata("gl::vertex_array").cast<gl::VertexArray>();
  if (!glVertexArray) {
    glVertexArray = new gl::VertexArray();
    geometry->setMetadata("gl::vertex_array", glVertexArray);
  }
  if (geometry->getVersion() != glVertexArray->getVersion()) {
    gl::VertexArray::bind(glVertexArray);
    gl::Buffer::bind(gl::BUFFER_TARGET::ELEMENT_ARRAY, glBuffer);
    for (auto &[index, attribute] : attributes) {
      auto glBuffer = attribute->getMetadata("gl::buffer").cast<gl::Buffer>();
      gl::Buffer::bind(gl::BUFFER_TARGET::ARRAY, glBuffer);
      gl::DATA_TYPE dtype;
      auto &type = attribute->getItemType();
      if (type == typeid(float).name()) {
        dtype = gl::DATA_TYPE::FLOAT;
      } else if (type == typeid(int32_t).name()) {
        dtype = gl::DATA_TYPE::INT;
      } else if (type == typeid(uint32_t).name()) {
        dtype = gl::DATA_TYPE::UNSIGNED_INT;
      } else if (type == typeid(double).name()) {
        dtype = gl::DATA_TYPE::DOUBLE;
      } else if (type == typeid(int8_t).name()) {
        dtype = gl::DATA_TYPE::BTYE;
      } else if (type == typeid(uint8_t).name()) {
        dtype = gl::DATA_TYPE::UNSIGNED_BYTE;
      } else if (type == typeid(int16_t).name()) {
        dtype = gl::DATA_TYPE::SHORT;
      } else if (type == typeid(uint16_t).name()) {
        dtype = gl::DATA_TYPE::UNSIGNED_SHORT;
      }
      glVertexArray->setAttribute(index, dtype, attribute->getItemSize(),
                                  attribute->isNormalized(),
                                  attribute->getStride(), 0);
      glVertexArray->enableAttribute(index);
    }
    glVertexArray->setVersion(geometry->getVersion());
  }

  gl::VertexArray::bind(glVertexArray);
  glDrawElements(GL_TRIANGLES, indices->getIndicesCount(), GL_UNSIGNED_INT, 0);
}

void Renderer::setShader(const std::string &name) {
  auto shader = gl::Program::get(name, fmt::format("shader::{}", name));
  if (shader != _shader) {
    _shader = shader;
    _shader->use();
    syncConstats(true);
  }
}

void Renderer::setMaterial(const core::AutoPtr<Material> &material) {
  material->active(_constants);
  auto textures = material->getTextures();
  auto index = 0;
  for (auto &[_, texture] : textures) {
    auto path = fmt::format("texture::{}", texture.path);
    auto tex = gl::Texture2D::get(path, path, texture.mappingmodeU,
                                  texture.mappingmodeV);
    glActiveTexture(GL_TEXTURE0 + index++);
    gl::Texture2D::bind(tex);
  }

  if (material->isDepthTest()) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  if (material->isStencilTest()) {
    glEnable(GL_STENCIL_TEST);
  } else {
    glDisable(GL_STENCIL_TEST);
  }

  if (material->isAlphaTest()) {
    glEnable(GL_ALPHA_TEST);
  } else {
    glDisable(GL_ALPHA_TEST);
  }

  if (material->isWireframe()) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

const core::AutoPtr<Constant> &Renderer::getConstants() const {
  return _constants;
}

core::AutoPtr<Constant> &Renderer::getConstants() { return _constants; }

void Renderer::draw(const core::AutoPtr<Material> &material,
                    const core::AutoPtr<Geometry> &geometry) {
  if (!material->isBlend()) {
    _normalRenderList.push_back(new RenderObject(geometry, material));
  } else {
    _blendRenderList.push_back(new RenderObject(geometry, material));
  }
}

void Renderer::draw(const core::AutoPtr<Model> &model) {
  if (model->getMaterial()->isVisible()) {
    draw(model->getMaterial(), model->getGeometry());
  }
}

void Renderer::draw(const core::AutoPtr<ModelSet> &modelset) {
  for (auto &[_, model] : modelset->getModels()) {
    draw(model);
  }
  for (auto &[_, child] : modelset->getChildren()) {
    draw(child);
  }
}

void Renderer::begin(const core::AutoPtr<Camera> &camera) {
  _constants->setField("projection", camera->getProjectionMatrix());
  _constants->setField("view", camera->getViewMatrix());
}

void Renderer::syncConstats(bool force = false) {
  if (_constants != nullptr) {
    for (auto &[name, field] : _constants->getFields()) {
      auto type = _constants->getFieldType(name);
      switch (type) {
      case CONSTANT_TYPE::BOOL:
        _shader->setUniform(name, std::any_cast<bool>(field));
        break;
      case CONSTANT_TYPE::INT:
        _shader->setUniform(name, std::any_cast<int>(field));
        break;
      case CONSTANT_TYPE::IVEC2:
        _shader->setUniform(name, std::any_cast<glm::ivec2>(field));
        break;
      case CONSTANT_TYPE::IVEC3:
        _shader->setUniform(name, std::any_cast<glm::ivec3>(field));
        break;
      case CONSTANT_TYPE::IVEC4:
        _shader->setUniform(name, std::any_cast<glm::ivec4>(field));
        break;
      case CONSTANT_TYPE::UINT:
        _shader->setUniform(name, std::any_cast<uint32_t>(field));
        break;
      case CONSTANT_TYPE::UIVEC2:
        _shader->setUniform(name, std::any_cast<glm::uvec2>(field));
        break;
      case CONSTANT_TYPE::UIVEC3:
        _shader->setUniform(name, std::any_cast<glm::uvec3>(field));
        break;
      case CONSTANT_TYPE::UIVEC4:
        _shader->setUniform(name, std::any_cast<glm::uvec4>(field));
        break;
      case CONSTANT_TYPE::FLOAT:
        _shader->setUniform(name, std::any_cast<float>(field));
        break;
      case CONSTANT_TYPE::VEC2:
        _shader->setUniform(name, std::any_cast<glm::vec2>(field));
        break;
      case CONSTANT_TYPE::VEC3:
        _shader->setUniform(name, std::any_cast<glm::vec3>(field));
        break;
      case CONSTANT_TYPE::VEC4:
        _shader->setUniform(name, std::any_cast<glm::vec4>(field));
        break;
      case CONSTANT_TYPE::DOUBLE:
        _shader->setUniform(name, std::any_cast<double>(field));
        break;
      case CONSTANT_TYPE::DVEC2:
        _shader->setUniform(name, std::any_cast<glm::dvec2>(field));
        break;
      case CONSTANT_TYPE::DVEC3:
        _shader->setUniform(name, std::any_cast<glm::dvec3>(field));
        break;
      case CONSTANT_TYPE::DVEC4:
        _shader->setUniform(name, std::any_cast<glm::dvec4>(field));
        break;
      case CONSTANT_TYPE::MATRIX2:
        _shader->setUniform(name, std::any_cast<glm::mat2>(field));
        break;
      case CONSTANT_TYPE::MATRIX2x3:
        _shader->setUniform(name, std::any_cast<glm::mat2x3>(field));
        break;
      case CONSTANT_TYPE::MATRIX3x2:
        _shader->setUniform(name, std::any_cast<glm::mat3x2>(field));
        break;
      case CONSTANT_TYPE::MATRIX3:
        _shader->setUniform(name, std::any_cast<glm::mat3>(field));
        break;
      case CONSTANT_TYPE::MATRIX3x4:
        _shader->setUniform(name, std::any_cast<glm::mat3x4>(field));
        break;
      case CONSTANT_TYPE::MATRIX4x3:
        _shader->setUniform(name, std::any_cast<glm::mat4x3>(field));
        break;
      case CONSTANT_TYPE::MATRIX4:
        _shader->setUniform(name, std::any_cast<glm::mat4>(field));
        break;
      case CONSTANT_TYPE::BLOCK: {
        auto block = std::any_cast<core::AutoPtr<ConstantBlock>>(field);
        auto bindingName =
            fmt::format("video::uniform_block_binding_{}", block->getIndex());
        auto binding = _shader->getMetadata(bindingName);
        if (binding != block) {
          _shader->bindUniformBlock(name, block->getIndex());
          _shader->setMetadata(bindingName, block);
        }

        auto buf = block->getMetadata("gl::buffer").cast<gl::Buffer>();
        if (!buf) {
          buf = new gl::Buffer(gl::BUFFER_USAGE::STATIC_DRAW);
          buf->setData((uint32_t)block->getBuffer()->getSize(),
                       block->getBuffer()->getData());
          gl::Buffer::bindBase(gl::BUFFER_TARGET::UNIFORM, block->getIndex(),
                               buf);
          block->setMetadata("gl::buffer", buf);
        }
        if (buf->getVersion() != block->getVersion()) {
          auto range = block->getMetadata("video::update_range_list")
                           .cast<UpdateRangeList>();
          for (auto &[start, count] : range->getRanges()) {
            buf->write(start, count,
                       (char *)block->getBuffer()->getData() + start);
          }
          range->clear();
          buf->setVersion(block->getVersion());
        }
      } break;
      }
    }
  }
}
void Renderer::end() {
  if (!_shader) {
    setShader("standard");
  }
  syncConstats();
  glDisable(GL_BLEND);
  for (auto &item : _normalRenderList) {
    setMaterial(item->getMeterial());
    drawGeomeory(item->getGeometry());
  }
  glEnable(GL_BLEND);
  for (auto &item : _blendRenderList) {
    setMaterial(item->getMeterial());
    drawGeomeory(item->getGeometry());
  }
  _normalRenderList.clear();
  _blendRenderList.clear();
}