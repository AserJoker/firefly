#pragma once
#include "core/CompileString.hpp"
using namespace firefly;
template <core::CompileString name> struct Identity {
  friend auto getAccessorType(Identity<name>);
};

template <core::CompileString name, auto member> struct AccessorBinding {
  friend auto getAccessorType(Identity<name>) {
    return AccessorBinding<name, member>();
  }

  template <class C> void set(C &obj, auto &&value) {
    (obj.*member) = std::move(value);
  }
  template <class C> auto &get(const C &obj) { return (obj.*member); }

  template <class C, class... ARGS> auto call(C &obj, ARGS &&...args) {
    return (obj.*member)(std::forward<ARGS>(args)...);
  }
};

template <class C> struct Accessor {
  template <core::CompileString name> static auto &get(const C &c) {
    using AccessorType =
        decltype(getAccessorType(std::declval<Identity<name>>()));
    AccessorType acc{};
    return acc.get(c);
  }

  template <core::CompileString name> static void set(C &c, auto &&value) {
    using AccessorType =
        decltype(getAccessorType(std::declval<Identity<name>>()));
    AccessorType{}.set(c, value);
  }

  template <core::CompileString name, class... ARGS>
  static auto call(C &c, ARGS &&...args) {
    using AccessorType =
        decltype(getAccessorType(std::declval<Identity<name>>()));
    return AccessorType{}.call(c, std::forward<ARGS>(args)...);
  }
};
