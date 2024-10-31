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
#include "core/Type.hpp"
#include "core/Value.hpp"
#include <functional>

namespace firefly::document {
class Node : public core::Object {
private:
  class Property : public core::AnyPtr {
  private:
    bool _readonly{};

  public:
    Property() = default;
    Property(core::String_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Integer_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Unsigned_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Float_t *ptr) : core::AnyPtr(ptr){};
    Property(core::Boolean_t *ptr) : core::AnyPtr(ptr){};

    Property(const Property &attr) = default;
    Property &operator=(const Property &another) = default;

    bool isReadonly() const { return _readonly; };
    void setReadonly(bool readonly) { _readonly = readonly; };
  };

private:
  static core::Map<core::String_t, Node *> _indexedNodes;

private:
  core::Array<core::AutoPtr<Node>> _children;
  Node *_parent;
  core::String_t _id;
  core::String_t _name;

  core::Map<core::String_t, Property> _properties;

  core::Map<core::String_t, core::Array<core::String_t>> _propertyGroups;

  core::Boolean_t _ready;

  core::String_t _currentPropertyGroup;

  core::Array<core::String_t> _propertyGroupStack;

  core::Map<core::String_t, core::Array<std::function<void()>>> _propWatchers;

protected:
  template <class T>
  void defineProperty(const core::String_t &name, T &property,
                      bool readonly = false) {
    _properties[name] = &property;
    _properties[name].setReadonly(readonly);
    auto tmp = name;
    auto pos = tmp.find_last_of('.');
    while (pos != core::String_t::npos) {
      auto group = tmp.substr(0, pos);
      auto &groups = _propertyGroups[group];
      groups.pushBack(tmp);
      tmp = group;
      pos = tmp.find_last_of('.');
    }
  }

  inline void defineProperty(const core::String_t &name, core::Rect<> &property,
                             bool readonly = false) {
    defineProperty(name + ".x", property.x, readonly);
    defineProperty(name + ".y", property.y, readonly);
    defineProperty(name + ".width", property.width, readonly);
    defineProperty(name + ".height", property.height, readonly);
  }

  inline void defineProperty(const core::String_t &name,
                             core::Color<> &property, bool readonly = false) {
    defineProperty(name + ".r", property.r, readonly);
    defineProperty(name + ".g", property.g, readonly);
    defineProperty(name + ".b", property.b, readonly);
    defineProperty(name + ".a", property.a, readonly);
  }

  inline void defineProperty(const core::String_t &name,
                             core::Point<> &property, bool readonly = false) {
    defineProperty(name + ".x", property.x, readonly);
    defineProperty(name + ".y", property.y, readonly);
  }

  inline void defineProperty(const core::String_t &name, core::Size<> &property,
                             bool readonly = false) {
    defineProperty(name + ".width", property.width, readonly);
    defineProperty(name + ".height", property.height, readonly);
  }

  template <class T>
  void watchProp(const core::String_t &prop, void (T::*callback)()) {
    auto &watchers = _propWatchers[prop];
    watchers.pushBack(std::function(
        [=, this]() -> void { (dynamic_cast<T *>(this)->*callback)(); }));
  }

  inline core::Boolean_t isProperty(const core::String_t &key,
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

  void setName(const core::String_t &name);
  const core::String_t &getName() const;

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