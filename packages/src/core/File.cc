#include "core/File.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include "exception/Exception.hpp"
#include "exception/FileException.hpp"
#include <corecrt.h>
#include <cstdio>
#include <fmt/format.h>
#include <stdio.h>
#include <string.h>
#include <vcruntime_new_debug.h>
using namespace firefly;
using namespace firefly::core;
File::File(const std::string &name, bool append) {
  if (append) {
    auto error = fopen_s(&_handle, name.c_str(), "a+");
    if (error != 0) {
      char buf[1024];
      strerror_s(buf, error);
      throw new exception::FileException(
          fmt::format("Failed to open file '{}':'{}'", name, buf));
    }
  } else {
    auto error = fopen_s(&_handle, name.c_str(), "w+");
    if (error != 0) {
      char buf[1024];
      strerror_s(buf, error);
      throw new exception::FileException(
          fmt::format("Failed to open file '{}':'{}'", name, buf));
    }
  }
}
File::~File() { fclose(_handle); }
const size_t File::getLength() const {
  auto pos = ftell(_handle);
  fseek(_handle, 0, SEEK_END);
  auto length = ftell(_handle);
  fseek(_handle, pos, SEEK_SET);
  return length;
}
const size_t File::getOffset() const { return ftell(_handle); }
void File::setOffset(size_t offset, bool absolute) {
  if (absolute) {
    fseek(_handle, offset, SEEK_SET);
  } else {
    fseek(_handle, offset, SEEK_CUR);
  }
}
core::AutoPtr<Buffer> File::read(size_t size) const {
  core::AutoPtr<Buffer> buffer = new core::Buffer(0);
  void *buf = ::operator new(size);
  auto readsize = fread(buf, size, 1, _handle);
  buffer->setData(0, readsize, buf);
  ::operator delete(buf);
  return buffer;
}
void File::write(const core::AutoPtr<Buffer> &data) {
  fwrite(data->getData(), data->getSize(), 1, _handle);
}
