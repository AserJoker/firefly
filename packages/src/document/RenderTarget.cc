#include "document/RenderTarget.hpp"
#include "core/AutoPtr.hpp"
#include "core/Rect.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "video/Renderer.hpp"
#include <SDL_image.h>
#include <glm/fwd.hpp>
using namespace firefly;
using namespace firefly::document;

RenderTarget::RenderTarget(const std::string &stage, const core::Size<> &size,
                           uint32_t attachment)
    : _renderTarget(new video::RenderTarget(stage, size, attachment)) {}
RenderTarget::RenderTarget(const core::Size<> &size, uint32_t attachment)
    : _renderTarget(new video::RenderTarget("basic", size, attachment)) {}

void RenderTarget::onTick() {
  _renderTarget->active();
  glClearColor(0, 0, 0, 0);
  auto renderer = core::Singleton<video::Renderer>::instance();
  auto context = renderer->pushContext();
  Node::onTick();
  renderer->popContext(context);
  gl::FrameBuffer::bind(nullptr);
  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
}

const core::AutoPtr<video::RenderTarget> &
RenderTarget::getRenderTarget() const {
  return _renderTarget;
}

void RenderTarget::resize(const core::Size<> &size) {
  _renderTarget->resize(size);
}