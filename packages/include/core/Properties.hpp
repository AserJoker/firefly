#pragma once
#include "AutoPtr.hpp"
#include "core/Array.hpp"
#include "core/Buffer.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"


namespace firefly::core {
class Properties : public Object {
private:
  struct Item {
    core::Map<std::string, Item> children;
    std::string value;
    bool isObject;
    Item(const core::Map<std::string, Item> &item)
        : children(item), isObject(true) {}
    Item(const std::string &item) : value(item), isObject(false) {}
    Item() : isObject(true){};
  };
  using PObject = core::Map<std::string, Item>;
  Item _root;

private:
  const core::Array<std::string> parse(const std::string &key) const;
  const std::string parseJSONField(const std::string &field = "") const;
  const std::string encode(const std::string &source) const;
  const std::string decode(const std::string &source) const;
  void mergeField(Item &a, const Item &b);
  const std::string storeField(const std::string key, const Item &item) const;

public:
  Properties(const core::AutoPtr<Buffer> &buffer = 0);
  const std::string store() const;
  const std::string get(const std::string &key) const;
  void set(const std::string &key, const std::string &value);
  const core::Array<std::string> keys(const std::string &key = "") const;
  void del(const std::string &key);
  bool has(const std::string &key = "") const;
  bool isObject(const std::string &key) const;
  const std::string toJSON() const;
  void merge(const core::AutoPtr<Properties> &prop);
};
}; // namespace firefly::core