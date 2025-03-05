#pragma once
#include "../util/JSAllocator.hpp"
#include "JSBase.hpp"
#include <vector>

class JSAtom {
private:
  static inline std::vector<JSAtom *> _destroyed = {};

private:
  JSAllocator *_allocator;
  std::vector<JSAtom *> _parents;
  std::vector<JSAtom *> _children;
  JSBase *_data;

public:
  JSAtom(JSAllocator *allocator, JSAtom *parent, JSBase *data);

  JSAtom(JSAllocator *allocator);

  ~JSAtom();

  inline JSAllocator *getAllocator() { return _allocator; }

  void addChild(JSAtom *child);

  void removeChild(JSAtom *child);

  inline const JSBase *getData() const { return _data; }

  inline JSBase *getData() { return _data; }

  void setData(JSBase *data);

  inline auto getType() const { return _data->getType(); }

  template <class T> bool isTypeof() const { return _data->isTypeof<T>(); }

public:
  static void gc(JSAllocator *allocator);
};