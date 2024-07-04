#include "runtime/ConfigProvider.hpp"
#include "core/Singleton.hpp"
#include "runtime/Logger.hpp"
#include <filesystem>
#include <yaml-cpp/yaml.h>

using namespace firefly::runtime;

void ConfigProvider::parse(const std::string &path) {
    for (auto &[file, setters]: _schemas) {
        try {
            std::filesystem::path p = path;
            p.append(file);
            YAML::Node root = YAML::LoadFile(p.string());
            for (auto &setter: setters) {
                setter(&root);
            }
        } catch (std::exception &e) {
            auto logger = core::Singleton<Logger>::instance();
            logger->warn("failed to load config : {}", e.what());
            for (auto &setter: setters) {
                setter(nullptr);
            }
        }
    }
}