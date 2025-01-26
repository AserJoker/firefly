#pragma once
#include "core/ObjectBase.hpp"
#include <string>
namespace firefly::render {
class Shader : public core::ObjectBase {
public:
  enum class TYPE { VERTEX, HULL, DOMAIN, GEOMETRY, PIXEL, COMPUTE };

public:
  Shader();
  ~Shader() override;
  void setShader(const TYPE &type, const std::string &source);
  
};
} // namespace firefly::render