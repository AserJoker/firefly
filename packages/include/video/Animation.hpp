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
    bool removed;
  };

  std::unordered_map<std::string, Action> _actions;

  uint32_t _frameTimeout;
  uint64_t _lastTick;
  std::string _group;

public:
  Animation(uint32_t fps = 200, const std::string &group = "");

  void setAction(const std::string &name, const std::string &attr, float start,
                 float end, uint32_t startFrame, uint32_t endFrame,
                 bool loop = false);
  void setAction(const std::string &name, const std::string &attr, float step,
                 uint32_t startFrame, uint32_t endFrame, bool loop = false);
  void removeAction(const std::string &name);
  void setFPS(const uint32_t &fps);
  void start(const std::string &name);
  void stop(const std::string &name);
  void resume(const std::string &name);
  void reset(const std::string &name);
  void setGroup(const std::string &name);
  void onTick() override;
};
}; // namespace firefly::video