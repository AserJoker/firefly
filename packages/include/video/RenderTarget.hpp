#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>
#include <vector>
namespace firefly::video {
class RenderTarget : public core::Object {
private:
  core::Size<> _size;
  core::AutoPtr<gl::FrameBuffer> _frame;
  std::string _stage;

public:
  RenderTarget(const std::string &stage, const core::Size<> &size,
               uint32_t attachment = 1);

  RenderTarget(const core::Size<> &size, uint32_t attachment = 1);

  void active();

  const core::Size<> &getSize() const;

  const std::string &getStage() const;

  const core::AutoPtr<gl::FrameBuffer> &getFrameBuffer() const;

  const std::vector<core::AutoPtr<gl::Texture2D>> &getAttachments() const;
  
  void resize(const core::Size<> &size);
};
} // namespace firefly::video