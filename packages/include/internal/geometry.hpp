#pragma once
#include <cstdint>
namespace firefly::internal {

constexpr const float rectPosition[] = {
    0.0f, 0.0f, 0.0f /*left-bottom*/, 1.0f, 0.0f, 0.0f /*right-bottom*/,
    1.0f, 1.0f, 0.0f /*right-top*/,   0.0f, 1.0f, 0.0f} /*left-bottom*/;

constexpr const uint32_t rectIndices[] = {0, 1, 2, 2, 3, 0};

constexpr const float rectTextureCoord[] = {
    0.0f, 0.0f /*left-bottom*/, 1.0f, 0.0f /*right-bottom*/,
    1.0f, 1.0f /*right-top*/,   0.0f, 1.0f /*left-top*/};

}; // namespace firefly::internal