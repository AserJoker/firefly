#pragma once
#include "AutoPtr.hpp"
#include "core/Buffer.hpp"
#include "core/Object.hpp"
#include <unordered_map>
#include <vector>
namespace firefly::core {
class Properties : public Object {
private:
  struct Item {
    std::unordered_map<std::string, Item> children;
    std::string value;
    bool isObject;
    Item(const std::unordered_map<std::string, Item> &item)
        : children(item), isObject(true) {}
    Item(const std::string &item) : value(item), isObject(false) {}
    Item() : isObject(true){};
  };
  using PObject = std::unordered_map<std::string, Item>;
  Item _root;

private:
  const std::vector<std::string> parse(const std::string &key) const;
  const std::string parseJSONField(const std::string &field = "") const;
  const std::string encode(const std::string &source) const;
  const std::string decode(const std::string &source) const;
  void mergeField(Item &a, const Item &b);

public:
  Properties(const core::AutoPtr<Buffer> &buffer = 0);
  const std::string get(const std::string &key) const;
  void set(const std::string &key, const std::string &value);
  const std::vector<std::string> keys(const std::string &key = "") const;
  void del(const std::string &key);
  bool has(const std::string &key = "") const;
  bool isObject(const std::string &key) const;
  const std::string toJSON() const;
  void merge(const core::AutoPtr<Properties> &prop);
};
}; // namespace firefly::core