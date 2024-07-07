#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "video/IRenderer.hpp"
namespace firefly::runtime {
class IWindow : public core::Object {
public:
  virtual void setTitle(const std::string &title) = 0;
  virtual std::string getTitle() const = 0;
  virtual void present() = 0;
  virtual core::AutoPtr<video::IRenderer> createRenderer() = 0;
};
} // namespace firefly::runtime