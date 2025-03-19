#pragma once
#include "core/Native.hpp"
namespace firefly::render {
class Shader : public core::Native {
public:
  Shader();

  ~Shader() override;
};
} // namespace firefly::render