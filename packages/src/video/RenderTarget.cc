#include "video/RenderTarget.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "exception/Exception.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Program.hpp"
#include "gl/Texture2D.hpp"
#include "video/Attribute.hpp"
#include "video/AttributeIndex.hpp"
#include "video/Geometry.hpp"
#include "video/Renderer.hpp"
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
#include <vector>
using namespace firefly;
using namespace firefly::video;
constexpr static const float quadVec[] = {-1.0f, 1.0f,  -1.0f, -1.0f,
                                          1.0f,  -1.0f, -1.0f, 1.0f,
                                          1.0f,  -1.0f, 1.0f,  1.0f};
constexpr static const float quadTex[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                                          0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

constexpr static const uint32_t quadIndex[] = {0, 1, 2, 3, 4, 5};

core::AutoPtr<Geometry> RenderTarget::_geometry = nullptr;
RenderTarget::RenderTarget(const std::string &stage, const glm::ivec2 &size,
                           uint32_t attachment)
    : _stage(stage), _size(size) {
  _frame = new gl::FrameBuffer(size);
  std::vector<core::AutoPtr<gl::Texture2D>> textures;
  for (uint32_t i = 0; i < attachment; i++) {
    core::AutoPtr texture =
        new gl::Texture2D(size.x, size.y, gl::PIXEL_FORMAT::RGBA);
    textures.push_back(texture);
  }
  _frame->bindAttachments(textures);
  if (!_frame->check()) {
    throw exception::RuntimeException<"Check Framebuffer">(
        "Failed to check framebuffer");
  }
  if (!_geometry) {
    _geometry = new Geometry();
    _geometry->setAttribute(0, new Attribute(quadVec, 2));
    _geometry->setAttribute(3, new Attribute(quadTex, 2));
    _geometry->setAttributeIndex(new AttributeIndex(quadIndex));
  }
}
RenderTarget::RenderTarget(const glm::ivec2 &size, uint32_t attachment)
    : RenderTarget("basic", size, attachment) {}
void RenderTarget::active() {
  gl::FrameBuffer::bind(_frame);
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

void RenderTarget::draw(core::AutoPtr<gl::Program> program) {
  auto attachments = _frame->getAttachments();
  for (size_t i = 0; i < attachments.size(); i++) {
    program->setUniform(fmt::format("attachment_{}", i), (int)i);
    glActiveTexture(GL_TEXTURE0 + (uint32_t)i);
    gl::Texture2D::bind(attachments[i]);
  }
  _geometry->draw(gl::DRAW_MODE::TRIANGLES);
}
void RenderTarget::resize(const glm::ivec2 &size) {
  if (size.x <= _size.x && size.y <= _size.y) {
    return;
  }
  _size = size;
  auto attachment = _frame->getAttachments().size();
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
}
void RenderTarget::onTick() {
  active();
  glClearColor(0, 0, 0, 0);
  auto renderer = core::Singleton<Renderer>::instance();
  auto context = renderer->pushContext();
  Node::onTick();
  renderer->popContext(context);
  gl::FrameBuffer::bind(nullptr);
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
  // auto texture = _frame->getAttachments()[0];
  // auto size = texture->getSize();
  // void *buffer = ::operator new(size.x * size.y * 4);
  // gl::Texture2D::bind(_frame->getAttachments()[0]);
  // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  // auto surface = SDL_CreateRGBSurfaceWithFormatFrom(
  //     buffer, size.x, size.y, 4, 4 * size.x, SDL_PIXELFORMAT_RGBA32);
  // IMG_SavePNG(surface, "1.png");
  // exit(0);
}