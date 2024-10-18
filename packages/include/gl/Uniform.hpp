#pragma once
#include "core/Value.hpp"
namespace firefly::gl {

class Uniform : core::Value {
public:
  enum class TYPE {
    BOOLEAN,
    INTEGER,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4,
    MAT2x3,
    MAT3x2,
    MAT3x4,
    MAT4x3
  };

private:
  
};
} // namespace firefly::gl