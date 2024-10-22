#include "video/Texture.hpp"
#include "core/AutoPtr.hpp"
#include "core/Value.hpp"
#include "gl/Texture2D.hpp"
using namespace firefly;
using namespace firefly::video;
Texture::Texture() : _coordMatrix(1.0f) {}

Texture::Texture(const core::String_t &path, const glm::mat4 &matrix)
    : _coordMatrix(matrix) {
  _texture = gl::Texture2D::get(path, fmt::format("texture::{}", path));
}

Texture::Texture(const core::AutoPtr<gl::Texture2D> &tex,
                 const glm::mat4 &matrix)
    : _texture(tex), _coordMatrix(matrix) {}

void Texture::bind(uint32_t index) const {
  gl::Texture2D::bind(_texture, index);
}

void Texture::setTexture(const core::String_t &path) {
  _texture = gl::Texture2D::get(path, fmt::format("texture::{}", path));
}

void Texture::setTexture(const core::AutoPtr<gl::Texture2D> &tex) {
  _texture = tex;
}

const core::AutoPtr<gl::Texture2D> &Texture::getTexture() const {
  return _texture;
}

void Texture::setMatrix(const glm::mat4 &matrix) { _coordMatrix = matrix; }

const glm::mat4 &Texture::getMatrix() const { return _coordMatrix; }