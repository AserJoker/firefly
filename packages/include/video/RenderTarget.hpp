#pragma once
#include "Geometry.hpp"
#include "core/AutoPtr.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/Program.hpp"
#include "gl/Texture2D.hpp"
#include "video/Node.hpp"
#include <glm/glm.hpp>
#include <vector>
namespace firefly::video {
class RenderTarget : public Node {
private:
  glm::ivec2 _size;
  core::AutoPtr<gl::FrameBuffer> _frame;
  std::string _stage;

private:
  static core::AutoPtr<Geometry> _geometry;

public:
  RenderTarget(const std::string &stage, const glm::ivec2 &size,
               uint32_t attachment = 1);
  RenderTarget(const glm::ivec2 &size, uint32_t attachment = 1);
  void active();
  const glm::ivec2 &getSize() const;
  const std::string &getStage() const;
  const core::AutoPtr<gl::FrameBuffer> &getFrameBuffer() const;
  const std::vector<core::AutoPtr<gl::Texture2D>> &getAttachments() const;
  void draw(core::AutoPtr<gl::Program> program);
  void resize(const glm::ivec2 &size);
  void onTick() override;
};
} // namespace firefly::video