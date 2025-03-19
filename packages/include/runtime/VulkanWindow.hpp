#pragma once
#include "runtime/Window.hpp"
namespace firefly::runtime {
class VulkanWindow : public Window {
public:
  VulkanWindow(const std::string &title, uint32_t width, uint32_t height,
               int32_t x = -1, int32_t y = -1, int flags = SDL_WINDOW_SHOWN);
  ~VulkanWindow();
};
} // namespace firefly::runtime