#pragma once
#include "core/AutoPtr.hpp"
#include "core/ObjectBase.hpp"
#include <functional>
#include <string>
#include <unordered_map>
namespace firefly::runtime {
class Assert : public core::ObjectBase {
private:
  std::unordered_map<std::string, std::function<core::AutoPtr<core::ObjectBase>(
                                      const std::string &path)>>
      _loader;

public:
  core::AutoPtr<core::ObjectBase> load(const std::string &type,
                                       const std::string &path) const;
  template <class T> void setLoader(const std::string &type, T &&loader) {
    _loader[type] = std::function(loader);
  }
  template <class T> void setLoader(const std::string &type) {
    _loader[type] = std::function(
        [](const std::string &path) -> core::AutoPtr<core::ObjectBase> {
          return T::load(path);
        });
  }
};
}; // namespace firefly::runtime