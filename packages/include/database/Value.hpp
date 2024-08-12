#pragma once
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <any>
#include <vector>
namespace firefly::database {
class Value : public core::Object {

private:
  std::any _value;

public:
  Value(std::any &&value = nullptr);
  const std::any &getValue() const;
  void setValue(std::any &&value);
  const bool isEqual(const core::AutoPtr<Value> &another) const;
  const std::string toString() const;
  const std::string getStringValue() const;
  const int32_t getIntegerValue() const;
  const double getNumberValue() const;
  const bool getBooleanValue() const;
  const std::vector<std::string> getStringArrayValue() const;
  const std::vector<int32_t> getIntegerArrayValue() const;
  const std::vector<double> getNumberArrayValue() const;
  const std::vector<bool> getBooleanArrayValue() const;
  const bool isNil() const;
  const bool isString() const;
  const bool isNumber() const;
  const bool isInteger() const;
  const bool isBoolean() const;
};
} // namespace firefly::database