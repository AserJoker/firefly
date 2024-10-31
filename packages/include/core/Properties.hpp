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
    core::Map<core::String_t, Item> children;
    core::String_t value;
    core::Boolean_t isObject;
    Item(const core::Map<core::String_t, Item> &item)
        : children(item), isObject(true) {}
    Item(const core::String_t &item) : value(item), isObject(false) {}
    Item() : isObject(true){};
  };
  using PObject = core::Map<core::String_t, Item>;
  Item _root;

private:
  const core::Array<core::String_t> parse(const core::String_t &key) const;
  const core::String_t parseJSONField(const core::String_t &field = "") const;
  const core::String_t encode(const core::String_t &source) const;
  const core::String_t decode(const core::String_t &source) const;
  void mergeField(Item &a, const Item &b);
  const core::String_t storeField(const core::String_t key,
                                  const Item &item) const;

public:
  Properties(const core::AutoPtr<Buffer> &buffer = 0);
  const core::String_t store() const;
  const core::String_t get(const core::String_t &key) const;
  void set(const core::String_t &key, const core::String_t &value);
  const core::Array<core::String_t> keys(const core::String_t &key = "") const;
  void del(const core::String_t &key);
  core::Boolean_t has(const core::String_t &key = "") const;
  core::Boolean_t isObject(const core::String_t &key) const;
  const core::String_t toJSON() const;
  void merge(const core::AutoPtr<Properties> &prop);
};
}; // namespace firefly::core