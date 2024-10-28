#pragma once

#include <algorithm>
namespace firefly::core {
template <size_t N> struct CompileString {
  constexpr CompileString(const char (&str)[N]) { std::copy_n(str, N, value); }

  char value[N]{};

  constexpr static inline size_t length = N;
};
} // namespace firefly::core