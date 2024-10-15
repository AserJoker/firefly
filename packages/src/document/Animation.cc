#include "document/Animation.hpp"
#include <SDL_timer.h>
using namespace firefly;
using namespace firefly::document;

Animation::Animation(uint32_t fps, const std::string &group)
    : _frameTimeout(1000 / fps), _group(group) {}
void Animation::setAction(const std::string &name, const std::string &attr,
                          float step, uint32_t startFrame, uint32_t endFrame,
                          bool loop) {
  Action action{};
  action.frame = 0;
  action.loop = loop;
  action.initialized = false;
  action.step = step;
  action.start = startFrame;
  action.end = endFrame;
  action.enable = true;
  action.attribute = attr;
  _actions[name] = action;
}
void Animation::setAction(const std::string &name, const std::string &attr,
                          float start, float end, uint32_t startFrame,
                          uint32_t endFrame, bool loop) {
  auto count = endFrame - startFrame;
  auto step = (end - start) / count;
  Action action{};
  action.frame = 0;
  action.loop = loop;
  action.initialized = true;
  action.initValue = start;
  action.step = step;
  action.start = startFrame;
  action.end = endFrame;
  action.enable = false;
  action.removed = false;
  action.attribute = attr;
  _actions[name] = action;
}
void Animation::removeAction(const std::string &name) {
  if (_actions.contains(name)) {
    reset(name);
    _actions[name].removed = true;
  }
}
void Animation::setSpeed(const uint32_t &speed) {
  _frameTimeout = 1000 / speed;
}

void Animation::setGroup(const std::string &name) { _group = name; }

void Animation::start(const std::string &name) {
  if (!_actions.contains(name)) {
    return;
  }
  auto &action = _actions[name];
  action.enable = true;
  action.frame = 0;
}

void Animation::stop(const std::string &name) {
  if (!_actions.contains(name)) {
    return;
  }
  auto &action = _actions[name];
  action.enable = false;
}

void Animation::resume(const std::string &name) {
  if (!_actions.contains(name)) {
    return;
  }
  auto &action = _actions[name];
  action.enable = true;
}
void Animation::reset(const std::string &name) {
  if (!_actions.contains(name)) {
    return;
  }
  auto &action = _actions[name];
  action.enable = false;
  getParent()->setAttribute(action.attribute, action.initValue);
}

void Animation::onTick() {
  auto parent = getParent();
  if (!parent) {
    return;
  }
  auto now = SDL_GetTicks();
  bool isRemoveItem = false;
  if (now - _lastTick > _frameTimeout) {
    parent->beginAttrGroup(_group);
    for (auto &[_, action] : _actions) {
      if (action.removed) {
        isRemoveItem = true;
        continue;
      }
      if (!action.enable) {
        continue;
      }
      if (action.frame >= action.start) {
        if (action.frame > action.end && action.end != 0) {
          if (action.loop) {
            action.frame = action.start;
          } else {
            action.enable = false;
            continue;
          }
        }
        if (!action.initialized) {
          action.initValue =
              getParent()->getAttribute(action.attribute).toFloat();
          action.initialized = true;
        }
        getParent()->setAttribute(
            action.attribute,
            action.initValue + action.step * (action.frame - action.start));
      }
      action.frame++;
    }
    if (isRemoveItem) {
      auto it = _actions.begin();
      while (it != _actions.end()) {
        if (it->second.removed) {
          _actions.erase(it);
          it = _actions.begin();
        } else {
          it++;
        }
      }
    }
    parent->endAttrGroup();
    _lastTick = SDL_GetTicks();
  }
}