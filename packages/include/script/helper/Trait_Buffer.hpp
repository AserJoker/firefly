#pragma once
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "script/Script.hpp"
#include "script/Value.hpp"
namespace firefly::script {
class Trait_Buffer {
private:
  static FUNC_DEF(getLength);
  static FUNC_DEF(readUint8);
  static FUNC_DEF(readUint16);
  static FUNC_DEF(readUint32);
  static FUNC_DEF(readFloat);
  static FUNC_DEF(writeUint8);
  static FUNC_DEF(writeUint16);
  static FUNC_DEF(writeUint32);
  static FUNC_DEF(writeFloat);
  static FUNC_DEF(toUint8Array);
  static FUNC_DEF(toUint16Array);
  static FUNC_DEF(toUint32Array);
  static FUNC_DEF(toFloatArray);
  static FUNC_DEF(toString);

public:
  static core::AutoPtr<Value> create(core::AutoPtr<Script> ctx,
                                     core::AutoPtr<core::Buffer> buffer);
  static void initialize(core::AutoPtr<Script> ctx);
};

} // namespace firefly::script