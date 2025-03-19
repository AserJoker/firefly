#pragma once
#include "Readable.hpp"
#include "Writable.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/ObjectBase.hpp"
#include <fstream>
#include <string>
namespace firefly::core {
class File : public ObjectBase, public Readable, public Writable {
private:
  std::fstream _file;
  bool _readable = true;
  bool _writable = true;

protected:
  File();

public:
  ~File() override;

  const core::AutoPtr<Buffer> read(size_t offset = 0,
                                   size_t size = 0) const override;

  void write(const core::AutoConstPtr<Buffer> &buffer,
             size_t offset = 0) override;

  bool isReadable() const override;

  bool isWritable() const override;

public:
  static core::AutoPtr<File> create(const std::string &path);

  static core::AutoPtr<File> open(const std::string &path);

  static std::string readText(const std::string &path);

  static core::AutoPtr<Buffer> read(const std::string &path);
};
}; // namespace firefly::core