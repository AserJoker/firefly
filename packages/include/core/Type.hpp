#pragma once

#include <type_traits>

namespace firefly::core {
namespace {

template <class T, typename K, class... NEXT> struct TypeValueGetter {
  using nextType = typename TypeValueGetter<T, NEXT...>::type;
  using type =
      std::conditional_t<std::is_same_v<typename K::valueType, T>, K, nextType>;
};

template <class T, typename K> struct TypeValueGetter<T, K> {
  using type =
      std::conditional_t<std::is_same_v<typename K::valueType, T>, K, void>;
};
}; // namespace

template <class T, class... TYPES> struct TypeSelect {
  using type = TypeValueGetter<std::remove_all_extents_t<T>, TYPES...>::type;
};

template <class T, class... NEXT> struct KeyType {
  using nextType = KeyType<NEXT...>::type;
  using type =
      std::enable_if_t<std::is_same_v<typename T::keyType, nextType>, nextType>;
};

template <class T> struct KeyType<T> {
  using type = typename T::keyType;
};
}; // namespace firefly::core