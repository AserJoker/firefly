#include "runtime/Resource_File.hpp"
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <stdexcept>
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
    throw std::runtime_error(fmt::format("Failed to open file: {}", filepath));
  }
}
Resource_File::~Resource_File() { _file.close(); }
void *Resource_File::read(const size_t &size, size_t *len) {
  void *buf = nullptr;
  if (!size) {
    return readAll(len);
  }
  buf = ::operator new(size);
  size_t pos = _file.tellg();
  _file.seekg(0, std::ios::end);
  size_t end = _file.tellg();
  _file.seekg(pos, std::ios::beg);
  if (end - pos == 0) {
    *len = 0;
    return nullptr;
  }
  if (end - pos < size) {
    _file.read((char *)buf, end - pos);
    if (len) {
      *len = end - pos;
    }
  } else {
    _file.read((char *)buf, size);
    if (len) {
      *len = size;
    }
  }
  return buf;
}
void Resource_File::write(void *buf, const size_t &size) {
  _file.write((char *)buf, size);
}
void *Resource_File::readAll(size_t *len) {
  auto pos = _file.tellg();
  void *buf = nullptr;
  _file.seekg(0, std::ios::end);
  auto end = _file.tellg();
  if (end - pos == 0) {
    *len = 0;
    return nullptr;
  }
  buf = ::operator new(end - pos);
  _file.seekg(pos, std::ios::beg);
  _file.read((char *)buf, end - pos);
  if (len) {
    *len = end - pos;
  }
  return buf;
}