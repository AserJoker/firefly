#include "core/File.hpp"
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
  if (!size) {
    uint32_t pos = (uint32_t)_handle.tellg();
    _handle.seekg(0, std::ios::end);
    uint32_t end = (uint32_t)_handle.tellg();
    if (end - pos == 0) {
      return new core::Buffer(0);
    }
    _handle.seekg(pos, std::ios::beg);
    auto buf = new core::Buffer(end - pos);
    _handle.read((char *)buf->getData(), end - pos);
    return buf;
  }
  uint32_t pos = (uint32_t)_handle.tellg();
  _handle.seekg(0, std::ios::end);
  uint32_t end = (uint32_t)_handle.tellg();
  _handle.seekg(pos, std::ios::beg);
  if (end - pos == 0) {
    return new core::Buffer(0);
  }
  auto buf = new core::Buffer(size);
  if (end - pos < size) {
    _handle.read((char *)buf->getData(), end - pos);
  } else {
    _handle.read((char *)buf->getData(), size);
  }
  return buf;
}

void File::write(const core::AutoPtr<Buffer> &data) {
  auto buf = data->getData();
  auto &size = data->getSize();
  _handle.write((char *)buf, size);
}