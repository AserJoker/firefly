#include "video/RenderTarget.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "exception/Exception.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Program.hpp"
#include "gl/Texture2D.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include "video/Geometry.hpp"
#include <vector>
using namespace firefly;
using namespace firefly::video;
constexpr static const glm::vec2 quadVec[] = {{-1.0f, 1.0f}, {-1.0f, -1.0f},
                                              {1.0f, -1.0f}, {-1.0f, 1.0f},
                                              {1.0f, -1.0f}, {1.0f, 1.0f}};
constexpr static const glm::vec2 quadTex[] = {{0.0f, 1.0f}, {0.0f, 0.0f},
                                              {1.0f, 0.0f}, {0.0f, 1.0f},
                                              {1.0f, 0.0f}, {1.0f, 1.0f}};

constexpr static const uint32_t quadIndex[] = {0, 1, 2, 3, 4, 5};

core::AutoPtr<Geometry> RenderTarget::_geometry = nullptr;
RenderTarget::RenderTarget(const std::string &stage, const glm::ivec2 &size,
                           uint32_t attachment)
    : _stage(stage), _size(size) {
  _frame = new gl::FrameBuffer(size);
  std::vector<core::AutoPtr<gl::Texture2D>> textures;
  for (uint32_t i = 0; i < attachment; i++) {
    core::AutoPtr texture =
        new gl::Texture2D(size.x, size.y, gl::PIXEL_FORMAT::RGB);
    textures.push_back(texture);
  }
  _frame->bindAttachments(textures);
  if (!_frame->check()) {
    throw exception::RuntimeException<"Check Framebuffer">(
        "Failed to check framebuffer");
  }
  if (!_geometry) {
    _geometry = new Geometry();
    _geometry->setAttribute(
        0, new Attribute(new core::Buffer(sizeof(quadVec), quadVec),
                         typeid(float), 2));
    _geometry->setAttribute(
        1, new Attribute(new core::Buffer(sizeof(quadTex), quadTex),
                         typeid(float), 2));
    _geometry->setAttributeIndex(
        new AttributeIndex(new core::Buffer(sizeof(quadIndex), quadIndex)));
  }
}
void RenderTarget::active() {
  gl::FrameBuffer::bind(_frame);
  glViewport(0, 0, _size.x, _size.y);
  auto attachmentBuffers = _frame->getAttachmentBuffers();
  glDrawBuffers(attachmentBuffers.size(), attachmentBuffers.data());
}

const glm::ivec2 &RenderTarget::getSize() const { return _size; }

const std::string &RenderTarget::getStage() const { return _stage; }

const std::vector<core::AutoPtr<gl::Texture2D>> &
RenderTarget::getAttachments() const {
  return _frame->getAttachments();
}
const core::AutoPtr<gl::FrameBuffer> &RenderTarget::getFrameBuffer() const {
  return _frame;
}

void RenderTarget::draw(core::AutoPtr<gl::Program> &program) {
  auto attachments = _frame->getAttachments();
  for (size_t i = 0; i < attachments.size(); i++) {
    program->setUniform(fmt::format("attachment_{}", i), (int)i);
    glActiveTexture(GL_TEXTURE0 + (uint32_t)i);
    gl::Texture2D::bind(attachments[i]);
  }
  _geometry->draw(gl::DRAW_MODE::TRIANGLES);
}