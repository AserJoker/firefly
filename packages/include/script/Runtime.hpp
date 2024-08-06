#pragma once
#include "core/Object.hpp"
#include <functional>
namespace firefly::script {
class Runtime : public core::Object {
public:
  struct Adapter {
    std::function<void(const std::string &filename, const std::string &source)>
        eval;
  };

protected:
  Adapter _adapter;

public:
  ~Runtime() override{};
  void eval(const std::string &filename, const std::string &source) {
    _adapter.eval(filename, source);
  };
};
template <class T> class AdapterRuntime : public Runtime {
public:
  AdapterRuntime() {
    _adapter.eval = T::eval;
  };
};
} // namespace firefly::script