#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "gl/Constant.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/ShaderType.hpp"
#include "gl/Texture2D.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Light.hpp"
#include "video/Material.hpp"
#include "video/RenderObject.hpp"
#include "video/RenderTarget.hpp"
#include "video/Shader.hpp"
#include <concurrencysal.h>
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
    "void main() {\n\r"
    "gl_Position = projection * view * model * vec4(position, 1.0);\n\r"
    "vertexTexcoord = coord;\n\r"
    "}\n\r";
constexpr static const char *internalGBufferFragment =
    "#version 330 core\n\r"
    "in vec2 vertexTexcoord;\n\r"
    "uniform sampler2D diffuse_texture;\n\r"
    "void main() {\n\r"
    "    vec3 col = texture(diffuse_texture, vertexTexcoord).rgb;\n\r"
    "    gl_FragColor = vec4(col, 1.0);\n\r"
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
    "    vec3 col = texture(attachment_0, vertexTexcoord).rgb;\n\r"
    "    gl_FragColor = vec4(col, 1.0);\n\r"
    "}\n\r";

Renderer::Renderer() : _shaderName("internal") {
  _constants = new gl::Constant();
  _light = new Light();
  _constants->setField("model", glm::mat4(1.0f));
}
void Renderer::initialize(const glm::ivec4 &viewport) {
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::setViewport(const glm::ivec4 &viewport) {
  _viewport = viewport;
  glViewport(_viewport.x, _viewport.y, _viewport.z, _viewport.w);
  if (_deferred != nullptr) {
    _deferred->resize({_viewport.z, _viewport.w});
  }
  for (auto &attr : _shaderRenderTargets) {
    attr->resize({_viewport.z, _viewport.w});
  }
  if (_renderTarget != nullptr) {
    _renderTarget->resize({_viewport.z, _viewport.w});
  }
}

const glm::ivec4 &Renderer::getViewport() const { return _viewport; }

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
  _shaderName = name;
  auto shader = Shader::get(name, fmt::format("shader::{}", name));
  auto &programs = shader->getPrograms();
  for (auto &[name, _] : programs) {
    if (name.starts_with("composite_") || name == "composite") {
      _shaderRenderTargets.push_back(
          new RenderTarget(name, {_viewport.z, _viewport.w}));
    }
    if (name == "deferred") {
      _deferred = new video::RenderTarget(name, {_viewport.z, _viewport.w}, 3);
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
  for (auto &[_, attribute] : material->getAttributes()) {
    attribute(_constants);
  }
  auto textures = material->getTextures();
  auto index = 0;
  for (auto &[name, texture] : textures) {
    auto path = fmt::format("texture::{}", texture.path);
    auto tex = gl::Texture2D::get(path, path, texture.mappingmodeU,
                                  texture.mappingmodeV);
    glActiveTexture(GL_TEXTURE0 + index);
    gl::Texture2D::bind(tex);
    _constants->setField(name, index);
    index++;
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

  if (material->isAlphaTest()) {
    glEnable(GL_ALPHA_TEST);
  } else {
    glDisable(GL_ALPHA_TEST);
  }

  if (material->isWireframe()) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

const core::AutoPtr<gl::Constant> &Renderer::getConstants() const {
  return _constants;
}

core::AutoPtr<gl::Constant> &Renderer::getConstants() { return _constants; }

core::AutoPtr<Light> &Renderer::getLight() { return _light; }

const core::AutoPtr<Light> &Renderer::getLight() const { return _light; }

void Renderer::draw(const core::AutoPtr<Material> &material,
                    const core::AutoPtr<Geometry> &geometry,
                    const glm::mat4 &model) {
  if (!material->isBlend()) {
    _normalRenderList.push_back(new RenderObject(geometry, material, model));
  } else {
    core::AutoPtr<RenderObject> rbo;
    for (auto it = _blendRenderList.begin(); it != _blendRenderList.end();
         it++) {
      auto &m = (*it)->getModelMatrix();
      if (model[3][2] > m[3][2]) {
        rbo = new RenderObject(geometry, material, model);
        _blendRenderList.insert(it, rbo);
        break;
      }
    }
    if (!rbo) {
      _blendRenderList.push_back(new RenderObject(geometry, material, model));
    }
  }
}

void Renderer::draw(const core::AutoPtr<ModelSet> &modelset,
                    const glm::mat4 &model) {
  for (auto &[_, m] : modelset->getModels()) {
    if (m->getMaterial()->isVisible()) {
      draw(m->getMaterial(), m->getGeometry(), model);
    }
  }
  for (auto &[_, child] : modelset->getChildren()) {
    draw(child, model);
  }
}

void Renderer::begin(const core::AutoPtr<Camera> &camera) {
  if (camera != nullptr) {
    _constants->setField("projection", camera->getProjectionMatrix());
    _constants->setField("view", camera->getViewMatrix());
    _constants->setField("cameraPosition", camera->getPosition());
  }
}

void Renderer::end() {
  _light->active(_constants);
  if (_deferred != nullptr) {
    _deferred->active();
  }
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (!activeShader(_shaderName, "gbuffer")) {
      activeShader("internal", "basic");
    }
    glDisable(GL_BLEND);
    for (auto &item : _normalRenderList) {
      setMaterial(item->getMeterial());
      _constants->setField("model", item->getModelMatrix());
      _shader->setUniform(_constants);
      item->getGeometry()->draw(gl::DRAW_MODE::TRIANGLES);
    }
    _normalRenderList.clear();
    glEnable(GL_BLEND);
    for (auto &item : _blendRenderList) {
      setMaterial(item->getMeterial());
      _constants->setField("model", item->getModelMatrix());
      _shader->setUniform(_constants);
      item->getGeometry()->draw(gl::DRAW_MODE::TRIANGLES);
    }
    _blendRenderList.clear();
  }
  auto current = _deferred;
  if (_shaderRenderTargets.size()) {
    for (auto it = _shaderRenderTargets.begin();
         it != _shaderRenderTargets.end(); it++) {
      auto &next = *it;
      if (activeShader(_shaderName, current->getStage())) {
        next->active();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);
        current->draw(_shader);
        current = next;
      }
    }
  }
  if (!_renderTarget) {
    gl::FrameBuffer::bind(nullptr);
    glViewport(_viewport.x, _viewport.y, _viewport.z, _viewport.w);
  } else {
    _renderTarget->active();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  if (!activeShader(_shaderName, current->getStage())) {
    activeShader("internal", "basic");
  }
  current->draw(_shader);
}

void Renderer::setRenderTarget(const core::AutoPtr<RenderTarget> &target) {
  _renderTarget = target;
}