#pragma once
#include "core/Object.hpp"
namespace firefly::runtime {
class RunProxy : public core::Object {
public:
  RunProxy();
  virtual int run() = 0;
};
}; // namespace firefly::runtime