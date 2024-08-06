#pragma once
#include "Atom.hpp"
#include "Context.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
namespace firefly::script {

class Value : public core::Object {

private:
  Atom *_atom;

public:
  using Stack = std::vector<core::AutoPtr<Value>>;
  using FunctionHandle = std::function<Stack(core::AutoPtr<Context>, Stack)>;

public:
  Value(Atom *atom);
  Atom::Type getType();
  double toNumber();
  int64_t toInteger();
  bool toBoolean();
  std::string toString();
  core::AutoPtr<Value> setNumber(const double &value);
  core::AutoPtr<Value> setInteger(const int64_t &value);
  core::AutoPtr<Value> setBoolean(const bool &value);
  core::AutoPtr<Value> setString(const std::string &value);
  core::AutoPtr<Value> setNil();
  core::AutoPtr<Value> setObject();
  core::AutoPtr<Value> setArray();
  core::AutoPtr<Value> setFunction(const FunctionHandle &func);

  Stack call(core::AutoPtr<Context> ctx, Stack args);

  core::AutoPtr<Value> setField(core::AutoPtr<Context> ctx,
                                const std::string &name,
                                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getField(core::AutoPtr<Context> ctx,
                                const std::string &name);
  core::AutoPtr<Value> setIndex(core::AutoPtr<Context> ctx,
                                const uint32_t &name,
                                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getIndex(core::AutoPtr<Context> ctx,
                                const uint32_t &name);
  uint32_t getLength(core::AutoPtr<Context> ctx);

  core::AutoPtr<Value> setRawField(core::AutoPtr<Context> ctx,
                                   const std::string &name,
                                   core::AutoPtr<Value> field);
  core::AutoPtr<Value> getRawField(core::AutoPtr<Context> ctx,
                                   const std::string &name);
  core::AutoPtr<Value> setRawIndex(core::AutoPtr<Context> ctx,
                                   const uint32_t &name,
                                   core::AutoPtr<Value> field);
  core::AutoPtr<Value> getRawIndex(core::AutoPtr<Context> ctx,
                                   const uint32_t &name);
  uint32_t getRawLength(core::AutoPtr<Context> ctx);

  core::AutoPtr<Value> getMetadata(core::AutoPtr<Context> ctx);
  core::AutoPtr<Value> setMetadata(core::AutoPtr<Value> value);
  core::AutoPtr<Value> arithmetic(core::AutoPtr<Value> another, int operation);
  Atom *getAtom();
};
} // namespace firefly::script