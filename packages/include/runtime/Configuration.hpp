#pragma once
#include "core/ObjectBase.hpp"
#include "core/Value.hpp"
#include <vector>
namespace firefly::runtime {
class Configuration : public core::ObjectBase {
private:
  core::Value _data;

  struct Config {
    struct {
      struct {
        std::string title = "firefly";
        int32_t x = -1;
        int32_t y = -1;
        uint32_t width = 1024;
        uint32_t height = 768;
      } window;
      struct {
        struct {
          std::string type = "OpenGL_4_2";
          struct {
            bool debug = false;
          } opengl;
        } renderer;
        std::string mixer = "";
        std::string input = "";
        std::string socket = "";
      } device;
    } project;
  } _config;

private:
  static std::vector<std::string> resolve(const std::string &key);

public:
  Configuration();
  const core::Value &get(const std::string &key,
                         const core::Value &def = {}) const;

  bool set(const std::string &key, const core::Value &value);

  bool has(const std::string &key) const;

  const Config &getConfig() const;
};
}; // namespace firefly::runtime