#pragma once
#include "Atom.hpp"
#include "Context.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
#include <vector>
namespace firefly::script {

class Value : public core::Object {
public:
  enum class Operation {
    CONNECT,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    EQ,
    NE,
    GT,
    GE,
    LT,
    LE,
    AND,
    OR,
    XOR,
    NOT,
    SHR,
    SHL,
  };

private:
  core::AutoPtr<Value> optAdd(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optConnect(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another);
  core::AutoPtr<Value> optSub(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optMul(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optDiv(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optMod(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optAnd(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optOr(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optXor(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optEq(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optNe(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optGt(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optGe(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optLt(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optLe(core::AutoPtr<Context> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optShl(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optShr(core::AutoPtr<Context> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optNot(core::AutoPtr<Context> ctx);

private:
  Atom *_atom;

public:
  using Stack = std::vector<core::AutoPtr<Value>>;
  using FunctionHandle = std::function<Stack(core::AutoPtr<Context>, Stack)>;

public:
  Value(Atom *atom);
  Atom::Type getType(core::AutoPtr<Context> ctx);
  std::string getTypeName(core::AutoPtr<Context> ctx);
  double toNumber(core::AutoPtr<Context> ctx);
  bool toBoolean(core::AutoPtr<Context> ctx);
  std::string toString(core::AutoPtr<Context> ctx);
  core::AutoPtr<Value> setNumber(core::AutoPtr<Context> ctx,
                                 const double &value);
  core::AutoPtr<Value> setBoolean(core::AutoPtr<Context> ctx,
                                  const bool &value);
  core::AutoPtr<Value> setString(core::AutoPtr<Context> ctx,
                                 const std::string &value);
  core::AutoPtr<Value> setNil(core::AutoPtr<Context> ctx);
  core::AutoPtr<Value> setObject(core::AutoPtr<Context> ctx);
  core::AutoPtr<Value> setArray(core::AutoPtr<Context> ctx);
  core::AutoPtr<Value> setFunction(core::AutoPtr<Context> ctx,
                                   const FunctionHandle &func);
  Stack call(core::AutoPtr<Context> ctx, Stack args);

  core::AutoPtr<Value> setField(core::AutoPtr<Context> ctx,
                                const std::string &name,
                                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getField(core::AutoPtr<Context> ctx,
                                const std::string &name);

  std::vector<std::string> getKeys(core::AutoPtr<Context> ctx);

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
  core::AutoPtr<Value> arithmetic(core::AutoPtr<Context> ctx,
                                  core::AutoPtr<Value> another,
                                  Operation operation);
  Atom *getAtom();
};
} // namespace firefly::script