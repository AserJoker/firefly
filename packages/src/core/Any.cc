#include "core/Any.hpp"
#include "core/Value.hpp"
using namespace firefly;
using namespace firefly::core;
Any::Any(const Value &value) : _value(value) {}

const Value &Any::getValue() const { return _value; }

Value &Any::getValue() { return _value; }