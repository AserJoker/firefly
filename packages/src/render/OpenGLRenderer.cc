#include "render/OpenGLRenderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Exception.hpp"
#include "core/Singleton.hpp"
#include "runtime/Configuration.hpp"
#include "runtime/Window.hpp"
#include <glad/glad.h>
using namespace firefly;
using namespace firefly::render;
OpenGLRenderer::OpenGLRenderer() {
  auto conf = core::Singleton<runtime::Configuration>::instance()->getConfig();
  auto renderer = conf.project.device.renderer;
  if (renderer.type.starts_with("OpenGL")) {
    int profile = SDL_GL_CONTEXT_PROFILE_CORE;
    if (renderer.opengl.debug) {
      profile |= SDL_GL_CONTEXT_DEBUG_FLAG;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    if (renderer.type == "OpenGL_4_2") {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  }
}

OpenGLRenderer::~OpenGLRenderer() {}

void OpenGLRenderer::present() {
  SDL_GL_SwapWindow((SDL_Window *)getWindow()->getHandle());
}

void OpenGLRenderer::initialize(const core::AutoPtr<runtime::Window> &window) {
  Renderer::initialize(window);
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    throw core::Exception("Failed to init GLAD");
  }
}