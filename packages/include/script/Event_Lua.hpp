#pragma once
#include "core/AutoPtr.hpp"
#include "runtime/Event.hpp"
#include "script/LuaValue.hpp"
namespace firefly::script {
class Event_Lua : public runtime::Event {
private:
  core::AutoPtr<LuaValue> _arg;
  std::string _type;

public:
  Event_Lua(const std::string &type, const core::AutoPtr<LuaValue> &arg)
      : _type(type), _arg(arg) {}
  const core::AutoPtr<LuaValue> &getArg() const { return _arg; };
  const std::string &getType() const { return _type; };
};
}; // namespace firefly::script