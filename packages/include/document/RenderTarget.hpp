#pragma once
#include "core/AutoPtr.hpp"
#include "document/Node.hpp"
#include "video/RenderTarget.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace firefly::document {
class RenderTarget : public Node {
private:
  core::AutoPtr<video::RenderTarget> _renderTarget;

public:
  RenderTarget(const std::string &stage, const core::Size<> &size,
               uint32_t attachment = 1);
  RenderTarget(const core::Size<> &size, uint32_t attachment = 1);
  const core::AutoPtr<video::RenderTarget> &getRenderTarget() const;
  void resize(const core::Size<> &size);
  void onTick() override;
};
} // namespace firefly::document