#pragma once
#include <any>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>
namespace firefly::core {
enum class AttributeType { NIL, BOOLEAN, I32, U32, F32, STRING };

class Attribute {
private:
  std::any _value;
  AttributeType _type;

public:
  Attribute();
  Attribute(const Attribute &another);
  Attribute(std::nullptr_t);
  Attribute(bool boolean);
  Attribute(int32_t i32);
  Attribute(uint32_t u32);
  Attribute(float f32);
  Attribute(const std::string &string);
  const AttributeType &getType() const;

  Attribute &operator=(std::nullptr_t);
  Attribute &operator=(bool);
  Attribute &operator=(int32_t);
  Attribute &operator=(uint32_t);
  Attribute &operator=(float);
  Attribute &operator=(const std::string &);
  Attribute &operator=(const Attribute &);

  bool operator==(std::nullptr_t);
  bool operator==(bool);
  bool operator==(int32_t);
  bool operator==(uint32_t);
  bool operator==(float);
  bool operator==(const std::string &);
  bool operator==(const Attribute &);

  std::string toString() const;
  bool toBoolean() const;
  int32_t toInt32() const;
  uint32_t toUint32() const;
  float toFloat32() const;
};
struct PtrAttribute {
  AttributeType type;
  union {
    bool *boolean;
    int32_t *i32;
    uint32_t *u32;
    float *f32;
    std::string *string;
  };
  PtrAttribute();
  PtrAttribute(std::nullptr_t);
  PtrAttribute(bool *boolean);
  PtrAttribute(int32_t *i32);
  PtrAttribute(uint32_t *u32);
  PtrAttribute(float *f32);
  PtrAttribute(std::string *string);
  PtrAttribute &operator=(const Attribute &attr);
  bool operator==(const Attribute &attr);
  bool operator==(const PtrAttribute &attr);
  Attribute toAttribute() const;
};
}; // namespace firefly::core