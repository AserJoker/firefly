#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>


namespace firefly::gl {
class FrameBuffer : public core::Object {
private:
  core::Unsigned_t _handle;
  core::Unsigned_t _renderBuffer;
  core::Array<core::AutoPtr<Texture2D>> _attachments;
  core::Array<core::Unsigned_t> _attachmentBuffers;
  core::Size<> _size;

public:
  FrameBuffer(const core::Size<> &size, core::Unsigned_t handle = 0);
  ~FrameBuffer() override;
  void bindAttachments(const core::Array<core::AutoPtr<Texture2D>> &textures);
  const core::Array<core::AutoPtr<Texture2D>> &getAttachments() const;
  core::Boolean_t check();
  core::Unsigned_t getHandle() const;
  const core::Size<> &getSize() const;
  const core::Array<core::Unsigned_t> &getAttachmentBuffers() const;

public:
  static void bind(const core::AutoPtr<FrameBuffer> &framebuffer);
};
} // namespace firefly::gl