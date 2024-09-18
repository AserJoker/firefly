#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>
namespace firefly::video {
class RenderTarget : public core::Object {
private:
  glm::ivec2 _size;
  core::AutoPtr<gl::FrameBuffer> _frame;
  core::AutoPtr<gl::Texture2D> _texture;
  
public:
};
} // namespace firefly::video