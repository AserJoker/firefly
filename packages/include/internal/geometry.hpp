#pragma once

namespace firefly::internal {

constexpr const core::Float_t rectPosition[] = {
    0.0f, 0.0f, 0.0f /*left-bottom*/, 1.0f, 0.0f, 0.0f /*right-bottom*/,
    1.0f, 1.0f, 0.0f /*right-top*/,   0.0f, 1.0f, 0.0f} /*left-bottom*/;

constexpr const core::Unsigned_t rectIndices[] = {0, 1, 2, 2, 3, 0};

constexpr const core::Float_t rectTextureCoord[] = {
    0.0f, 0.0f /*left-bottom*/, 1.0f, 0.0f /*right-bottom*/,
    1.0f, 1.0f /*right-top*/,   0.0f, 1.0f /*left-top*/};

}; // namespace firefly::internal