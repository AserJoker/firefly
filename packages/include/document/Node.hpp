#pragma once
#include "core/AnyPtr.hpp"
#include "core/Array.hpp"
#include "core/AutoPtr.hpp"
#include "core/Color.hpp"
#include "core/CompileString.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Point.hpp"
#include "core/Rect.hpp"
#include "core/Value.hpp"
#include <functional>


namespace firefly::document {
class Node : public core::Object {
private:
  class Property : public core::AnyPtr {
  private:
  public:
    Property() = default;
    Property(core::Byte_t *ptr) : core::AnyPtr(ptr){};
    Property(core::String_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Integer_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Unsigned_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Float_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Boolean_t *ptr) : core::AnyPtr(ptr){};

    Property(core::Array<core::String_t> *ptr) : core::AnyPtr(ptr){};
    Property(core::Array<core::Integer_t> *ptr) : core::AnyPtr(ptr){};
    Property(core::Array<core::Unsigned_t> *ptr) : core::AnyPtr(ptr){};
    Property(core::Array<core::Float_t> *ptr) : core::AnyPtr(ptr){};
    Property(core::Array<core::Boolean_t> *ptr) : core::AnyPtr(ptr){};

    Property(const Property &attr) = default;
    Property &operator=(const Property &another) = default;
  };

private:
  static core::Map<core::String_t, Node *> _indexedNodes;

private:
  core::Array<core::AutoPtr<Node>> _children;
  Node *_parent;
  core::String_t _id;

  core::Map<core::String_t, Property> _properties;

  core::Map<core::String_t, core::Array<core::String_t>> _propertyGroups;

  bool _ready;

  core::String_t _currentPropertyGroup;
  core::Array<core::String_t> _propertyGroupStack;

  core::Map<std::string, core::Array<std::function<void()>>> _propWatchers;

protected:
  template <class T>
  void defineProperty(const core::String_t &name, T &property) {
    _properties[name] = &property;
    auto tmp = name;
    auto pos = tmp.find_last_of('.');
    while (pos != std::string::npos) {
      auto group = tmp.substr(0, pos);
      auto &groups = _propertyGroups[group];
      groups.pushBack(tmp);
      tmp = group;
      pos = tmp.find_last_of('.');
    }
  }

  inline void defineProperty(const core::String_t &name,
                             core::Rect<> &property) {
    defineProperty(name + ".x", property.x);
    defineProperty(name + ".y", property.y);
    defineProperty(name + ".width", property.width);
    defineProperty(name + ".height", property.height);
  }

  inline void defineProperty(const core::String_t &name,
                             core::Color<> &property) {
    defineProperty(name + ".r", property.r);
    defineProperty(name + ".g", property.g);
    defineProperty(name + ".b", property.b);
    defineProperty(name + ".a", property.a);
  }

  inline void defineProperty(const core::String_t &name,
                             core::Point<> &property) {
    defineProperty(name + ".x", property.x);
    defineProperty(name + ".y", property.y);
  }

  inline void defineProperty(const core::String_t &name,
                             core::Size<> &property) {
    defineProperty(name + ".width", property.width);
    defineProperty(name + ".height", property.height);
  }

  template <class T>
  void watchProp(const core::String_t &prop, void (T::*callback)()) {
    auto &watchers = _propWatchers[prop];
    watchers.pushBack(std::function(
        [=, this]() -> void { (dynamic_cast<T *>(this)->*callback)(); }));
  }

  inline bool isProperty(const core::String_t &key,
                         const core::String_t &property) {
    return property == key || property.starts_with(key + ".");
  }

  template <class T> T *findParent() {
    auto parent = _parent;
    while (parent) {
      auto p = dynamic_cast<T *>(parent);
      if (p != nullptr) {
        return p;
      }
      parent = parent->_parent;
    }
    return nullptr;
  }

  void onPropChange(const core::String_t &name) {
    for (auto &[prop, watchers] : _propWatchers) {
      if (isProperty(prop, name)) {
        for (auto &watcher : watchers) {
          watcher();
        }
      }
    }
  };

public:
  Node();
  ~Node() override;

  const core::Array<core::AutoPtr<Node>> &getChildren() const;
  core::Array<core::AutoPtr<Node>> &getChildren();

  core::AutoPtr<Node> getParent();
  const core::AutoPtr<Node> getParent() const;

  void setIdentity(const core::String_t &id);
  const core::String_t &getIdentity() const;

  void appendChild(core::AutoPtr<Node> child);
  void removeChild(core::AutoPtr<Node> child);

  const core::Value getProperty(const core::String_t &name) const;
  virtual void setProperty(const core::String_t &name,
                           const core::Value &value);
  inline void setProperty(const core::String_t &name,
                          const core::Rect<> &rect) {
    beginPropGroup(name);
    setProperty("x", rect.x);
    setProperty("y", rect.y);
    setProperty("width", rect.width);
    setProperty("height", rect.height);
    endPropGroup();
  }
  inline void setProperty(const core::String_t &name,
                          const core::Size<> &size) {
    beginPropGroup(name);
    setProperty("width", size.width);
    setProperty("height", size.height);
    endPropGroup();
  }
  inline void setProperty(const core::String_t &name,
                          const core::Point<> &position) {
    beginPropGroup(name);
    setProperty("x", position.x);
    setProperty("y", position.y);
    endPropGroup();
  }
  void beginPropGroup(const core::String_t &name);
  void endPropGroup();

  void onMainLoop();

protected:
  virtual void onTick();
  virtual void onLoad();
  virtual void onUnload();

private:
  static inline core::Map<core::String_t, std::function<core::AutoPtr<Node>()>>
      _nodeConstructors;

public:
  template <core::CompileString type, class T> struct Register {
  public:
    static void registerNode() {
      _nodeConstructors[type.value] = []() -> core::AutoPtr<Node> {
        return new T();
      };
    };
  };

public:
  static core::AutoPtr<Node> create(const core::String_t &type);
  static core::AutoPtr<Node> select(const core::String_t &identity);
  static core::AutoPtr<Node> load(const core::String_t &path);
};
}; // namespace firefly::document