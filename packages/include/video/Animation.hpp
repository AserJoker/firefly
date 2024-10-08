#pragma once
#include "Node.hpp"
#include <functional>
#include <string>
#include <unordered_map>
namespace firefly::video {
class Animation : public Node {
private:
  struct Action {
    std::function<void(uint32_t frame)> onTick;
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

  void createAction(const std::string &name, const std::string &attr,
                    float start, float end, uint32_t startFrame,
                    uint32_t endFrame, bool loop = false) {
    auto count = endFrame - startFrame;
    auto step = (end - start) / count;
    Action action{};
    action.frame = 0;
    action.loop = loop;
    action.onTick = [=, this](uint32_t frame) -> void {
      getParent()->setAttribute(attr, start + step * (frame - start));
    };
    action.start = startFrame;
    action.end = endFrame;
    action.enable = true;
    _actions[name] = action;
  }

  void createAction(const std::string &name, const std::string &attr,
                    float step, uint32_t startFrame, uint32_t endFrame,
                    bool loop = false) {
    auto count = endFrame - startFrame;
    Action action{};
    action.frame = 0;
    action.loop = loop;
    auto attribute = getParent()->getAttribute(attr);
    auto start = attribute.toNumber();
    action.onTick = [=, this](uint32_t frame) -> void {
      getParent()->setAttribute(attr, start + step * (frame - start));
    };
    action.start = startFrame;
    action.end = endFrame;
    action.enable = true;
    _actions[name] = action;
  }

  void setFPS(const uint32_t &fps);
  void start(const std::string &name);
  void stop(const std::string &name);
  void resume(const std::string &name);
  void onTick() override;
};
}; // namespace firefly::video