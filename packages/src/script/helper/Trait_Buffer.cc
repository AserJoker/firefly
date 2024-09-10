#include "script/helper/Trait_Buffer.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "exception/ValidateException.hpp"
#include "script/Value.hpp"
using namespace firefly;
using namespace firefly::script;
FUNC_DEF(Trait_Buffer::getLength) {
  VALIDATE_ARGS(getLength, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  return {ctx->createValue()->setNumber(ctx, (double)buffer->getSize())};
}
FUNC_DEF(Trait_Buffer::readUint8) {
  VALIDATE_ARGS(readUint8, 2);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  uint8_t *buf = (uint8_t *)buffer->getData();
  if (offset + 1 < buffer->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint8,out of range");
}
FUNC_DEF(Trait_Buffer::readUint16) {
  VALIDATE_ARGS(readUint16, 2);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  uint16_t *buf = (uint16_t *)buffer->getData();
  if ((offset + 1) * 2 < buffer->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint16,out of range");
}
FUNC_DEF(Trait_Buffer::readUint32) {
  VALIDATE_ARGS(readUint32, 2);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  uint32_t *buf = (uint32_t *)buffer->getData();
  if ((offset + 1) * 4 < buffer->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read uint32,out of range");
}
FUNC_DEF(Trait_Buffer::readFloat) {
  VALIDATE_ARGS(readFloat, 2);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  float *buf = (float *)buffer->getData();
  if ((offset + 1) * 4 < buffer->getSize()) {
    return {ctx->createValue()->setNumber(ctx, *(buf + offset))};
  }
  throw exception::ValidateException("Failed to read float,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint8) {
  VALIDATE_ARGS(writeUint8, 3);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset =(uint32_t) args[1]->toNumber(ctx);
  auto value = args[2]->toNumber(ctx);
  if (value >= 0xff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint8,{} is not uint8", value));
  }
  uint8_t *buf = (uint8_t *)buffer->getData();
  if (offset + 1 < buffer->getSize()) {
    buf[offset] = (uint8_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint8,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint16) {
  VALIDATE_ARGS(writeUint16, 3);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  auto value = args[2]->toNumber(ctx);
  if (value >= 0xffff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint8,{} is not uint8", value));
  }
  uint16_t *buf = (uint16_t *)buffer->getData();
  if ((offset + 1) * 2 < buffer->getSize()) {
    buf[offset] = (uint16_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint16,out of range");
}
FUNC_DEF(Trait_Buffer::writeUint32) {
  VALIDATE_ARGS(writeUint32, 3);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  auto value = args[2]->toNumber(ctx);
  if (value >= 0xffffffff || value <= 0) {
    throw exception::ValidateException(
        fmt::format("Failed to write uint32,{} is not uint32", value));
  }
  uint32_t *buf = (uint32_t *)buffer->getData();
  if ((offset + 1) * 4 < buffer->getSize()) {
    buf[offset] = (uint32_t)value;
    return {};
  }
  throw exception::ValidateException("Failed to write uint32,out of range");
}
FUNC_DEF(Trait_Buffer::writeFloat) {
  VALIDATE_ARGS(writeFloat, 3);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint32_t offset = (uint32_t)args[1]->toNumber(ctx);
  auto value = args[2]->toNumber(ctx);
  float *buf = (float *)buffer->getData();
  if ((offset + 1) * 4 < buffer->getSize()) {
    buf[offset] = (float)value;
    return {};
  }
  throw exception::ValidateException("Failed to write float,out of range");
}
FUNC_DEF(Trait_Buffer::toUint8Array) {
  VALIDATE_ARGS(toUint8Array, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  uint8_t *buf = (uint8_t *)buffer->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < buffer->getSize(); i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}
FUNC_DEF(Trait_Buffer::toUint16Array) {
  VALIDATE_ARGS(toUint16Array, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  if (buffer->getSize() % 2 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to uint16 array,size is not aligned");
  }
  uint16_t *buf = (uint16_t *)buffer->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < buffer->getSize() / 2; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}
FUNC_DEF(Trait_Buffer::toUint32Array) {
  VALIDATE_ARGS(toUint32Array, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  if (buffer->getSize() % 4 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to uint32 array,size is not aligned");
  }
  uint32_t *buf = (uint32_t *)buffer->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < buffer->getSize() / 4; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}

FUNC_DEF(Trait_Buffer::toFloatArray) {
  VALIDATE_ARGS(toFloatArray, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  if (buffer->getSize() % 4 != 0) {
    throw exception::ValidateException(
        "Failed to convert buffer to float array,size is not aligned");
  }
  float *buf = (float *)buffer->getData();
  auto result = ctx->createValue()->setArray(ctx);
  for (uint32_t i = 0; i < buffer->getSize() / 4; i++) {
    result->setIndex(ctx, i, ctx->createValue()->setNumber(ctx, buf[i]));
  }
  return {result};
}

FUNC_DEF(Trait_Buffer::toString) {
  VALIDATE_ARGS(toString, 1);
  auto self = args[0];
  auto buffer = self->getOpaque().cast<core::Buffer>();
  std::string result =
      std::string((char *)buffer->getData(), buffer->getSize());
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