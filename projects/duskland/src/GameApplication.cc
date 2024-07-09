#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Application.hpp"
#include "runtime/Resource_Buffer.hpp"
#include "runtime/Window.hpp"
#include "video/ElementBuffer.hpp"
#include "video/Mesh.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/VertexArray.hpp"
#include <fstream>
#include <glad/glad.h>
#include <initializer_list>
#include <iostream>

using namespace firefly;
using namespace duskland;
core::AutoPtr<video::VertexArray> vertexarray;
core::AutoPtr<video::Mesh> mesh;
core::AutoPtr<video::Shader> shader;
std::string readFile(const std::string &path) {
  std::ifstream ifile(path);
  ifile.seekg(0, std::ios::end);
  size_t len = ifile.tellg();
  char *buf = new char[len];
  ifile.seekg(std::ios::beg);
  ifile.read(buf, len);
  ifile.close();
  return std::string(buf, len);
}

core::AutoPtr<video::Shader> loadShader(const std::string &path) {
  std::string vertex = readFile(path + "/vertex.glsl");
  std::string fragment = readFile(path + "/fragment.glsl");
  return new video::Shader({{video::ShaderType::VERTEX, vertex},
                            {video::ShaderType::FRAGMENT, fragment}});
}
struct Vertex {
  float x, y, z, r, g, b;
};

std::initializer_list<Vertex> vertices = {{-0.5f, -0.5f, 0.f, 1.0f, 0.0f, 0.0f},
                                          {0.f, 0.5f, 0.f, 0.0f, 1.0f, 0.0f},
                                          {0.5f, -0.5f, 0.f, 0.0f, 0.0f, 1.0f}};

std::initializer_list<video::Face> indices = {
    {0, 1, 2}, // first Triangle
};
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  shader = loadShader("shader");

  mesh = new video::Mesh({video::POSITION_XYZ, video::COLOR_RGB}, vertices,
                         indices);
  auto resource = new runtime::Resource_Buffer("hello world", 11);
  _resources->dump("a::b::c::d::1.txt", resource);
}

void GameApplication::onMainLoop() {
  static uint32_t i = 0;
  runtime::Application::onMainLoop();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  shader->use();
  shader->setValue("s_Time", i++);
  mesh->draw(shader);
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }
