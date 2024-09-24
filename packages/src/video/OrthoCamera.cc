#include "video/OrthoCamera.hpp"
#include "video/Camera.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
using namespace firefly;
using namespace firefly::video;

OrthoCamera::OrthoCamera(const glm::ivec4 &viewport, const glm::vec3 &position,
                         const glm::vec3 &up, const glm::vec3 &front)
    : Camera(glm::ortho(((float)viewport.x - viewport.z / 2.0f) / (viewport.z),
                        ((float)viewport.x + viewport.z / 2.0f) / (viewport.z),
                        ((float)viewport.y - viewport.w / 2.0f) / (viewport.w),
                        ((float)viewport.y + viewport.w / 2.0f) / viewport.w,
                        -1000.f, 1000.0f) *
                 glm::translate(glm::mat4(1.0f), {-0.5f, 0.5f, 0.0f}) *
                 glm::scale(glm::mat4(1.0f),
                            {1.0f / (viewport.z), -1.0f / (viewport.w), 1.0f}),
             position, up, front) {}

void OrthoCamera::setViewport(const glm::ivec4 &viewport) {
  setProjectionMatrix(
      glm::ortho(((float)viewport.x - viewport.z / 2.0f) / (viewport.z),
                 ((float)viewport.x + viewport.z / 2.0f) / (viewport.z),
                 ((float)viewport.y - viewport.w / 2.0f) / (viewport.w),
                 ((float)viewport.y + viewport.w / 2.0f) / viewport.w, -1000.f,
                 1000.0f) *
      glm::translate(glm::mat4(1.0f), {-0.5f, 0.5f, 0.0f}) *
      glm::scale(glm::mat4(1.0f),
                 {1.0f / (viewport.z), -1.0f / (viewport.w), 1.0f}));
}