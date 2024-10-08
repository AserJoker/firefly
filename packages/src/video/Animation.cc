#include "video/Animation.hpp"
#include <SDL_timer.h>
using namespace firefly;
using namespace firefly::video;

Animation::Animation(uint32_t fps) : _frameTimeout(1000.0f / fps) {}

void Animation::setFPS(const uint32_t &fps) { _frameTimeout = 1000.0f / fps; }

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

void Animation::onTick() {
  auto now = SDL_GetTicks();
  if (now - _lastTick > _frameTimeout) {
    for (auto &[_, action] : _activeActions) {
      if (!action.enable) {
        continue;
      }
      action.frame++;
      if (action.frame < action.start) {
        continue;
      }
      action.onTick(action.frame++);
      if (action.frame > action.end) {
        if (action.loop) {
          action.frame = action.start;
        } else {
          action.enable = false;
        }
      }
    }
    _lastTick = SDL_GetTicks();
  }
}