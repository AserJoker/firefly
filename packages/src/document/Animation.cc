#include "document/Animation.hpp"
#include <chrono>
using namespace firefly;
using namespace firefly::document;
using namespace std::chrono;
Animation::Animation()
    : _loop(false), _autoStart(false), _running(false), _maxFrame(0),
      _delay(50) {
  defineProperty(PROP_LOOP, _loop);
  defineProperty(PROP_AUTO_START, _autoStart);
  defineProperty(PROP_DELAY, _delay);
}

core::Unsigned_t Animation::createChannel(const core::String_t &attribute,
                                          core::Unsigned_t start,
                                          core::Unsigned_t end,
                                          core::Float_t init,
                                          core::Float_t final) {
  static core::Unsigned_t handle = 0;
  while (_channels.contains(handle)) {
    handle++;
  }
  core::Float_t step = 0;
  if (end > start) {
    step = (final - init) / (end - start);
  }
  _channels[handle] = {start, end, init, final, step, attribute};
  if (end > _maxFrame) {
    _maxFrame = end;
  }
  return handle++;
}
void Animation::setChannelAttribute(core::Unsigned_t handle,
                                    const core::String_t &attribute) {
  if (!_channels.contains(handle)) {
    return;
  }
  _channels[handle].attribute = attribute;
}
void Animation::setChannelStart(core::Unsigned_t handle,
                                core::Unsigned_t start) {
  if (!_channels.contains(handle)) {
    return;
  }
  _channels[handle].start = start;
}
void Animation::setChannelEnd(core::Unsigned_t handle, core::Unsigned_t end) {
  if (!_channels.contains(handle)) {
    return;
  }
  _channels[handle].end = end;
  if (end > _maxFrame) {
    _maxFrame = end;
  }
}
void Animation::setChannelInit(core::Unsigned_t handle, core::Float_t init) {
  if (!_channels.contains(handle)) {
    return;
  }
  _channels[handle].init = init;
}
void Animation::setChannelFinal(core::Unsigned_t handle, core::Float_t final) {
  if (!_channels.contains(handle)) {
    return;
  }
  _channels[handle].final = final;
}
void Animation::removeChannel(core::Unsigned_t handle) {
  _channels.erase(handle);
}

void Animation::onLoad() {
  if (_autoStart) {
    start();
  }
}

void Animation::onTick() {
  static auto time = system_clock::now();
  auto now = system_clock::now();
  if (now - time < (_delay * 1ms)) {
    return;
  }
  time = now;
  if (!_running) {
    return;
  }
  if (_frame > _maxFrame) {
    if (_loop) {
      _frame = 0;
    } else {
      _running = false;
      return;
    }
  }
  auto parent = getParent();
  if (parent == nullptr) {
    return;
  }
  for (auto &[_, channel] : _channels) {
    if (_frame >= channel.start && _frame <= channel.end) {
      parent->setProperty(channel.attribute,
                          channel.init +
                              channel.step * (_frame - channel.start));
    }
  }
  _frame++;
}

void Animation::start() {
  if (_frame > _maxFrame) {
    _frame = _maxFrame;
  }
  _running = true;
}

void Animation::stop() {
  reset();
  _running = false;
}
void Animation::pause() { _running = false; }

void Animation::reset() {
  _frame = 0;
  auto parent = getParent();
  if (parent == nullptr) {
    return;
  }
  for (auto &[_, channel] : _channels) {
    parent->setProperty(channel.attribute, channel.init);
  }
}