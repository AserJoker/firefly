#include "document/Channel.hpp"
#include "document/Animation.hpp"
using namespace firefly;
using namespace firefly::document;

Channel::Channel() : _handle(0), _start(0), _end(0), _init(0), _final(0) {
  defineProperty(PROP_START, _start);
  defineProperty(PROP_END, _end);
  defineProperty(PROP_INIT, _init);
  defineProperty(PROP_FINAL, _final);
  defineProperty(PROP_ATTRIBUTE, _attribute);

  watchProp(PROP_START, &Channel::onStartChange);
  watchProp(PROP_END, &Channel::onEndChange);
  watchProp(PROP_INIT, &Channel::onInitChange);
  watchProp(PROP_FINAL, &Channel::onFinalChange);
  watchProp(PROP_ATTRIBUTE, &Channel::onAttributeChange);
}
void Channel::setStart(core::Unsigned_t start) {
  setProperty(PROP_START, start);
}
void Channel::setEnd(core::Unsigned_t end) { setProperty(PROP_END, end); }
void Channel::setInit(core::Float_t init) { setProperty(PROP_INIT, init); }
void Channel::setFinal(core::Float_t final) { setProperty(PROP_FINAL, final); }
void Channel::setAttribute(const core::String_t &attribute) {
  setProperty(PROP_ATTRIBUTE, attribute);
}
core::Unsigned_t Channel::getStart() const { return _start; }
core::Unsigned_t Channel::getEnd() const { return _end; }
core::Float_t Channel::getInit() const { return _init; }
core::Float_t Channel::getFinal() const { return _final; }
const core::String_t &Channel::getAttribute() const { return _attribute; }
void Channel::onStartChange() {
  auto parent = findParent<Animation>();
  if (parent) {
    parent->setChannelStart(_handle, _start);
  }
}
void Channel::onEndChange() {
  auto parent = findParent<Animation>();
  if (parent) {
    parent->setChannelEnd(_handle, _end);
  }
}
void Channel::onInitChange() {
  auto parent = findParent<Animation>();
  if (parent) {
    parent->setChannelInit(_handle, _init);
  }
}
void Channel::onFinalChange() {
  auto parent = findParent<Animation>();
  if (parent) {
    parent->setChannelFinal(_handle, _final);
  }
}
void Channel::onAttributeChange() {
  auto parent = findParent<Animation>();
  if (parent) {
    parent->setChannelAttribute(_handle, _attribute);
  }
}
void Channel::onLoad() {
  auto parent = findParent<Animation>();
  if (parent) {
    _handle = parent->createChannel();
    parent->setChannelAttribute(_handle, _attribute);
    parent->setChannelStart(_handle, _start);
    parent->setChannelEnd(_handle, _end);
    parent->setChannelInit(_handle, _init);
    parent->setChannelFinal(_handle, _final);
  }
  Node::onLoad();
}
void Channel::onUnload() {
  Node::onUnload();
  auto parent = findParent<Animation>();
  if (parent) {
    parent->removeChannel(_handle);
    _handle = 0;
  }
}