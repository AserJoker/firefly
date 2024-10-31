#pragma once
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>

namespace firefly::video {
class RenderTarget : public core::Object {
private:
  core::Size<> _size;
  core::AutoPtr<gl::FrameBuffer> _frame;
  core::String_t _stage;

public:
  RenderTarget(const core::String_t &stage, const core::Size<> &size,
               core::Unsigned_t attachment = 1);

  RenderTarget(const core::Size<> &size, core::Unsigned_t attachment = 1);

  void active();

  const core::Size<> &getSize() const;

  const core::String_t &getStage() const;

  const core::AutoPtr<gl::FrameBuffer> &getFrameBuffer() const;

  const core::Array<core::AutoPtr<gl::Texture2D>> &getAttachments() const;

  void resize(const core::Size<> &size);
};
} // namespace firefly::video