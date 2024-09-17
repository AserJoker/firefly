#include "core/File.hpp"
#include "core/Coroutine.hpp"
#include "exception/FileException.hpp"
#include <fmt/format.h>
#include <ios>
using namespace firefly;
using namespace firefly::core;
File::File(const std::string &name, bool append) {
  if (append) {
    _handle.open(name, std::ios::in | std::ios::out | std::ios::binary |
                           std::ios::app);
  } else {
    _handle.open(name, std::ios::in | std::ios::out | std::ios::binary |
                           std::ios::trunc);
  }
  if (!_handle.is_open()) {
    throw exception::FileException(
        fmt::format("Failed to open file '{}'", name));
  }
}

File::~File() { _handle.close(); }

core::AutoPtr<Buffer> File::read(uint32_t size) {
  uint32_t pos = (uint32_t)_handle.tellg();
  _handle.seekg(0, std::ios::end);
  uint32_t end = (uint32_t)_handle.tellg();
  _handle.seekg(pos, std::ios::beg);
  if (!size || size > end - pos) {
    size = end - pos;
    _handle.seekg(pos, std::ios::beg);
  }
  if (size == 0) {
    return new core::Buffer(0);
  }
  auto buf = new core::Buffer(size);
  auto len = 0;
  while (len < size) {
    auto chunk = 4096;
    if (len + chunk > size) {
      chunk = size - len;
    }
    _handle.read((char *)buf->getData() + len, chunk);
    Coroutine::yield();
    len += chunk;
  }
  return buf;
}

void File::write(const core::AutoPtr<Buffer> &data) {
  auto buf = data->getData();
  auto &size = data->getSize();
  auto len = 0;
  while (len < size) {
    auto chunk = 4096;
    if (len + chunk > size) {
      chunk = size - len;
    }
    _handle.write((char *)buf + len, chunk);
    Coroutine::yield();
    len += chunk;
  }
}
AutoPtr<Promise<AutoPtr<Buffer>>> File::readAsync(uint32_t size) {
  return Coroutine::async<AutoPtr<Buffer>>(
      [=, this]() -> AutoPtr<Buffer> { return read(size); });
}