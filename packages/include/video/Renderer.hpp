#pragma once
#include "Mesh.hpp"
#include "RenderObject.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "runtime/Event_SDL.hpp"
#include <string>
#include <unordered_map>
namespace firefly::video {
class Renderer : public core::Object {
  friend class RenderObject;

public:
  struct ResourceSet {
    std::unordered_map<std::string, core::AutoPtr<Texture>> textures;
    std::unordered_map<std::string, core::AutoPtr<Shader>> shaders;
    std::unordered_map<std::string, core::AutoPtr<Mesh>> meshs;
  };

private:
  std::unordered_map<std::string, RenderObject *> _robjects;
  ResourceSet _default;
  ResourceSet _current;
  core::AutoPtr<Shader> _currentShader;

  void onEvent(runtime::Event_SDL &event);

public:
  void initialize() override;
  void initDefaultResourceSet();
  void clear();
  void setClearColor(float r, float g, float b, float a);
  void render();
  void bindShader(const std::string &name);
  void bindTexture(const std::string &name, int index);
  core::AutoPtr<Shader> getCurrentShader();
  const core::AutoPtr<Mesh> getMesh(const std::string &name) const;
  const core::AutoPtr<Shader> getShader(const std::string &name) const;
  const core::AutoPtr<Texture> getTexture(const std::string &name) const;
  void drawMesh(const std::string &name);
  void setResourceSet(const ResourceSet &resources);
};
}; // namespace firefly::video