#include "video/Animation.hpp"
#include <SDL_timer.h>
using namespace firefly;
using namespace firefly::video;

Animation::Animation(uint32_t fps, const std::string &group)
    : _frameTimeout(1000.0f / fps), _group(group) {}
void Animation::createAction(const std::string &name, const std::string &attr,
                             float step, uint32_t startFrame, uint32_t endFrame,
                             bool loop) {
  auto count = endFrame - startFrame;
  Action action{};
  action.frame = 0;
  action.loop = loop;
  action.onTick = [=, this](uint32_t frame) -> void {
    static auto start = getParent()->getAttribute(attr).toNumber();
    getParent()->setAttribute(attr, start + step * (frame - startFrame));
  };
  action.start = startFrame;
  action.end = endFrame;
  action.enable = true;
  _actions[name] = action;
}
void Animation::createAction(const std::string &name, const std::string &attr,
                             float start, float end, uint32_t startFrame,
                             uint32_t endFrame, bool loop) {
  auto count = endFrame - startFrame;
  auto step = (end - start) / count;
  Action action{};
  action.frame = 0;
  action.loop = loop;
  action.onTick = [=, this](uint32_t frame) -> void {
    getParent()->setAttribute(attr, start + step * (frame - startFrame));
  };
  action.start = startFrame;
  action.end = endFrame;
  action.enable = true;
  _actions[name] = action;
}
void Animation::setFPS(const uint32_t &fps) { _frameTimeout = 1000.0f / fps; }
void Animation::setGroup(const std::string &name) { _group = name; }

void Animation::start(const std::string &name) {
  if (!_activeActions.contains(name)) {
    _activeActions[name] = _actions[name];
  }
  auto &action = _activeActions[name];
  action.enable = true;
  action.frame = 0;
}

void Animation::stop(const std::string &name) {
  if (!_activeActions.contains(name)) {
    return;
  }
  auto &action = _activeActions[name];
  action.enable = false;
}

void Animation::resume(const std::string &name) {
  if (!_activeActions.contains(name)) {
    return;
  }
  auto &action = _activeActions[name];
  action.enable = true;
}
void Animation::reset(const std::string &name) {
  if (!_activeActions.contains(name)) {
    return;
  }
  auto &action = _activeActions[name];
  action.enable = false;
  action.onTick(action.start);
}

void Animation::onTick() {
  auto parent = getParent();
  if (!parent) {
    return;
  }
  auto now = SDL_GetTicks();
  if (now - _lastTick > _frameTimeout) {
    parent->beginAttrGroup(_group);
    for (auto &[_, action] : _activeActions) {
      if (!action.enable) {
        continue;
      }
      action.frame++;
      if (action.frame < action.start) {
        continue;
      }
      action.onTick(action.frame++);
      if (action.frame > action.end && action.end != 0) {
        if (action.loop) {
          action.frame = action.start;
        } else {
          action.enable = false;
        }
      }
    }
    parent->endAttrGroup();
    _lastTick = SDL_GetTicks();
  }
}