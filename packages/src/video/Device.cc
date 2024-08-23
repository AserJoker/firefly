#include "video/Device.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::video;

void Device::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Device::setClearColor(float r, float g, float b, float a) {
  glClearColor(r, g, b, a);
}
void Device::enable(uint32_t state) { glEnable(state); }
void Device::disable(uint32_t state) { glDisable(state); }