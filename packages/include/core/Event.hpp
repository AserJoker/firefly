#pragma once
#include <string>
namespace firefly::core {
class Event {
private:
  std::string _type;

public:
  Event(const std::string &type = "");
  void setType(const std::string &type);
  const std::string &getType() const;
};
} // namespace firefly::core