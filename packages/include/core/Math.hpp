#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
namespace firefly::core {
static inline auto scale(const glm::vec3 &vec) {
  return glm::scale(glm::mat4(1.0f), vec);
}
static inline auto translate(const glm::vec3 &vec) {
  return glm::translate(glm::mat4(1.0f), vec);
}

static inline auto rotate(const glm::vec3 &axis, core::Float_t angle) {
  return glm::rotate(glm::mat4(1.0f), angle, axis);
}
} // namespace firefly::core