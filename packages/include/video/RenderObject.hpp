#pragma once
#include "core/Object.hpp"
namespace firefly::video {
class RenderObject : public core::Object {
private:
  std::string _shader;

public:
  RenderObject();
  ~RenderObject() override;
  virtual void draw() = 0;
  const std::string &getShader() const;
  void setShader(const std::string &shader);
  void enable();
  void disable();
};
} // namespace firefly::video