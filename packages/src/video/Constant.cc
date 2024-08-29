#include "video/Constant.hpp"
#include "core/Bitmap.hpp"
#include "video/ConstantType.hpp"
#include <glm/glm.hpp>
using namespace firefly;
using namespace firefly::video;
Constant::Constant() { setMetadata("video::bitmap", new core::Bitmap()); }
void Constant::setField(const std::string &name, const CONSTANT_TYPE &type,
                        const std::any &value) {
  _metadata[name] = type;
  _fields[name] = value;
  auto bitmap = getMetadata("video::bitmap").cast<core::Bitmap>();
  bitmap->enable(name);
}
void Constant::setField(const std::string &name, const bool &value) {
  setField<CONSTANT_TYPE::BOOL>(name, value);
}
void Constant::setField(const std::string &name, const int32_t &value) {
  setField<CONSTANT_TYPE::INT>(name, value);
}
void Constant::setField(const std::string &name, const glm::ivec2 &value) {
  setField<CONSTANT_TYPE::IVEC2>(name, value);
}
void Constant::setField(const std::string &name, const glm::ivec3 &value) {
  setField<CONSTANT_TYPE::IVEC3>(name, value);
}
void Constant::setField(const std::string &name, const glm::ivec4 &value) {
  setField<CONSTANT_TYPE::IVEC4>(name, value);
}
void Constant::setField(const std::string &name, const uint32_t &value) {
  setField<CONSTANT_TYPE::UINT>(name, value);
}
void Constant::setField(const std::string &name, const glm::uvec2 &value) {
  setField<CONSTANT_TYPE::UIVEC2>(name, value);
}
void Constant::setField(const std::string &name, const glm::uvec3 &value) {
  setField<CONSTANT_TYPE::UIVEC3>(name, value);
}
void Constant::setField(const std::string &name, const glm::uvec4 &value) {
  setField<CONSTANT_TYPE::UIVEC4>(name, value);
}
void Constant::setField(const std::string &name, const float &value) {
  setField<CONSTANT_TYPE::FLOAT>(name, value);
}
void Constant::setField(const std::string &name, const glm::vec2 &value) {
  setField<CONSTANT_TYPE::VEC2>(name, value);
}
void Constant::setField(const std::string &name, const glm::vec3 &value) {
  setField<CONSTANT_TYPE::VEC3>(name, value);
}
void Constant::setField(const std::string &name, const glm::vec4 &value) {
  setField<CONSTANT_TYPE::VEC4>(name, value);
}
void Constant::setField(const std::string &name, const double &value) {
  setField<CONSTANT_TYPE::DOUBLE>(name, value);
}
void Constant::setField(const std::string &name, const glm::dvec2 &value) {
  setField<CONSTANT_TYPE::DVEC2>(name, value);
}
void Constant::setField(const std::string &name, const glm::dvec3 &value) {
  setField<CONSTANT_TYPE::DVEC3>(name, value);
}
void Constant::setField(const std::string &name, const glm::dvec4 &value) {
  setField<CONSTANT_TYPE::DVEC4>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat2 &value) {
  setField<CONSTANT_TYPE::MATRIX2>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat2x3 &value) {
  setField<CONSTANT_TYPE::MATRIX2x3>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat3x2 &value) {
  setField<CONSTANT_TYPE::MATRIX3x2>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat3 &value) {
  setField<CONSTANT_TYPE::MATRIX3>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat3x4 &value) {
  setField<CONSTANT_TYPE::MATRIX3x4>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat4x3 &value) {
  setField<CONSTANT_TYPE::MATRIX4x3>(name, value);
}
void Constant::setField(const std::string &name, const glm::mat4 &value) {
  setField<CONSTANT_TYPE::MATRIX4>(name, value);
}
void Constant::setField(const std::string &name,
                        const core::AutoPtr<ConstantBlock> &value) {
  setField<CONSTANT_TYPE::BLOCK>(name, value);
}
const std::unordered_map<std::string, std::any> &Constant::getFields() const {
  return _fields;
}
const std::any Constant::getField(const std::string &name) const {
  if (_fields.contains(name)) {
    return _fields.at(name);
  }
  return 0;
}
const CONSTANT_TYPE Constant::getFieldType(const std::string &name) {
  if (_metadata.contains(name)) {
    return _metadata.at(name);
  }
  return CONSTANT_TYPE::INT;
}