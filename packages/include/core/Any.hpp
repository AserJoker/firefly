#pragma once
#include "core/ObjectBase.hpp"
#include "core/Value.hpp"
namespace firefly::core {
class Any : public ObjectBase {
private:
  Value _value;

public:
  Any(const Value &value = nullptr);

  ~Any() override = default;

  const Value &getValue() const;

  Value &getValue();
};
}; // namespace firefly::core