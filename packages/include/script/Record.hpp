#pragma once
#include "Context.hpp"
#include "Value.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "script/Atom.hpp"
#include <string>
#include <unordered_map>
namespace firefly::script {
class Record : public core::Object {
private:
  Atom *_self;
  std::unordered_map<std::string, Atom *> _fields;

public:
  Record(Atom *atom);
  void setField(core::AutoPtr<Context> ctx, const std::string &name,
                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getField(core::AutoPtr<Context> ctx,
                                const std::string &name);
  std::unordered_map<std::string, Atom *> &getFields() { return _fields; }
  bool hasField(const std::string &name) { return _fields.contains(name); }
  void removeField(core::AutoPtr<Context> ctx, const std::string &name);
  uint32_t getLength(core::AutoPtr<Context> ctx);
};
} // namespace firefly::script