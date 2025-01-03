#include "core/File.hpp"
#include "core/Coroutine.hpp"
#include "core/FileException.hpp"
#include <fmt/format.h>
#include <ios>
using namespace firefly;
using namespace firefly::core;
File::File(const core::String_t &name, core::Boolean_t append) {
  if (append) {
    _handle.open(name, std::ios::in | std::ios::out | std::ios::binary |
                           std::ios::app);
  } else {
    _handle.open(name, std::ios::in | std::ios::out | std::ios::binary |
                           std::ios::trunc);
  }
  if (!_handle.is_open()) {
    throw FileException(fmt::format("Failed to open file '{}'", name));
  }
}

File::~File() { _handle.close(); }

core::AutoPtr<Buffer> File::read(core::Unsigned_t size) {
  core::Unsigned_t pos = (core::Unsigned_t)_handle.tellg();
  _handle.seekg(0, std::ios::end);
  core::Unsigned_t end = (core::Unsigned_t)_handle.tellg();
  _handle.seekg(pos, std::ios::beg);
  if (!size || size > end - pos) {
    size = end - pos;
    _handle.seekg(pos, std::ios::beg);
  }
  if (size == 0) {
    return new core::Buffer(0);
  }
  auto buf = new core::Buffer(size);
  core::Unsigned_t len = 0;
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
  core::Unsigned_t len = 0;
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