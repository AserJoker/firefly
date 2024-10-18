#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/ShaderType.hpp"
#include "gl/Texture2D.hpp"
#include "video/Geometry.hpp"
#include "video/Material.hpp"
#include "video/RenderTarget.hpp"
#include "video/Shader.hpp"
#include <concurrencysal.h>
#include <fmt/format.h>
#include <glm/ext/matrix_transform.hpp>
#include <unordered_map>

using namespace firefly;
using namespace firefly::video;

constexpr static const char *internalGBufferVertex =
    "#version 330 core \n\r"
    "layout(location = 0) in vec3 position;\n\r"
    "layout(location = 3) in vec2 coord;\n\r"
    "out vec2 vertexTexcoord;\n\r"
    "uniform mat4 projection;\n\r"
    "uniform mat4 view;\n\r"
    "uniform mat4 model;\n\r"
    "uniform mat4 diffuse_texture_coord_matrix;\n\r"
    "void main() {\n\r"
    "gl_Position = projection * view * model * vec4(position, 1.0);\n\r"
    "vertexTexcoord = (diffuse_texture_coord_matrix * "
    "vec4(coord,0.0,1.0)).xy;\n\r"
    "}\n\r";
constexpr static const char *internalGBufferFragment =
    "#version 330 core\n\r"
    "in vec2 vertexTexcoord;\n\r"
    "uniform sampler2D diffuse_texture;\n\r"
    "uniform float diffuse_texture_blend;\n\r"
    "void main() {\n\r"
    "    vec4 col = texture(diffuse_texture, vertexTexcoord).rgba;\n\r"
    "    gl_FragColor = col * diffuse_texture_blend;\n\r"
    "}\n\r";

constexpr static const char *internalBasicVertex =
    "#version 330 core \n\r"
    "layout(location = 0) in vec2 position;\n\r"
    "layout(location = 3) in vec2 coord;\n\r"
    "out vec2 vertexTexcoord;\n\r"
    "void main() {\n\r"
    "gl_Position = vec4(position, 0.0, 1.0);\n\r"
    "vertexTexcoord = coord;\n\r"
    "}\n\r";
constexpr static const char *internalBasicFragment =
    "#version 330 core\n\r"
    "in vec2 vertexTexcoord;\n\r"
    "uniform sampler2D attachment_0;\n\r"
    "void main() {\n\r"
    "    gl_FragColor = texture(attachment_0, vertexTexcoord).rgba;\n\r"
    "}\n\r";

Renderer::Renderer() : _shaderName("internal") {
  _context = new RenderContext();
}
void Renderer::initialize(const core::Rect<> &viewport) {
  setViewport(viewport);
  std::unordered_map<std::string, Shader::ShaderSource> sources = {
      {
          "gbuffer",
          {
              {gl::SHADER_TYPE::VERTEX_SHADER, internalGBufferVertex},
              {gl::SHADER_TYPE::FRAGMENT_SHADER, internalGBufferFragment},
          },
      },
      {
          "basic",
          {
              {gl::SHADER_TYPE::VERTEX_SHADER, internalBasicVertex},
              {gl::SHADER_TYPE::FRAGMENT_SHADER, internalBasicFragment},
          },
      },
  };
  core::AutoPtr<Shader> shader = new Shader(sources);
  Shader::set("internal", shader);
}

void Renderer::setViewport(const core::Rect<> &viewport) {
  _viewport = viewport;
  glViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
  if (_deferred != nullptr) {
    _deferred->resize({_viewport.width, _viewport.height});
  }
  for (auto &attr : _shaderRenderTargets) {
    attr->resize({_viewport.width, _viewport.height});
  }
}

const core::Rect<> &Renderer::getViewport() const { return _viewport; }

bool Renderer::activeShader(const std::string &name, const std::string &stage) {
  auto shader = Shader::get(name, fmt::format("shader::{}", name));
  auto program = shader->getProgram(stage);
  if (!program) {
    return false;
  }
  if (program != _shader) {
    _shader = program;
    _shader->use();
  }
  return true;
}

core::AutoPtr<gl::Program> Renderer::getShaderProgram() { return _shader; }

