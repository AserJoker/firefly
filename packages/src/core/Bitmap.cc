#include "core/Bitmap.hpp"
using namespace firefly;
using namespace firefly::core;
void Bitmap::enable(const std::string &name) { _bitmap[name] = true; }
void Bitmap::disable(const std::string &name) { _bitmap[name] = false; }
const bool Bitmap::check(const std::string &name) const {
  if (_bitmap.contains(name)) {
    return _bitmap.at(name);
  }
  return false;
}
void Bitmap::clear() { _bitmap.clear(); }