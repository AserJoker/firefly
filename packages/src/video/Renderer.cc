#include "video/Renderer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Singleton.hpp"
#include "gl/Constant.hpp"
#include "gl/DrawMode.hpp"
#include "gl/FrameBuffer.hpp"
#include "gl/PixelFormat.hpp"
#include "gl/Texture2D.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Event_Resize.hpp"
#include "video/Camera.hpp"
#include "video/Geometry.hpp"
#include "video/Light.hpp"
#include "video/Material.hpp"
#include "video/RenderObject.hpp"
#include "video/Shader.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace firefly;
using namespace firefly::video;

float quadVertices[] = {
    -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
// cube VAO
unsigned int quadVAO, quadVBO;

core::AutoPtr<gl::FrameBuffer> frameBuffer;
core::AutoPtr<gl::Texture2D> frameTexture;

Renderer::Renderer() : _shaderName("demo") {
  _constants = new gl::Constant();
  _light = new Light();
  _constants->setField("model", glm::mat4(1.0f));
  auto bus = core::Singleton<runtime::EventBus>::instance();
  bus->on(this, &Renderer::onWindowResize);
}

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

void Renderer::setShader(const std::string &name) { _shaderName = name; }

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
  if (!frameBuffer) {
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    activeShader(_shaderName, "final");
    _shader->setUniform("final_color", 0);

    frameBuffer = new gl::FrameBuffer();
    frameTexture = new gl::Texture2D(1024, 768, gl::PIXEL_FORMAT::RGB);
    frameBuffer->bindAttachments({frameTexture});
  }
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getHandle());
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  activeShader(_shaderName, "basic");
  for (auto &item : _normalRenderList) {
    setMaterial(item->getMeterial());
    _shader->setUniform(_constants);
    item->getGeometry()->draw(gl::DRAW_MODE::TRIANGLES);
  }
  _normalRenderList.clear();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  activeShader(_shaderName, "final");
  glBindVertexArray(quadVAO);
  glActiveTexture(GL_TEXTURE0);
  gl::Texture2D::bind(frameTexture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
void Renderer::onWindowResize(const runtime::Event_Resize &event) {
  glViewport(0, 0, event.getSize().x, event.getSize().y);
}