void Renderer::setShader(const std::string &name) {
  if (_shaderName == name) {
    return;
  }
  _deferred = nullptr;
  _shaderRenderTargets.clear();
  _shaderName = name;
  auto shader = Shader::get(name, fmt::format("shader::{}", name));
  auto &programs = shader->getPrograms();
  for (auto &[name, _] : programs) {
    if (name.starts_with("composite_") || name == "composite") {
      _shaderRenderTargets.push_back(
          new RenderTarget(name, {_viewport.width, _viewport.height}));
    }
    if (name == "deferred") {
      _deferred =
          new video::RenderTarget(name, {_viewport.width, _viewport.height}, 3);
    }
  }
  _shaderRenderTargets.sort([](const core::AutoPtr<RenderTarget> &a,
                               const core::AutoPtr<RenderTarget> &b) -> bool {
    if (a->getStage() == "composite") {
      return false;
    }
    if (b->getStage() == "composite") {
      return true;
    }
    auto sida = a->getStage().substr(11);
    auto ida = std::atoi(sida.c_str());
    auto sidb = b->getStage().substr(11);
    auto idb = std::atoi(sidb.c_str());
    return ida <= idb;
  });
}

const std::string &Renderer::getShader() const { return _shaderName; }

void Renderer::setMaterial(const core::AutoPtr<Material> &material) {
  auto shader = material->getShader();
  if (shader.empty()) {
    shader = "gbuffer";
  }
  if (!activeShader(_shaderName, shader)) {
    activeShader("internal", "basic");
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

  if (material->isBlend()) {
    auto &func = material->getBlendFunc();
    glBlendFunc((GLenum)func.first, (GLenum)func.second);
  }
}

void Renderer::draw(const core::AutoPtr<Material> &material,
                    const core::AutoPtr<Geometry> &geometry,
                    const glm::mat4 &model) {
  if (!material->isBlend()) {
    _context->normalRenderList.push_back({geometry, material, model});
  } else {
    _context->blendRenderList.push_back({geometry, material, model});
    _context->blendRenderList.sort([](auto &a, auto &b) -> bool {
      return a.matrixModel[3][2] < b.matrixModel[3][2];
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

void Renderer::draw(const Renderer::RenderItem &item) {
  setMaterial(item.material);
  for (auto &[name, uniform] : _uniforms) {
    _shader->setUniform(name, uniform);
  }
  auto instanced = item.material->getInstanced();
  if (instanced > 1) {
    item.geometry->drawInstanced(gl::DRAW_MODE::TRIANGLES, instanced);
  } else {
    item.geometry->draw(gl::DRAW_MODE::TRIANGLES);
  }
}

void Renderer::setTexture(const std::string &name,
                          core::AutoPtr<gl::Texture2D> texture) {
  _textures[name] = texture;
}

void Renderer::clearTexture(const std::string &name) {
  if (name.empty()) {
    _textures.clear();
  } else {
    _textures.erase(name);
  }
}

void Renderer::present() {
  std::list<core::AutoPtr<RenderTarget>> pipeline;
  if (_deferred != nullptr) {
    pipeline.push_back(_deferred);
  }
  for (auto &process : _shaderRenderTargets) {
    pipeline.push_back(process);
  }
  core::AutoPtr<RenderTarget> current;
  if (pipeline.size()) {
    current = *pipeline.begin();
  }
  if (current != nullptr) {
    current->active();
  }
  int32_t index = 0;
  for (auto &[name, texture] : _textures) {
    glActiveTexture(GL_TEXTURE0 + index);
    gl::Texture2D::bind(texture);
    _shader->setUniform(name, gl::Uniform(index));
    _shader->setUniform(fmt::format("{}_enable", name), gl::Uniform(true));
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glDisable(GL_BLEND);
  for (auto &item : _context->normalRenderList) {
    draw(item);
  }
  _context->normalRenderList.clear();

  glEnable(GL_BLEND);
  for (auto &item : _context->blendRenderList) {
    draw(item);
  }
  _context->blendRenderList.clear();

  if (_shaderRenderTargets.size() > 1) {
    auto it = pipeline.begin();
    it++;
    while (it != pipeline.end()) {
      auto &next = *it;
      if (activeShader(_shaderName, current->getStage())) {
        next->active();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);
        current->draw(_shader);
        current = next;
      }
      it++;
    }
    gl::FrameBuffer::bind(nullptr);
    glViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (!activeShader(_shaderName, current->getStage())) {
      activeShader("internal", "basic");
    }
    current->draw(_shader);
  }
}
void Renderer::setUniform(const std::string &name, const gl::Uniform &uniform) {
  gl::Uniform a = 1;
  a = uniform;
}