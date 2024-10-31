#include "gl/FrameBuffer.hpp"
#include "gl/Texture2D.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
FrameBuffer::FrameBuffer(const core::Size<> &size, core::Unsigned_t handle)
    : _handle(handle), _size(size) {
  if (!_handle) {
    glGenFramebuffers(1, &_handle);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  glGenRenderbuffers(1, &_renderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.width,
                        size.height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, _renderBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer() {
  glDeleteRenderbuffers(1, &_renderBuffer);
  glDeleteFramebuffers(1, &_handle);
}
void FrameBuffer::bindAttachments(
    const core::Array<core::AutoPtr<Texture2D>> &textures) {
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  auto index = 0;
  for (auto &texture : textures) {
    gl::Texture2D::bind(texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                           GL_TEXTURE_2D, texture->getHandle(), 0);
    _attachmentBuffers.pushBack(GL_COLOR_ATTACHMENT0 + index);
    index++;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  _attachments = textures;
}

const core::Array<core::AutoPtr<Texture2D>> &
FrameBuffer::getAttachments() const {
  return _attachments;
}
core::Boolean_t FrameBuffer::check() {
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  core::Boolean_t result =
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return result;
}
core::Unsigned_t FrameBuffer::getHandle() const { return _handle; }
void FrameBuffer::bind(const core::AutoPtr<FrameBuffer> &framebuffer) {
  if (!framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->_handle);
  }
}
const core::Size<> &FrameBuffer::getSize() const { return _size; }
const core::Array<core::Unsigned_t> &FrameBuffer::getAttachmentBuffers() const {
  return _attachmentBuffers;
}