#pragma once
#include "Resource.hpp"
#include <fstream>
#include <string>
namespace firefly::runtime {
class Resource_File : public Resource {
private:
  std::fstream _file;

public:
  void write(void *buf, const size_t &size) override;
  void *read(const size_t &size = 0, size_t *len = 0) override;
  void *readAll(size_t *size = 0);
  Resource_File(const std::string &filepath);
  ~Resource_File() override;
};
}; // namespace firefly::runtime