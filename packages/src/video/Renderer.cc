#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/FileException.hpp"
#include "core/Singleton.hpp"
#include "gl/DrawMode.hpp"
#include "runtime/Logger.hpp"
#include "video/GLADException.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/Shader.hpp"
#include "video/Texture.hpp"
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace firefly::video;

static void APIENTRY showOpenGLMessage(GLenum source, GLenum type,
                                       core::Unsigned_t id, GLenum severity,
                                       GLsizei length, core::CString_t message,
                                       const void *userParam) {
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  auto logger = core::Singleton<runtime::Logger>::instance();
  logger->warn("---------------");
  logger->warn("Debug message ({}): {}", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    logger->warn("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    logger->warn("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    logger->warn("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    logger->warn("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    logger->warn("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    logger->warn("Source: Other");
    break;
  }

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    logger->warn("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    logger->warn("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    logger->warn("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    logger->warn("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    logger->warn("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    logger->warn("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    logger->warn("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    logger->warn("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    logger->warn("Type: Other");
    break;
  }

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    logger->warn("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    logger->warn("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    logger->warn("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    logger->warn("Severity: notification");
    break;
  }
  logger->warn("---------------");
}

Renderer::Renderer(SDL_Window *window) : _shaderName("internal") {
  _window = window;
  _ctx = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(0);
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw GLADException("Failed to initialize glad");
  }

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(showOpenGLMessage, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                        GL_TRUE);

  glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
  glDepthFunc(GL_LEQUAL);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  core::Integer_t width, height;
  SDL_GetWindowSize(window, &width, &height);
  setViewport({0, 0, (core::Unsigned_t)width, (core::Unsigned_t)height});
  _context = new RenderContext();
}

Renderer::~Renderer() { SDL_GL_DeleteContext(_ctx); }

void Renderer::setViewport(const core::Rect<> &viewport) {
  _viewport = viewport;
  glViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
}

const core::Rect<> &Renderer::getViewport() const { return _viewport; }

void Renderer::bindingTextures(
    const core::Map<core::String_t, core::AutoPtr<Texture>> &textures,
    core::Unsigned_t offset) {
  core::Integer_t index = (core::Integer_t)offset;
  for (auto &[name, tex] : textures) {
    tex->bind(index);
    setUniform(fmt::format("{}_texture", name), index);
    setUniform(fmt::format("{}_matrix", name), tex->getMatrix());
    setUniform(fmt::format("{}_enable", name), true);
    index++;
  }
}
core::Boolean_t Renderer::activeShader(const core::String_t &name,
                                       const core::String_t &stage) {
  try {
    auto shader = Shader::get(name, fmt::format("shader::{}", name));
    auto program = shader->getProgram(stage);
    if (!program) {
      return false;
    }
    if (program != _shader) {
      _shader = program;
      _shader->use();
    }
    for (auto &[name, uniform] : _uniforms) {
      _shader->setUniform(name, uniform);
    }
    return true;
  } catch (core::FileException &e) {
    core::Singleton<runtime::Logger>::instance()->warn(
        "Failed to active shader '{}:{}'\n\t{}", name, stage, e.what());
    return false;
  }
}

core::AutoPtr<gl::Program> Renderer::getShaderProgram() { return _shader; }

void Renderer::setShader(const core::String_t &name) { _shaderName = name; }

const core::String_t &Renderer::getShader() const { return _shaderName; }

void Renderer::setMaterial(const core::AutoPtr<Material> &material) {
  bindingTextures(material->getTextures(), (core::Unsigned_t)_textures.size());

  for (auto &[name, attribute] : material->getAttributes()) {
    setUniform(name, attribute);
  }

  if (material->isDepthTest()) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  if (material->isStencilTest()) {
    glEnable(GL_STENCIL_TEST);
  } else {
    glDisable(GL_STENCIL_TEST);
  }

  if (material->isWireframe()) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (material->isTransparent()) {
    auto &func = material->getBlendFunc();
    glBlendFunc((GLenum)func.first, (GLenum)func.second);
  }

  auto shader = material->getShader();
  if (shader.empty()) {
    shader = "basic";
  }

  if (!activeShader(_shaderName, shader)) {
    activeShader("internal", "basic");
  }
}

void Renderer::draw(const core::AutoPtr<Material> &material,
                    const core::AutoPtr<Geometry> &geometry,
                    const glm::mat4 &model) {
  if (!material->isTransparent()) {
    _context->solidRenderList.push_back({geometry, material, model});
  } else {
    _context->blendRenderList.push_back({geometry, material, model});
    _context->blendRenderList.sort([](auto &a, auto &b) -> core::Boolean_t {
      return a.matrixModel[3][2] > b.matrixModel[3][2];
    });
  }
}

core::AutoPtr<Renderer::RenderContext> Renderer::pushContext() {
  auto context = _context;
  _context = new RenderContext();
  return context;
}

void Renderer::popContext(const core::AutoPtr<Renderer::RenderContext> &ctx) {
  _context = ctx;
}

void Renderer::drawPass(const Renderer::Pass &item) {
  setUniform("model", item.matrixModel);
  setMaterial(item.material);
  auto instanced = item.material->getInstanced();
  if (instanced > 1) {
    item.geometry->drawInstanced(gl::DRAW_MODE::TRIANGLES, instanced);
  } else {
    item.geometry->draw(gl::DRAW_MODE::TRIANGLES);
  }
}
void Renderer::drawPassList(std::list<Renderer::Pass> &list) {
  for (auto &item : list) {
    drawPass(item);
  }
  list.clear();
}

void Renderer::drawContext(core::AutoPtr<Renderer::RenderContext> &ctx) {
  glDisable(GL_BLEND);
  drawPassList(ctx->solidRenderList);
  ctx->solidRenderList.clear();

  glEnable(GL_BLEND);
  drawPassList(ctx->blendRenderList);
  ctx->blendRenderList.clear();
}

void Renderer::present() {
  SDL_GL_MakeCurrent(_window, _ctx);
  auto uniforms = _uniforms;
  bindingTextures(_textures);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  drawContext(_context);
  _uniforms = uniforms;
}

void Renderer::setUniform(const core::String_t &name,
                          const gl::Uniform &uniform) {
  _uniforms[name] = uniform;
}

void Renderer::setTexture(const core::String_t &name,
                          const core::String_t &path) {
  _textures[name] = new Texture(path);
}

void Renderer::removeTexture(const core::String_t &name) {
  _textures.erase(name);
}

const core::Map<core::String_t, core::AutoPtr<Texture>> &
Renderer::getTextures() const {
  return _textures;
}
const SDL_Window *Renderer::getWindow() const { return _window; }