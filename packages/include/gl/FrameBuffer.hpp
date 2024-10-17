#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Rect.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace firefly::gl {
class FrameBuffer : public core::Object {
private:
  uint32_t _handle;
  uint32_t _renderBuffer;
  std::vector<core::AutoPtr<Texture2D>> _attachments;
  std::vector<uint32_t> _attachmentBuffers;
  core::Size<> _size;

public:
  FrameBuffer(const core::Size<> &size, uint32_t handle = 0);
  ~FrameBuffer() override;
  void bindAttachments(const std::vector<core::AutoPtr<Texture2D>> &textures);
  const std::vector<core::AutoPtr<Texture2D>> &getAttachments() const;
  bool check();
  uint32_t getHandle() const;
  const core::Size<> &getSize() const;
  const std::vector<uint32_t> &getAttachmentBuffers() const;

public:
  static void bind(const core::AutoPtr<FrameBuffer> &framebuffer);
};
} // namespace firefly::gl