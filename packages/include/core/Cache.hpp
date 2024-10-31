#pragma once
#include "core/AutoPtr.hpp"
#include "core/Map.hpp"
#include "core/Object.hpp"
#include "core/Singleton.hpp"

namespace firefly::core {
template <class T> class Cache {
private:
  class CacheInstance : public Object {
  private:
    friend class Cache<T>;
    core::Map<core::String_t, core::AutoPtr<T>> _cache;

  public:
    CacheInstance(){};
  };

public:
  Cache(){};
  template <class... ARGS>
  static const core::AutoPtr<T> &get(const core::String_t &name,
                                     ARGS &&...args) {
    auto theCahce = core::Singleton<Cache<T>::CacheInstance>::instance();
    if (!theCahce->_cache.contains(name)) {
      core::AutoPtr<T> object = new T(std::forward<ARGS>(args)...);
      theCahce->_cache[name] = object;
    }
    return theCahce->_cache.at(name);
  }
  static void set(const core::String_t &name, const core::AutoPtr<T> &value) {
    auto theCahce = core::Singleton<Cache<T>::CacheInstance>::instance();
    theCahce->_cache[name] = value;
  }
  static void clear() {
    auto theCahce = core::Singleton<Cache<T>::CacheInstance>::instance();
    theCahce->_cache.clear();
  }
};
} // namespace firefly::core