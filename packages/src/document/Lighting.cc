#include "document/Lighting.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/Event_Resize.hpp"
#include "video/Renderer.hpp"

using namespace firefly;
using namespace firefly::document;
Lighting::Lighting() : _ambientColor(1.0f), _ambientStrength(0.0f) {
  auto renderer = core::Singleton<video::Renderer>::instance();
  auto &viewport = renderer->getViewport();
  _lightTexture = new gl::Texture2D(viewport.width, viewport.height,
                                    gl::PIXEL_FORMAT::RGBA);
  _bus->on(this, &Lighting::onResize);
}

void Lighting::onResize(runtime::Event_Resize &resize) {
  auto &size = resize.getSize();
  _lightTexture =
      new gl::Texture2D(size.width, size.height, gl::PIXEL_FORMAT::RGBA);
}

void Lighting::updateTexture() {
  if (!_lightTexture) {
    _lightTexture =
        new gl::Texture2D(_viewport.z, _viewport.w, gl::PIXEL_FORMAT::RGBA);
  }
}
