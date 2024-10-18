#pragma once
#include "core/Value.hpp"
#include <glm/fwd.hpp>
namespace firefly::gl {

enum class UniformType {
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

using Boolean_t = core::Boolean_t;
using Intgeter_t = core::Integer_t;
using Float_t = core::Float_t;
using Vector2_t = glm::vec2;
using Vector3_t = glm::vec3;
using Vector4_t = glm::vec4;
using Matrix2x2_t = glm::mat2;
using Matrix2x3_t = glm::mat2x3;
using Matrix3x2_t = glm::mat3x2;
using Matrix3x3_t = glm::mat3;
using Matrix3x4_t = glm::mat3x4;
using Matrix4x3_t = glm::mat4x3;
using Matrix4x4_t = glm::mat4;

using BooleanUniformType =
    core::Type<Boolean_t, UniformType::BOOLEAN, "boolean">;
using IntegerUniformType =
    core::Type<Intgeter_t, UniformType::INTEGER, "integer">;

using FloatUniformType = core::Type<Float_t, UniformType::INTEGER, "integer">;

using Vertex2UniformType = core::Type<Vector2_t, UniformType::VEC2, "vector2">;
using Vertex3UniformType = core::Type<Vector3_t, UniformType::VEC3, "vector3">;
using Vertex4UniformType = core::Type<Vector4_t, UniformType::VEC4, "vector4">;
using Matrix2x2UniformType =
    core::Type<Matrix2x2_t, UniformType::MAT2, "matrix2x2">;
using Matrix2x3UniformType =
    core::Type<Matrix2x3_t, UniformType::MAT2x3, "matrix2x3">;
using Matrix3x2UniformType =
    core::Type<Matrix3x2_t, UniformType::MAT3x2, "matrix3x2">;
using Matrix3x3UniformType =
    core::Type<Matrix3x3_t, UniformType::MAT3, "matrix3x3">;
using Matrix3x4UniformType =
    core::Type<Matrix3x4_t, UniformType::MAT3x4, "matrix3x4">;
using Matrix4x3UniformType =
    core::Type<Matrix4x3_t, UniformType::MAT4x3, "matrix4x3">;
using Matrix4x4UniformType =
    core::Type<Matrix4x4_t, UniformType::MAT4, "matrix4x4">;

using Uniform =
    core::BaseValue<BooleanUniformType, IntegerUniformType, FloatUniformType,
                    Vertex2UniformType, Vertex3UniformType, Vertex4UniformType,
                    Matrix2x2UniformType, Matrix2x3UniformType,
                    Matrix3x3UniformType, Matrix3x2UniformType,
                    Matrix3x4UniformType, Matrix4x4UniformType,
                    Matrix4x3UniformType>;
} // namespace firefly::gl