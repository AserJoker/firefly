#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/Texture2D.hpp"
#include <vector>
namespace firefly::gl {
class FrameBuffer : public core::Object {
private:
  uint32_t _handle;
  uint32_t _renderbuffer;

public:
  FrameBuffer(uint32_t handle = 0);
  ~FrameBuffer() override;
  void bindAttachments(const std::vector<core::AutoPtr<Texture2D>> &textures);
  bool check();
  uint32_t getHandle() const;

public:
  static void bind(const core::AutoPtr<FrameBuffer> &framebuffer);
};
} // namespace firefly::gl