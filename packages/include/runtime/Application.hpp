//
// Created by w30029682 on 2024/7/5.
//
#pragma once

#include "BaseApplication.hpp"
#include "core/AutoPtr.hpp"
#include "runtime/IWindow.hpp"
#include "video/IRenderer.hpp"

namespace firefly::runtime {
class Application : public BaseApplication {
public:
  enum class GRAPHIC_API { NONE, OPENGL, VULKAN, DIRECTX12, DIRECTX11 };

public:
  Application(int argc, char **argv);

  core::AutoPtr<IWindow> createWindow(const std::string &title, int width,
                                      int height,
                                      GRAPHIC_API api = GRAPHIC_API::OPENGL);

protected:
  void processEvent();

  void onInitialize() override;

  void onMainLoop() override;

  void onUnInitialize() override;
};
} // namespace firefly::runtime