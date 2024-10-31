#include "video/RenderTarget.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Texture2D.hpp"
#include "video/FramebufferException.hpp"
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>

using namespace firefly;
using namespace firefly::video;

RenderTarget::RenderTarget(const core::String_t &stage,
                           const core::Size<> &size,
                           core::Unsigned_t attachment)
    : _size(size), _stage(stage) {
  _frame = new gl::FrameBuffer(size);
  core::Array<core::AutoPtr<gl::Texture2D>> textures;
  for (core::Unsigned_t i = 0; i < attachment; i++) {
    core::AutoPtr texture =
        new gl::Texture2D(size.width, size.height, gl::PIXEL_FORMAT::RGBA);
    textures.pushBack(texture);
  }
  _frame->bindAttachments(textures);
  if (!_frame->check()) {
    throw FramebufferException("Failed to check framebuffer");
  }
}

RenderTarget::RenderTarget(const core::Size<> &size,
                           core::Unsigned_t attachment)
    : RenderTarget("basic", size, attachment) {}

void RenderTarget::active() {
  gl::FrameBuffer::bind(_frame);
  auto attachmentBuffers = _frame->getAttachmentBuffers();
  glDrawBuffers(attachmentBuffers.size(), attachmentBuffers.data());
}

const core::Size<> &RenderTarget::getSize() const { return _size; }

const core::String_t &RenderTarget::getStage() const { return _stage; }

const core::Array<core::AutoPtr<gl::Texture2D>> &
RenderTarget::getAttachments() const {
  return _frame->getAttachments();
}

const core::AutoPtr<gl::FrameBuffer> &RenderTarget::getFrameBuffer() const {
  return _frame;
}

void RenderTarget::resize(const core::Size<> &size) {
  if (size.width <= _size.width && size.height <= _size.height) {
    return;
  }
  _size = size;
  auto attachment = _frame->getAttachments().size();
  _frame = new gl::FrameBuffer(size);
  core::Array<core::AutoPtr<gl::Texture2D>> textures;
  for (core::Unsigned_t i = 0; i < attachment; i++) {
    core::AutoPtr texture =
        new gl::Texture2D(size.width, size.height, gl::PIXEL_FORMAT::RGB);
    textures.pushBack(texture);
  }
  _frame->bindAttachments(textures);
  if (!_frame->check()) {
    throw FramebufferException("Failed to check framebuffer");
  }
}