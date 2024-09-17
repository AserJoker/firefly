#include "gl/FrameBuffer.hpp"
#include "core/Singleton.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/Application.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::gl;
FrameBuffer::FrameBuffer(uint32_t handle) : _handle(handle) {
  if (!_handle) {
    glGenFramebuffers(1, &_handle);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer() {
  glDeleteRenderbuffers(1, &_renderbuffer);
  glDeleteFramebuffers(1, &_handle);
}
void FrameBuffer::bindAttachments(
    const std::vector<core::AutoPtr<Texture2D>> &textures) {
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  auto index = 0;
  std::vector<uint32_t> attachments;
  for (auto &texture : textures) {
    gl::Texture2D::bind(texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                           GL_TEXTURE_2D, texture->getHandle(), 0);
    attachments.push_back(GL_COLOR_ATTACHMENT0 + index);
    index++;
  }
  if (attachments.size() > 1) {
    glDrawBuffers(textures.size(), attachments.data());
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool FrameBuffer::check() {
  glBindFramebuffer(GL_FRAMEBUFFER, _handle);
  bool result =
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return result;
}
uint32_t FrameBuffer::getHandle() const { return _handle; }
void FrameBuffer::bind(const core::AutoPtr<FrameBuffer> &framebuffer) {
  if (!framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->_handle);
  }
}