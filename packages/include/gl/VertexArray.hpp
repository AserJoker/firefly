#pragma once
#include "Buffer.hpp"
#include "BufferTraget.hpp"
#include "ComponentType.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
namespace firefly::gl {
class VertexArray : public core::Object {
private:
  uint32_t _handle;
  bool _autoDelete;

public:
  VertexArray(uint32_t handle = 0);
  ~VertexArray() override;
  void addBuffer(BUFFER_TARGET target, core::AutoPtr<Buffer> buffer);
  void removeBuffer(BUFFER_TARGET target);
  void enable(uint32_t index);
  void disable(uint32_t index);
  void setAttribPointer(uint32_t index, uint32_t size, COMPOMENT_TYPE type,
                        uint32_t stride, bool normalize, void *pointer);
  void *getAttribPointer(uint32_t index);

  void bindVertexBuffer(uint32_t bindingindex, core::AutoPtr<Buffer> buffer,
                        uint32_t offset, size_t length);

  void setAttribDivisor(uint32_t index, uint32_t divisor);
  void setBindingDivisor(uint32_t bindingindex, uint32_t divisor);

  void setAttribBinding(uint32_t index, uint32_t bindingindex);
  void setAttribFormat(uint32_t index, uint32_t size, COMPOMENT_TYPE type,
                       bool normalize, uint32_t pointer);

public:
  static void bind(core::AutoPtr<VertexArray> varr);
  static void unbind();
};
}; // namespace firefly::gl