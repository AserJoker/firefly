#include "script/LuaModule_Buffer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "script/LuaClass.hpp"
#include "script/LuaModule.hpp"
#include "script/LuaValue.hpp"
#include <stdexcept>
using namespace firefly;
using namespace firefly::script;
int LuaModule_Buffer::openLib(lua_State *state) {
  clazzBuffer = new LuaClass(state);
  clazzBuffer->constructor(&LuaModule_Buffer::construct)
      ->method("getLength", &LuaModule_Buffer::getLength)
      ->method("writeUint8", &LuaModule_Buffer::writeUint8)
      ->method("writeUint16", &LuaModule_Buffer::writeUint16)
      ->method("writeUint32", &LuaModule_Buffer::writeUint32)
      ->method("readUint8", &LuaModule_Buffer::readUint8)
      ->method("readUint16", &LuaModule_Buffer::readUint16)
      ->method("readUint32", &LuaModule_Buffer::readUint32)
      ->method("toUint8Array", &LuaModule_Buffer::toUint8Array)
      ->method("toUint16Array", &LuaModule_Buffer::toUint16Array)
      ->method("toUint32Array", &LuaModule_Buffer::toUint32Array)
      ->method("toString", &LuaModule_Buffer::toString);
  setField("createBuffer", clazzBuffer);
  return LuaModule::openLib(state);
}
LUA_CONSTRUCTOR_DEFINE(LuaModule_Buffer::construct) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER>(args)) {
    if (args.empty()) {
      throw std::runtime_error(
          fmt::format("Failed to create Buffer, size is nil"));
    } else {
      throw std::runtime_error(fmt::format(
          "Failed to create Buffer, size is '{}'", args[1]->getTypeName()));
    }
  }
  auto size = args[1]->toInteger();
  return new core::Buffer(size);
}
LUA_METHOD_DEFINE(LuaModule_Buffer::getLength) {
  if (!LuaValue::validate<LUA_TTABLE>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to get buffer length: self is '{}'", args[0]->getTypeName()));
  }
  auto buffer = self.cast<core::Buffer>();
  return {LuaValue::create(state, (double)buffer->getSize())};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::writeUint8) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to write uint8 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  uint32_t value = args[2]->toInteger();
  if (index >= buffer->getSize()) {
    throw std::runtime_error("Failed to write uint8 to buffer: out of range");
  }
  if (value > 0xff || value < 0) {
    throw std::runtime_error(fmt::format(
        "Failed to write uint8 to buffer: '{}' is not uint8 value", value));
  }
  *(((uint8_t *)buffer->getData()) + index) = (uint8_t)value;
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::writeUint16) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to write uint16 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  uint32_t value = args[2]->toInteger();
  if (index * 2 >= buffer->getSize() || (index + 1) * 2 > buffer->getSize()) {
    throw std::runtime_error("Failed to write uint16 to buffer: out of range");
  }
  if (value > 0xffff) {
    throw std::runtime_error(fmt::format(
        "Failed to write uint16 to buffer: '{}' is not uint16 value", value));
  }
  *(((uint16_t *)buffer->getData()) + index) = (uint16_t)value;
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::writeUint32) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to write uint32 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  uint32_t value = args[2]->toInteger();
  if (index * 4 >= buffer->getSize() || (index + 1) * 4 > buffer->getSize()) {
    throw std::runtime_error("Failed to write uint32 to buffer: out of range");
  }
  *(((uint32_t *)buffer->getData()) + index) = (uint32_t)value;
  return {};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::readUint8) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to read uint8 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  if (index >= buffer->getSize()) {
    throw std::runtime_error("Failed to read uint8 from buffer: out of range");
  }
  auto res = *(((uint8_t *)buffer->getData()) + index);
  return {LuaValue::create(state, (double)res)};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::readUint16) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to read uint16 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  if (index * 2 >= buffer->getSize() || (index + 1) * 2 > buffer->getSize()) {
    throw std::runtime_error("Failed to read uint16 from buffer: out of range");
  }
  auto res = *(((uint16_t *)buffer->getData()) + index);
  return {LuaValue::create(state, (double)res)};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::readUint32) {
  if (!LuaValue::validate<LUA_TTABLE, LUA_TNUMBER>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to read uint32 to buffer: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE, LUA_TNUMBER>()));
  }
  auto buffer = self.cast<core::Buffer>();
  auto index = args[1]->toInteger();
  if (index * 4 >= buffer->getSize() || (index + 1) * 4 > buffer->getSize()) {
    throw std::runtime_error("Failed to read uint32 from buffer: out of range");
  }
  auto res = *(((uint32_t *)buffer->getData()) + index);
  return {LuaValue::create(state, (double)res)};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::toUint8Array) {
  if (!LuaValue::validate<LUA_TTABLE>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to convert buffer to Uint8Array: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE>()));
  }
  auto buffer = self.cast<core::Buffer>();
  uint8_t *data = (uint8_t *)buffer->getData();
  auto array = LuaValue::create(state, LuaValue::LuaValueStack{});
  for (uint32_t i = 1; i <= buffer->getSize(); i++) {
    array->setIndex(i, LuaValue::create(state, (double)data[i - 1]));
  }
  return {array};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::toUint16Array) {
  if (!LuaValue::validate<LUA_TTABLE>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to convert buffer to Uint16Array: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE>()));
  }
  auto buffer = self.cast<core::Buffer>();
  if (buffer->getSize() % 2 != 0) {
    throw std::runtime_error(
        "Failed to conver buffer to Uint16Array: buffer length is not aligned");
  }
  uint16_t *data = (uint16_t *)buffer->getData();
  auto array = LuaValue::create(state, LuaValue::LuaValueStack{});
  for (uint32_t i = 1; i <= buffer->getSize() / 2; i++) {
    array->setIndex(i, LuaValue::create(state, (double)data[i - 1]));
  }
  return {array};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::toUint32Array) {
  if (!LuaValue::validate<LUA_TTABLE>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to convert buffer to Uint32Array: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE>()));
  }
  auto buffer = self.cast<core::Buffer>();
  if (buffer->getSize() % 4 != 0) {
    throw std::runtime_error(
        "Failed to conver buffer to Uint32Array: buffer length is not aligned");
  }
  uint16_t *data = (uint16_t *)buffer->getData();
  auto array = LuaValue::create(state, LuaValue::LuaValueStack{});
  for (uint32_t i = 1; i <= buffer->getSize() / 4; i++) {
    array->setIndex(i, LuaValue::create(state, (double)data[i - 1]));
  }
  return {array};
}
LUA_METHOD_DEFINE(LuaModule_Buffer::toString) {
  if (!LuaValue::validate<LUA_TTABLE>(args)) {
    throw std::runtime_error(fmt::format(
        "Failed to convert buffer to Uint32Array: cannot convert '{}' to '{}'",
        LuaValue::stackTypesToString(args),
        LuaValue::stackTypesToString<LUA_TTABLE>()));
  }
  auto buffer = self.cast<core::Buffer>();
  return {LuaValue::create(
      state, std::string((char *)buffer->getData(), buffer->getSize()))};
}