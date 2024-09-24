#include "runtime/Window.hpp"
#include "exception/GLADException.hpp"
#include "exception/SDLException.hpp"
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <iostream>

using namespace firefly;
using namespace firefly::runtime;
#define SDL_ASSERT(expr)                                                       \
  if (!(expr))                                                                 \
  throw exception::SDLException(SDL_GetError())

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
}

Window::Window(const std::string &title, int width, int height) {
  SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) == 0);
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) == 0);
#ifndef __apple__
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE) == 0);
#else
  SDL_ASSERT(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                                 SDL_GL_CONTEXT_PROFILE_CORE |
                                     SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));
#endif
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  _window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width, height,
                             SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL |
                                 SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
  SDL_ASSERT(_window != nullptr);
  _ctx = SDL_GL_CreateContext(_window);
  SDL_ASSERT(_ctx != nullptr);
  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    throw exception::GLADException("Failed to initialize glad");
  }
  SDL_GL_SetSwapInterval(0);

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::setTitle(const std::string &title) {
  SDL_SetWindowTitle(_window, title.c_str());
}

const std::string Window::getTitle() const {
  return SDL_GetWindowTitle(_window);
}

void Window::present() { SDL_GL_SwapWindow(_window); }

void Window::setSwapInterval(int flag) {
  SDL_ASSERT(SDL_GL_SetSwapInterval(flag) >= 0);
}
const glm::ivec2 Window::getSize() const {
  int w, h;
  SDL_GetWindowSize(_window, &w, &h);
  return {w, h};
}
void Window::setSize(const glm::ivec2 &size) {
  SDL_SetWindowSize(_window, size.x, size.y);
}
void Window::hide() { SDL_HideWindow(_window); }
void Window::show() { SDL_ShowWindow(_window); }
const glm::ivec2 Window::getWindowPosition() const {
  int x, y;
  SDL_GetWindowPosition(_window, &x, &y);
  return {x, y};
}
void Window::setWindowPosition(const glm::ivec2 &pos) {
  SDL_SetWindowSize(_window, pos.x, pos.y);
}