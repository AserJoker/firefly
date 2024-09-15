#include "runtime/Resource_File.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "exception/FileException.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
using namespace firefly;
using namespace firefly::runtime;
Resource_File::Resource_File(const std::string &filepath) {
  std::filesystem::path p = filepath;
  p = p.parent_path();
  if (!std::filesystem::exists(p)) {
    std::filesystem::create_directories(p);
  }
  _file.open(filepath,
             std::ios::binary | std::ios::app | std::ios::in | std::ios::out);
  if (!_file.is_open()) {
    throw exception::FileException(
        fmt::format("Failed to open file: {}", filepath));
  }
}
Resource_File::~Resource_File() { _file.close(); }
core::AutoPtr<core::Buffer> Resource_File::read(uint32_t size) {
  if (!size) {
    return readAll();
  }
  uint32_t pos = (uint32_t)_file.tellg();
  _file.seekg(0, std::ios::end);
  uint32_t end = (uint32_t)_file.tellg();
  _file.seekg(pos, std::ios::beg);
  if (end - pos == 0) {
    return new core::Buffer(0);
  }
  auto buf = new core::Buffer(size);
  if (end - pos < size) {
    _file.read((char *)buf->getData(), end - pos);
  } else {
    _file.read((char *)buf->getData(), size);
  }
  return buf;
}
void Resource_File::write(const core::AutoPtr<core::Buffer> &buffer) {
  auto buf = buffer->getData();
  auto &size = buffer->getSize();
  _file.write((char *)buf, size);
}
core::AutoPtr<core::Buffer> Resource_File::readAll() {
  uint32_t pos = (uint32_t)_file.tellg();
  _file.seekg(0, std::ios::end);
  uint32_t end = (uint32_t)_file.tellg();
  if (end - pos == 0) {
    return new core::Buffer(0);
  }
  _file.seekg(pos, std::ios::beg);
  auto buf = new core::Buffer(end - pos);
  _file.read((char *)buf->getData(), end - pos);
  return buf;
}