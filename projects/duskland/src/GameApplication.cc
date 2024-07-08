#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Application.hpp"
#include "runtime/Window.hpp"
#include "video/Mesh.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/VertexArray.hpp"
#include <fstream>
#include <glad/glad.h>
#include <initializer_list>
#include <iostream>

using namespace std::chrono;
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
std::initializer_list<float> vertices = {
    0.5f,  0.5f,  0.0f, // top right
    0.5f,  -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f  // top left
};
std::initializer_list<video::Face> indices = {
    {0, 1, 3}, // first Triangle
    {1, 2, 3}  // second Triangle
};
GameApplication::GameApplication(int argc, char *argv[])
    : runtime::Application(argc, argv) {}

void GameApplication::onInitialize() {
  runtime::Application::onInitialize();
  _window = new runtime::Window("duskland", 1024, 768);
  _renderer = new video::Renderer();
  shader = loadShader("shader");

  mesh = new video::Mesh({video::POSITION_XYZ}, vertices, indices);
}

void GameApplication::onMainLoop() {
  static int i = 0;
  runtime::Application::onMainLoop();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});
  shader->use();
  shader->setValue("s_Time", i++);
  mesh->draw(shader);
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }
