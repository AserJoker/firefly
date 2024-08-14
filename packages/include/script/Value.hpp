#pragma once
#include "Atom.hpp"
#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include <functional>
#include <vector>
namespace firefly::script {
class Script;

class Value : public core::Object {
public:
  enum class OPERATION {
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
  core::AutoPtr<Value> optAdd(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optConnect(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another);
  core::AutoPtr<Value> optSub(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optMul(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optDiv(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optMod(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optAnd(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optOr(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optXor(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optEq(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optNe(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optGt(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optGe(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optLt(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optLe(core::AutoPtr<Script> ctx,
                             core::AutoPtr<Value> another);
  core::AutoPtr<Value> optShl(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optShr(core::AutoPtr<Script> ctx,
                              core::AutoPtr<Value> another);
  core::AutoPtr<Value> optNot(core::AutoPtr<Script> ctx);

private:
  Atom *_atom;

public:
  using Stack = std::vector<core::AutoPtr<Value>>;
  using FunctionHandle = std::function<Stack(core::AutoPtr<Script>, Stack)>;

public:
  Value(Atom *atom);
  Atom::TYPE getType(core::AutoPtr<Script> ctx);
  std::string getTypeName(core::AutoPtr<Script> ctx);
  double toNumber(core::AutoPtr<Script> ctx);
  bool toBoolean(core::AutoPtr<Script> ctx);
  std::string toString(core::AutoPtr<Script> ctx);
  core::AutoPtr<Value> setNumber(core::AutoPtr<Script> ctx,
                                 const double &value);
  core::AutoPtr<Value> setBoolean(core::AutoPtr<Script> ctx, const bool &value);
  core::AutoPtr<Value> setString(core::AutoPtr<Script> ctx,
                                 const std::string &value);
  core::AutoPtr<Value> setNil(core::AutoPtr<Script> ctx);
  core::AutoPtr<Value> setObject(core::AutoPtr<Script> ctx);
  core::AutoPtr<Value> setArray(core::AutoPtr<Script> ctx);
  core::AutoPtr<Value> setFunction(core::AutoPtr<Script> ctx,
                                   const FunctionHandle &func);
  Stack call(core::AutoPtr<Script> ctx, Stack args);

  core::AutoPtr<Value> setField(core::AutoPtr<Script> ctx,
                                const std::string &name,
                                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getField(core::AutoPtr<Script> ctx,
                                const std::string &name);

  std::vector<std::string> getKeys(core::AutoPtr<Script> ctx);

  core::AutoPtr<Value> setIndex(core::AutoPtr<Script> ctx, const uint32_t &name,
                                core::AutoPtr<Value> field);
  core::AutoPtr<Value> getIndex(core::AutoPtr<Script> ctx,
                                const uint32_t &name);
  uint32_t getLength(core::AutoPtr<Script> ctx);

  core::AutoPtr<Value> setRawField(core::AutoPtr<Script> ctx,
                                   const std::string &name,
                                   core::AutoPtr<Value> field);
  core::AutoPtr<Value> getRawField(core::AutoPtr<Script> ctx,
                                   const std::string &name);
  core::AutoPtr<Value> setRawIndex(core::AutoPtr<Script> ctx,
                                   const uint32_t &name,
                                   core::AutoPtr<Value> field);
  core::AutoPtr<Value> getRawIndex(core::AutoPtr<Script> ctx,
                                   const uint32_t &name);
  uint32_t getRawLength(core::AutoPtr<Script> ctx);

  core::AutoPtr<Value> getMetadata(core::AutoPtr<Script> ctx);
  core::AutoPtr<Value> setMetadata(core::AutoPtr<Value> value);
  core::AutoPtr<Value> setOpaque(core::AutoPtr<core::Object> obj);
  core::AutoPtr<core::Object> getOpaque();
  core::AutoPtr<Value> arithmetic(core::AutoPtr<Script> ctx,
                                  core::AutoPtr<Value> another,
                                  OPERATION operation);
  Atom *getAtom();
  std::string toJSON(core::AutoPtr<Script> ctx);
  std::string toYAML(core::AutoPtr<Script> ctx);
  static core::AutoPtr<Value> parseJSON(core::AutoPtr<Script> ctx,
                                        const std::string &source);
  static core::AutoPtr<Value> parseYAML(core::AutoPtr<Script> ctx,
                                        const std::string &source);
};
} // namespace firefly::script