#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "gl/Constant.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/EventBus.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Light.hpp"
#include "video/Material.hpp"
#include "video/RenderObject.hpp"
#include "video/RenderTarget.hpp"
#include "video/Shader.hpp"
#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace firefly::video;

Renderer::Renderer() : _shaderName("standard") {
  _constants = new gl::Constant();
  _light = new Light();
  _constants->setField("model", glm::mat4(1.0f));
  auto bus = core::Singleton<runtime::EventBus>::instance();
}

void Renderer::setViewport(const glm::ivec4 &viewport) {
  _viewport = viewport;
  glViewport(_viewport.x, _viewport.y, _viewport.z, _viewport.w);
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
  std::sort(_shaderRenderTargets.begin(), _shaderRenderTargets.end(),
            [](const core::AutoPtr<RenderTarget> &a,
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

void Renderer::setMaterial(const core::AutoPtr<Material> &material) {
  material->active(_constants);
  auto textures = material->getTextures();
  auto index = 0;
  for (auto &[_, texture] : textures) {
    auto path = fmt::format("texture::{}", texture.path);
    auto tex = gl::Texture2D::get(path, path, texture.mappingmodeU,
                                  texture.mappingmodeV);
    glActiveTexture(GL_TEXTURE0 + index);
    gl::Texture2D::bind(tex);
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
void Renderer::draw(const core::AutoPtr<Material> &material,
                    const core::AutoPtr<Geometry> &geometry) {
  if (!material->isBlend()) {
    _normalRenderList.push_back(new RenderObject(geometry, material));
  } else {
    _blendRenderList.push_back(new RenderObject(geometry, material));
  }
}

void Renderer::draw(const core::AutoPtr<Model> &model) {
  if (model->getMaterial()->isVisible()) {
    draw(model->getMaterial(), model->getGeometry());
  }
}

void Renderer::draw(const core::AutoPtr<ModelSet> &modelset) {
  for (auto &[_, model] : modelset->getModels()) {
    draw(model);
  }
  for (auto &[_, child] : modelset->getChildren()) {
    draw(child);
  }
}

void Renderer::begin(const core::AutoPtr<Camera> &camera) {
  _constants->setField("projection", camera->getProjectionMatrix());
  _constants->setField("view", camera->getViewMatrix());
  _constants->setField("cameraPosition", camera->getPosition());
}

void Renderer::end() {
  _light->active(_constants);
  if (_deferred != nullptr) {
    _deferred->active();
  }
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    activeShader(_shaderName, "gbuffer");
    glDisable(GL_BLEND);
    for (auto &item : _normalRenderList) {
      setMaterial(item->getMeterial());
      _shader->setUniform(_constants);
      item->getGeometry()->draw(gl::DRAW_MODE::TRIANGLES);
    }
    _normalRenderList.clear();
    glEnable(GL_BLEND);
    for (auto &item : _blendRenderList) {
      setMaterial(item->getMeterial());
      _shader->setUniform(_constants);
      item->getGeometry()->draw(gl::DRAW_MODE::TRIANGLES);
    }
    _blendRenderList.clear();
  }
  auto current = _deferred;
  if (_shaderRenderTargets.size()) {
    for (size_t i = 0; i < _shaderRenderTargets.size(); i++) {
      auto next = _shaderRenderTargets[i];
      next->active();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
              GL_STENCIL_BUFFER_BIT);
      activeShader(_shaderName, current->getStage());
      current->draw(_shader);
      current = next;
    }
  }
  if (!_renderTarget) {
    gl::FrameBuffer::bind(nullptr);
    glViewport(_viewport.x, _viewport.y, _viewport.z, _viewport.w);
  } else {
    _renderTarget->active();
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  activeShader(_shaderName, current->getStage());
  current->draw(_shader);
}

void Renderer::setRenderTarget(const core::AutoPtr<RenderTarget> &target) {
  _renderTarget = target;
}