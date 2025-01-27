#include "render/opengl/OpenGLRenderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Exception.hpp"
#include "core/File.hpp"
#include "core/Singleton.hpp"
#include "render/Shader.hpp"
#include "render/opengl/OpenGLShader.hpp"
#include "runtime/Configuration.hpp"
#include "runtime/Window.hpp"
#include <filesystem>
#include <glad/glad.h>
#include <math.h>
#include <unordered_map>

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

core::AutoPtr<Shader> OpenGLRenderer::createShader(const std::string &path) {
  core::AutoPtr<Shader> shader = new Shader();
  std::unordered_map<OpenGLShader::TYPE, std::string> sources;
  using namespace std::filesystem;
  if (is_directory(path)) {
    directory_iterator it(path);
    for (auto &item : it) {
      if (!item.is_directory()) {
        auto ext = item.path().extension();
        if (ext == ".vert") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::VERTEX] = source;
        }
        if (ext == ".frag") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::FRAGMENT] = source;
        }
        if (ext == ".geom") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::GEOMETORY] = source;
        }
        if (ext == ".comp") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::COMPUTE] = source;
        }
        if (ext == ".tese") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::TESS_EVALUATION] = source;
        }
        if (ext == ".tesc") {
          auto source = core::File::readText(item.path().string());
          sources[OpenGLShader::TYPE::TESS_CONTROL] = source;
        }
      }
    }
  }
  shader->setNative(new OpenGLShader(sources));
  return shader;
};

void OpenGLRenderer::activeShader(const core::AutoPtr<Shader> &shader) {
  auto glShader = shader->getNative().cast<OpenGLShader>();
  if (!glShader) {
    throw core::Exception("Failed to active OpenGL shader");
  }
  glShader->use();
};

void OpenGLRenderer::clear(const Color &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
};