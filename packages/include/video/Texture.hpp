#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "core/Value.hpp"
#include "gl/Texture2D.hpp"
#include <glm/glm.hpp>

namespace firefly::video {
class Texture : public core::Object {
private:
  core::AutoPtr<gl::Texture2D> _texture;
  glm::mat4 _coordMatrix;

public:
  Texture();

  Texture(const core::String_t &path,
          const glm::mat4 &matrix = glm::mat4(1.0f));

  Texture(const core::AutoPtr<gl::Texture2D> &texture,
          const glm::mat4 &matrix = glm::mat4(1.0f));

  void bind(uint32_t index = 0) const;

  void setTexture(const core::String_t &path);

  void setTexture(const core::AutoPtr<gl::Texture2D> &texture);

  const core::AutoPtr<gl::Texture2D> &getTexture() const;

  void setMatrix(const glm::mat4 &matrix);

  const glm::mat4 &getMatrix() const;
};
} // namespace firefly::video