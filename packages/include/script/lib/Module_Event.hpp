#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
namespace firefly::script {
class Module_Event {
private:
  static FUNC_DEF(on);
  static FUNC_DEF(off);
  static FUNC_DEF(emit2);

public:
  static void open(core::AutoPtr<Script> ctx);
  static void emit(core::AutoPtr<Script> ctx, const std::string &msg,
                   core::AutoPtr<Value> arg = nullptr);

  constexpr static inline auto EVENT_GAME_LOADED = "gameLoaded";
  constexpr static inline auto EVENT_TICK = "tick";
};
} // namespace firefly::script