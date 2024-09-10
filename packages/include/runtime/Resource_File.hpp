#pragma once
#include "Resource.hpp"
#include <fstream>
#include <string>
namespace firefly::runtime {
class Resource_File : public Resource {
private:
  std::fstream _file;

public:
  void write(const core::AutoPtr<core::Buffer> &data) override;
  core::AutoPtr<core::Buffer> read(const uint32_t &size) override;
  core::AutoPtr<core::Buffer> readAll();
  Resource_File(const std::string &filepath);
  ~Resource_File() override;
};
}; // namespace firefly::runtime