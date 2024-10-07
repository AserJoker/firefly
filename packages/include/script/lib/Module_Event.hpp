#pragma once
#include "core/AutoPtr.hpp"
#include "script/Script.hpp"
#include <utility>
namespace firefly::script {
class Module_Event {
private:
  static FUNC_DEF(on);
  static FUNC_DEF(off);
  static FUNC_DEF(emit2);

public:
  static void open(core::AutoPtr<Script> ctx);
  template <class... ARGS>
  static void emit(core::AutoPtr<Script> ctx, const std::string &event,
                   ARGS &&...args) {
    auto scope = ctx->pushScope();
    auto global = ctx->getNativeGlobal();
    auto events = global->getField(ctx, "$events");
    auto callbacks = events->getField(ctx, event);
    if (callbacks->getType(ctx) != Atom::TYPE::NIL) {
      auto length = callbacks->getLength(ctx);
      for (uint32_t i = 0; i < length; i++) {
        auto callback = callbacks->getIndex(ctx, i);
        if (callback->getType(ctx) != Atom::TYPE::NIL) {
          Value::Stack vargs = {
              ctx->createValue(std::forward<ARGS...>(args))...};
          callback->call(ctx, vargs);
        }
      }
    }
    ctx->gc();
    ctx->popScope(scope);
  }

  constexpr static inline auto EVENT_GAME_LOADED = "gameLoaded";
  constexpr static inline auto EVENT_TICK = "tick";
};
} // namespace firefly::script