#pragma once
#include "ObjectBase.hpp"
#include "core/Event.hpp"
#include "typedef.hpp"
#include <set>
#include <string>
#include <unordered_map>
namespace firefly::core {
class EventBus : public ObjectBase {
  using LISTENERS = std::set<HANDLE>;

private:
  std::unordered_map<std::string, LISTENERS> _listeners;

public:
  void emit(const HANDLE &emitter, const Event &event);
  void on(const std::string &event, const HANDLE &listener);
  void off(const std::string &event, const HANDLE &listener);
};
} // namespace firefly::core