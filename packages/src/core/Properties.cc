#include "core/Properties.hpp"
#include "core/AutoPtr.hpp"
#include "core/Buffer.hpp"
#include <fmt/format.h>
#include <vector>
using namespace firefly;
using namespace firefly::core;
Properties::Properties(const core::AutoPtr<Buffer> &buffer) : _root(PObject{}) {
  std::string key;
  std::string value;
  std::string *iterator = &key;
  const char *data = (const char *)buffer->getData();
  bool inValue = false;
  for (auto i = 0;; i++) {
    if (data[i] == '\"') {
      inValue = !inValue;
      continue;
    }
    if (!inValue) {
      if (i == buffer->getSize() || data[i] == '\n' || data[i] == '\r') {
        if (!key.empty() || !value.empty()) {
          set(key, value);
          key.clear();
          value.clear();
          iterator = &key;
        }
        if (i == buffer->getSize()) {
          break;
        }
      } else if (data[i] == '=') {
        iterator = &value;
      } else if (data[i] == ' ' || data[i] == '\t') {
      } else {
        (*iterator) += data[i];
      }
    } else {
      if (i == buffer->getSize()) {
        break;
      }
      (*iterator) += data[i];
    }
  }
}

const std::string Properties::encode(const std::string &source) const {
  std::string result;
  for (auto &c : source) {
    if (c == '\n') {
      result += "\\n";
    } else if (c == '\t') {
      result += "\\t";
    } else if (c == '\r') {
      continue;
    } else if (c == '\\') {
      result += "\\\\";
    } else {
      result += c;
    }
  }
  return result;
}
const std::string Properties::decode(const std::string &source) const {
  std::string result;
  for (auto i = 0; i < source.size(); i++) {
    auto c = source[i];
    if (c == '\\') {
      if (source[i + 1] == 'n') {
        result += '\n';
        i++;
      } else if (source[i + 1] == 't') {
        result += '\t';
        i++;
      } else if (source[i + 1] == 'r') {
        i++;
      } else if (source[i + 1] == '\\') {
        result += '\\';
        i++;
      } else {
        result += '\\';
      }
    } else {
      result += c;
    }
  }
  return result;
}
const std::vector<std::string> Properties::parse(const std::string &key) const {
  std::vector<std::string> result;
  std::string part;
  for (auto &c : key) {
    if (c == '.') {
      if (!part.empty()) {
        result.push_back(part);
        part.clear();
      }
    } else {
      part += c;
    }
  }
  if (!part.empty()) {
    result.push_back(part);
  }
  return result;
}
const std::string Properties::get(const std::string &key) const {
  auto parts = parse(key);
  auto iterator = &_root;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      return "";
    }
    if (!iterator->children.contains(part)) {
      return "";
    }
    iterator = &iterator->children.at(part);
  }
  return iterator->value;
}
void Properties::set(const std::string &key, const std::string &value) {
  auto parts = parse(key);
  Item *iterator = &_root;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      iterator->isObject = true;
      iterator->value.clear();
    }
    if (!iterator->children.contains(part)) {
      iterator->children[part] = PObject{};
    }
    iterator = &iterator->children.at(part);
  }
  iterator->value = decode(value);
  iterator->isObject = false;
  iterator->children.clear();
}
const std::vector<std::string> Properties::keys(const std::string &key) const {
  auto parts = parse(key);
  auto *iterator = &_root;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      return {};
    }
    if (!iterator->children.contains(part)) {
      return {};
    }
    iterator = &iterator->children.at(part);
  }
  std::vector<std::string> result;
  for (auto &[k, _] : iterator->children) {
    result.push_back(k);
  }
  return result;
}
void Properties::del(const std::string &key) {
  auto parts = parse(key);
  auto *iterator = &_root;
  auto i = 0;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      return;
    }
    if (i == parts.size() - 1) {
      iterator->children.erase(part);
    }
    if (!iterator->children.contains(part)) {
      return;
    }
    iterator = &iterator->children.at(part);
    i++;
  }
}
bool Properties::has(const std::string &key) const {
  auto parts = parse(key);
  auto iterator = &_root;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      return false;
    }
    if (!iterator->children.contains(part)) {
      return false;
    }
    iterator = &iterator->children.at(part);
  }
  return true;
}
bool Properties::isObject(const std::string &key) const {
  auto parts = parse(key);
  auto iterator = &_root;
  for (auto &part : parts) {
    if (!iterator->isObject) {
      return false;
    }
    if (!iterator->children.contains(part)) {
      return false;
    }
    iterator = &iterator->children.at(part);
  }
  return iterator->isObject;
}
const std::string Properties::parseJSONField(const std::string &field) const {
  std::string result = "{";
  auto keys = this->keys(field);
  for (auto i = 0; i < keys.size(); i++) {
    if (i != 0) {
      result += ",";
    }
    auto next = field + "." + keys[i];
    if (isObject(next)) {
      result += fmt::format("\"{}\":{}", keys[i], parseJSONField(next));
    } else {
      auto value = get(next);
      result += fmt::format("\"{}\":\"{}\"", keys[i], encode(get(next)));
    }
  }
  result += "}";
  return result;
}
const std::string Properties::toJSON() const { return parseJSONField(); }
void Properties::mergeField(Item &a, const Item &b) {
  if (!b.isObject) {
    a.children.clear();
    a.isObject = b.isObject;
    a.value = b.value;
    return;
  }
  if (!a.isObject) {
    a.isObject = true;
    a.value.clear();
    a.children = PObject{};
  }
  for (auto &[k, v] : b.children) {
    mergeField(a.children[k], v);
  }
}
void Properties::merge(const core::AutoPtr<Properties> &prop) {
  mergeField(_root, prop->_root);
}