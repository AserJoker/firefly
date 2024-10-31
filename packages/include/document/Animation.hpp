#pragma once
#include "Node.hpp"
#include "core/Map.hpp"
namespace firefly::document {
class Animation : public Node, public Node::Register<"animation", Animation> {
private:
  struct Channel {
    core::Unsigned_t start;
    core::Unsigned_t end;
    core::Float_t init;
    core::Float_t final;
    core::Float_t step;
    core::String_t attribute;
  };

private:
  core::Map<core::Unsigned_t, Channel> _channels;

  core::Boolean_t _loop;
  core::Boolean_t _autoStart;

  core::Unsigned_t _frame;
  core::Boolean_t _running;
  core::Unsigned_t _maxFrame;

  core::Unsigned_t _delay;

protected:
  void onLoad() override;
  void onTick() override;

public:
  Animation();
  core::Unsigned_t createChannel(const core::String_t &attribute = "",
                                 core::Unsigned_t start = 0,
                                 core::Unsigned_t end = 0,
                                 core::Float_t init = 0,
                                 core::Float_t final = 0);
  void setChannelAttribute(core::Unsigned_t handle,
                           const core::String_t &attribute);
  void setChannelStart(core::Unsigned_t handle, core::Unsigned_t start);
  void setChannelEnd(core::Unsigned_t handle, core::Unsigned_t end);
  void setChannelInit(core::Unsigned_t handle, core::Float_t init);
  void setChannelFinal(core::Unsigned_t handle, core::Float_t final);
  void removeChannel(core::Unsigned_t handle);
  void start();
  void stop();
  void pause();
  void reset();

public:
  static inline constexpr auto PROP_LOOP = "loop";
  static inline constexpr auto PROP_AUTO_START = "auto-start";
  static inline constexpr auto PROP_DELAY = "delay";
};
}; // namespace firefly::document