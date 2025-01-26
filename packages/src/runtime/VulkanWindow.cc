#include "runtime/VulkanWindow.hpp"
#include <SDL_video.h>
using namespace firefly;
using namespace firefly::runtime;
VulkanWindow::VulkanWindow(const std::string &title, uint32_t width,
                           uint32_t height, int32_t x, int32_t y, int32_t flags)
    : Window(title, width, height, x, y, flags | SDL_WINDOW_VULKAN) {}
    
VulkanWindow::~VulkanWindow() {}