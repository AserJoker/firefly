#include "script/helper/Trait_Buffer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "exception/ValidateException.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
using namespace firefly;
using namespace firefly::script;
using SelfType = core::AutoPtr<core::Buffer>;
FUNC_DEF(Trait_Buffer::getLength) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  return {ctx->createValue(self->getSize())};
}
FUNC_DEF(Trait_Buffer::readUint8) {
  auto [self, offset] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  uint8_t *buf = (uint8_t *)self->getData();
  if (offset + 1 < self->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint8,out of range");
}
FUNC_DEF(Trait_Buffer::readUint16) {
  auto [self, offset] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  uint16_t *buf = (uint16_t *)self->getData();
  if ((offset + 1) * 2 < self->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint16,out of range");
}
FUNC_DEF(Trait_Buffer::readUint32) {
  auto [self, offset] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  uint32_t *buf = (uint32_t *)self->getData();
  if ((offset + 1) * 4 < self->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint32,out of range");
}
FUNC_DEF(Trait_Buffer::readFloat) {
  auto [self, offset] = Script::parseArgs<SelfType, uint32_t>(ctx, args);
  float *buf = (float *)self->getData();
  if ((offset + 1) * 4 < self->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read float,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint8) {
  auto [self, offset, value] =
      Script::parseArgs<SelfType, uint32_t, uint32_t>(ctx, args);
  if (value >= 0xff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint8,{} is not uint8", value));
  }
  uint8_t *buf = (uint8_t *)self->getData();
  if (offset + 1 < self->getSize()) {
    buf[offset] = (uint8_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint8,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint16) {

  auto [self, offset, value] =
      Script::parseArgs<SelfType, uint32_t, uint32_t>(ctx, args);
  if (value >= 0xffff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint8,{} is not uint8", value));
  }
  uint16_t *buf = (uint16_t *)self->getData();
  if ((offset + 1) * 2 < self->getSize()) {
    buf[offset] = (uint16_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint16,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint32) {
  auto [self, offset, value] =
      Script::parseArgs<SelfType, uint32_t, uint32_t>(ctx, args);
  if (value >= 0xffffffff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint32,{} is not uint32", value));
  }
  uint32_t *buf = (uint32_t *)self->getData();
  if ((offset + 1) * 4 < self->getSize()) {
    buf[offset] = (uint32_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint32,out of range");
}
FUNC_DEF(Trait_Buffer::writeFloat) {
  auto [self, offset, value] =
      Script::parseArgs<SelfType, uint32_t, float>(ctx, args);
  float *buf = (float *)self->getData();
  if ((offset + 1) * 4 < self->getSize()) {
    buf[offset] = (float)value;
    return {};
  }
  throw exception::ValidateException("Failed to write float,out of range");
}
FUNC_DEF(Trait_Buffer::toUint8Array) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  uint8_t *buf = (uint8_t *)self->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < self->getSize(); i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}
FUNC_DEF(Trait_Buffer::toUint16Array) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  if (self->getSize() % 2 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to uint16 array,size is not aligned");
  }
  uint16_t *buf = (uint16_t *)self->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < self->getSize() / 2; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}
FUNC_DEF(Trait_Buffer::toUint32Array) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  if (self->getSize() % 4 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to uint32 array,size is not aligned");
  }
  uint32_t *buf = (uint32_t *)self->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < self->getSize() / 4; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}

FUNC_DEF(Trait_Buffer::toFloatArray) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  if (self->getSize() % 4 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to float array,size is not aligned");
  }
  float *buf = (float *)self->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < self->getSize() / 4; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}

FUNC_DEF(Trait_Buffer::toString) {
  auto [self] = Script::parseArgs<SelfType>(ctx, args);
  std::string result = std::string((char *)self->getData(), self->getSize());
  return {ctx->createValue()->setString(ctx, result)};
}

void Trait_Buffer::initialize(core::AutoPtr<Script> ctx) {

  auto global = ctx->getNativeGlobal();
  auto Buffer =
      ctx->createValue()
          ->setObject(ctx)
          ->setField(ctx, "getLength",
                     ctx->createValue()->setFunction(ctx, getLength))
          ->setField(ctx, "readUint8",
                     ctx->createValue()->setFunction(ctx, readUint8))
          ->setField(ctx, "readUint16",
                     ctx->createValue()->setFunction(ctx, readUint16))
          ->setField(ctx, "readUint32",
                     ctx->createValue()->setFunction(ctx, readUint32))
          ->setField(ctx, "readFloat",
                     ctx->createValue()->setFunction(ctx, readFloat))
          ->setField(ctx, "writeUint8",
                     ctx->createValue()->setFunction(ctx, writeUint8))
          ->setField(ctx, "writeUint16",
                     ctx->createValue()->setFunction(ctx, writeUint16))
          ->setField(ctx, "writeUint32",
                     ctx->createValue()->setFunction(ctx, writeUint32))
          ->setField(ctx, "writeFloat",
                     ctx->createValue()->setFunction(ctx, writeFloat))
          ->setField(ctx, "toUint8Array",
                     ctx->createValue()->setFunction(ctx, toUint8Array))
          ->setField(ctx, "toUint32Array",
                     ctx->createValue()->setFunction(ctx, toUint32Array))
          ->setField(ctx, "toFloatArray",
                     ctx->createValue()->setFunction(ctx, toFloatArray))
          ->setField(ctx, "toString",
                     ctx->createValue()->setFunction(ctx, toString));
  global->setField(ctx, "Buffer", Buffer);
}

core::AutoPtr<Value> Trait_Buffer::create(core::AutoPtr<Script> ctx,
                                          core::AutoPtr<core::Buffer> buffer) {
  auto global = ctx->getNativeGlobal();
  auto Buffer = global->getField(ctx, "Buffer");
  auto instance = ctx->createValue()->setObject(ctx)->setOpaque(buffer);
  for (auto &field : Buffer->getKeys(ctx)) {
    instance->setField(ctx, field, Buffer->getField(ctx, field));
  }
  return instance;
}