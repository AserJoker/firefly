#pragma once
#include "core/Type.hpp"
namespace firefly::runtime {
class Event {
private:
  core::Boolean_t _canceled;

public:
  Event();

  Event(const Event &) = delete;

  void cancel();

  [[nodiscard]] core::Boolean_t isCanceled() const;
};
} // namespace firefly::runtime