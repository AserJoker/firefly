#pragma once
#include "Node.hpp"
#include <functional>
#include <string>
#include <unordered_map>
namespace firefly::video {
class Animation : public Node {
private:
  struct Action {
    std::function<void(uint64_t timestamp, uint32_t frame)> onTick;
    uint32_t frame;
    bool loop;
    uint32_t start;
    uint32_t end;
    bool enable;
  };

  std::unordered_map<std::string, Action> _actions;
  std::unordered_map<std::string, Action> _activeActions;

  uint32_t _frameTimeout;
  uint64_t _lastTick;

public:
  Animation(uint32_t fps = 200);
  template <class T>
  void createAction(std::string name, T *attribute, T &&start, T &&end,
                    uint32_t startFrame, uint32_t endFrame, bool loop = false) {
    auto count = endFrame - startFrame;
    auto step = (end - start) / count;
    Action action{};
    action.frame = 0;
    action.loop = loop;
    action.onTick = [=](uint32_t timestamp, uint32_t frame) -> void {
      const_cast<T &>(*attribute) = start + step * (frame - start);
    };
    action.start = startFrame;
    action.end = endFrame;
    action.enable = true;
    _actions[name] = action;
  }
  template <class T>
  void createAction(std::string name, T *attribute, T &&start, T &&end,
                    uint32_t endFrame, bool loop = false) {
    createAction(name, attribute, start, end, 0, endFrame, loop);
  }
  void setFPS(const uint32_t &fps);
  void start(const std::string &name);
  void stop(const std::string &name);
  void resume(const std::string &name);
  void onTick() override;
};
}; // namespace firefly::video