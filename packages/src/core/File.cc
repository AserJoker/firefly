#include "core/File.hpp"
#include "core/AutoPtr.hpp"
#include "core/Exception.hpp"
#include "core/MemoryBuffer.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
using namespace firefly;
using namespace firefly::core;

File::File() {}

File::~File() {
  if (_file.is_open()) {
    _file.close();
  }
}

core::AutoPtr<File> File::create(const std::string &path) {
  if (!std::filesystem::exists(path)) {
    std::ofstream file;
    file.open(path);
    file.close();
  } else {
    throw core::Exception(
        fmt::format("Failed to create file: '{}' is exists", path));
  }
  return open(path);
}

core::AutoPtr<File> File::open(const std::string &path) {

  auto file = new File();
  file->_file.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!file->_file.is_open()) {
    throw core::Exception(fmt::format("Failed to open file: {}", path));
  }
  return file;
}

std::string File::readText(const std::string &path) {
  auto file = open(path);
  auto buf = file->read();
  return {(char *)buf->getRaw(), buf->getSize()};
}

core::AutoPtr<Buffer> File::read(const std::string &path) {
  return open(path)->read();
}

const core::AutoPtr<Buffer> File::read(size_t offset, size_t size) const {
  if (!_file.is_open()) {
    return new MemoryBuffer();
  }
  auto f = (std::fstream *)&_file;
  f->seekg(0, std::ios::end);
  size_t max = f->tellg();
  if (max == (size_t)-1) {
    throw core::Exception("Failed to get file size");
  }
  if (offset > max) {
    return new MemoryBuffer();
  }
  f->seekg(offset, std::ios::beg);
  auto len = std::min(max - offset, size);
  if (size == 0) {
    len = max - offset;
  }
  auto buf = new MemoryBuffer(len);
  f->read((char *)buf->getRaw(), len);
  if (f->bad()) {
    f->clear();
    throw core::Exception("Failed to read file");
  }
  return buf;
};

void File::write(const core::AutoConstPtr<Buffer> &buffer, size_t offset) {
  if (!_file.is_open()) {
    return;
  }
  _file.seekg(0, std::ios::end);
  size_t max = _file.tellg();
  if (max == (size_t)-1) {
    throw core::Exception("Failed to get file size");
  }
  if (max < offset) {
    core::AutoPtr empty = new MemoryBuffer(offset - max);
    _file.seekg(0, std::ios::end);
    _file.write((char *)empty->getRaw(), empty->getSize());
  }
  _file.seekg(offset, std::ios::beg);
  _file.write((char *)buffer->getRaw(), buffer->getSize());
  if (_file.bad()) {
    _file.clear();
    throw core::Exception("Failed to write file");
  }
  _file.flush();
};

bool File::isReadable() const { return _readable; };

bool File::isWritable() const { return _writable; };