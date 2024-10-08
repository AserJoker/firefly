#pragma once
#include "ConstantBlock.hpp"
#include "ConstantType.hpp"
#include "core/AutoPtr.hpp"
#include "core/Bitmap.hpp"
#include "core/Object.hpp"
#include "gl/ConstantType.hpp"
#include <any>
#include <glm/glm.hpp>
#include <unordered_map>
namespace firefly::gl {
class Constant : public core::Object {
public:
private:
  std::unordered_map<std::string, CONSTANT_TYPE> _metadata;
  std::unordered_map<std::string, std::any> _fields;
  core::AutoPtr<core::Bitmap> _bitmap;

private:
  template <class T>
  constexpr bool check_any(const std::any &value, const T &raw) {
    return value.type() == typeid(T) && std::any_cast<T>(value) == raw;
  }
  template <CONSTANT_TYPE type, class T>
  void setField(const std::string &name, const T &value) {
    if (_fields.contains(name) && check_any(_fields.at(name), value)) {
      return;
    }
    setField(name, type, value);
  }

public:
  Constant(const core::AutoPtr<core::Bitmap> &bitmap = nullptr);
  void setField(const std::string &name, const CONSTANT_TYPE &type,
                const std::any &value);
  void setField(const std::string &name, bool value);
  void setField(const std::string &name, int32_t value);
  void setField(const std::string &name, const glm::ivec2 &value);
  void setField(const std::string &name, const glm::ivec3 &value);
  void setField(const std::string &name, const glm::ivec4 &value);
  void setField(const std::string &name, uint32_t value);
  void setField(const std::string &name, const glm::uvec2 &value);
  void setField(const std::string &name, const glm::uvec3 &value);
  void setField(const std::string &name, const glm::uvec4 &value);
  void setField(const std::string &name, float value);
  void setField(const std::string &name, const glm::vec2 &value);
  void setField(const std::string &name, const glm::vec3 &value);
  void setField(const std::string &name, const glm::vec4 &value);
  void setField(const std::string &name, double value);
  void setField(const std::string &name, const glm::dvec2 &value);
  void setField(const std::string &name, const glm::dvec3 &value);
  void setField(const std::string &name, const glm::dvec4 &value);
  void setField(const std::string &name, const glm::mat2 &value);
  void setField(const std::string &name, const glm::mat2x3 &value);
  void setField(const std::string &name, const glm::mat3x2 &value);
  void setField(const std::string &name, const glm::mat3 &value);
  void setField(const std::string &name, const glm::mat3x4 &value);
  void setField(const std::string &name, const glm::mat4x3 &value);
  void setField(const std::string &name, const glm::mat4 &value);
  void setField(const std::string &name,
                const core::AutoPtr<ConstantBlock> &value);
  const std::unordered_map<std::string, std::any> &getFields() const;
  const std::any getField(const std::string &name) const;
  const CONSTANT_TYPE getFieldType(const std::string &name);
  void sync(core::AutoPtr<Constant> &another);
  void clear() {
    _bitmap->clear();
    _fields.clear();
    _metadata.clear();
  }
};
}; // namespace firefly::gl