#include "GameApplication.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/Application.hpp"
#include "runtime/Window.hpp"
#include "video/Buffer.hpp"
#include "video/Mesh.hpp"
#include "video/Renderer.hpp"
#include "video/Shader.hpp"
#include "video/VertexArray.hpp"
#include <glad/glad.h>
#include <initializer_list>

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

using namespace std::chrono;
using namespace firefly;
using namespace duskland;
core::AutoPtr<video::VertexArray> vertexarray;
core::AutoPtr<video::Mesh> mesh;
core::AutoPtr<video::Shader> shader;
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
  shader =
      new video::Shader({{video::ShaderType::VERTEX, vertexShaderSource},
                         {video::ShaderType::FRAGMENT, fragmentShaderSource}});

  mesh = new video::Mesh({video::POSITION_XYZ}, vertices, indices);
}

void GameApplication::onMainLoop() {
  runtime::Application::onMainLoop();
  _renderer->clear({0.2f, 0.3f, 0.3f, 1.0f});

  shader->use();
  mesh->draw();
  _window->present();
}

void GameApplication::onUnInitialize() { runtime::Application::onInitialize(); }
