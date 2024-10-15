#pragma once
#include <any>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>
namespace firefly::core {

class Value {
public:
  enum class Type { NIL, BOOLEAN, I32, U32, F32, STRING };

private:
  std::any _value;
  Type _type;

public:
  Value();
  Value(const Value &another);
  Value(std::nullptr_t);
  Value(bool boolean);
  Value(int32_t i32);
  Value(uint32_t u32);
  Value(float f32);
  Value(const std::string &string);
  const Type &getType() const;

  Value &operator=(std::nullptr_t);
  Value &operator=(bool);
  Value &operator=(int32_t);
  Value &operator=(uint32_t);
  Value &operator=(float);
  Value &operator=(const std::string &);
  Value &operator=(const Value &);

  bool operator==(std::nullptr_t);
  bool operator==(bool);
  bool operator==(int32_t);
  bool operator==(uint32_t);
  bool operator==(float);
  bool operator==(const std::string &);
  bool operator==(const Value &);

  std::string toString() const;
  bool toBoolean() const;
  int32_t toInt32() const;
  uint32_t toUint32() const;
  float toFloat32() const;
};
struct ValuePtr {
  Value::Type type;
  union {
    bool *boolean;
    int32_t *i32;
    uint32_t *u32;
    float *f32;
    std::string *string;
  };
  ValuePtr();
  ValuePtr(std::nullptr_t);
  ValuePtr(bool *boolean);
  ValuePtr(int32_t *i32);
  ValuePtr(uint32_t *u32);
  ValuePtr(float *f32);
  ValuePtr(std::string *string);
  ValuePtr &operator=(const Value &attr);
  bool operator==(const Value &attr);
  bool operator==(const ValuePtr &attr);
  Value toValue() const;
  std::string getTypeName() const;
};
}; // namespace firefly::core