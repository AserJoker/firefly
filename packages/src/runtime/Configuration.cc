#include "runtime/Configuration.hpp"
#include "core/Value.hpp"
#include <string>
#include <vector>
using namespace firefly;
using namespace firefly::runtime;
std::vector<std::string> Configuration::resolve(const std::string &key) {
  std::vector<std::string> path;
  std::string tmp;
  for (auto &ch : key) {
    if (ch == '[' || ch == '.') {
      path.push_back(tmp);
      tmp.clear();
    } else if (ch == ']') {
      continue;
    } else {
      tmp += ch;
    }
  }
  if (!tmp.empty()) {
    path.push_back(tmp);
  }
  return path;
}

Configuration::Configuration() {
  _data.setObject();
  set("project.window.title",
      core::Value::bindString(&_config.project.window.title));
  set("project.window.x", core::Value::bindNumber(&_config.project.window.x));
  set("project.window.y", core::Value::bindNumber(&_config.project.window.y));
  set("project.window.width",
      core::Value::bindNumber(&_config.project.window.width));
  set("project.window.height",
      core::Value::bindNumber(&_config.project.window.height));

  set("project.device.renderer.type",
      core::Value::bindString(&_config.project.device.renderer.type));
  set("project.device.renderer.opengl.debug",
      core::Value::bindBoolean(&_config.project.device.renderer.opengl.debug));
  set("project.device.mixer",
      core::Value::bindString(&_config.project.device.mixer));
  set("project.device.input",
      core::Value::bindString(&_config.project.device.input));
  set("project.device.socket",
      core::Value::bindString(&_config.project.device.socket));
  return;
}

const core::Value &Configuration::get(const std::string &key,
                                      const core::Value &def) const {
  std::vector<std::string> path = resolve(key);
  auto val = &_data;
  for (auto &part : path) {
    if (part[0] >= '0' && part[0] <= '9') {
      if (!val->isArray()) {
        return def;
      }
      auto idx = std::stoul(part);
      if (idx >= val->getArray().size()) {
        return def;
      }
      val = &val->getArray().at(idx);
    } else if (val->isObject()) {
      if (!val->getObject().contains(part)) {
        return def;
      }
      val = &val->getObject().at(part);
    } else {
      return def;
    }
  }
  return *val;
}

bool Configuration::set(const std::string &key, const core::Value &value) {
  std::vector<std::string> path = resolve(key);
  auto field = *path.rbegin();
  path.pop_back();
  auto val = &_data;
  for (auto &part : path) {
    if (part[0] >= '0' && part[0] <= '9') {
      auto idx = std::stoull(part);
      if (val->isNil()) {
        val->setArray();
      }
      if (!val->isArray()) {
        return false;
      }
      while (val->getArray().size() <= idx) {
        val->getArray().push_back({});
      }
      val = &val->getArray().at(idx);
    } else {
      if (val->isNil()) {
        val->setObject();
      }
      if (!val->isObject()) {
        return false;
      }
      if (!val->getObject().contains(part)) {
        val->getObject()[part] = {};
      }
      val = &val->getObject().at(part);
    }
  }
  if (field[0] >= '0' && field[0] <= '9') {
    auto idx = std::stoull(field);
    if (val->isNil()) {
      val->setArray();
    }
    if (!val->isArray()) {
      return false;
    }
    while (val->getArray().size() <= idx) {
      val->getArray().push_back({});
    }
    val->getArray()[idx] = value;
  } else {
    if (val->isNil()) {
      val->setObject();
    }
    if (!val->isObject()) {
      return false;
    }
    val->getObject()[field] = value;
  }
  return true;
}

bool Configuration::has(const std::string &key) const {
  std::vector<std::string> path = resolve(key);
  auto val = &_data;
  for (auto &part : path) {
    if (part[0] >= '0' && part[0] <= '9') {
      if (!val->isArray()) {
        return false;
      }
      auto idx = std::stoul(part);
      if (idx >= val->getArray().size()) {
        return false;
      }
      val = &val->getArray().at(idx);
    } else if (val->isObject()) {
      if (!val->getObject().contains(part)) {
        return false;
      }
      val = &val->getObject().at(part);
    } else {
      return false;
    }
  }
  return true;
}

const Configuration::Config &Configuration::getConfig() const {
  return _config;
}