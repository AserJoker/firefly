#pragma once
#include "core/Object.hpp"
#include <string>
#include <vector>
namespace firefly::runtime {
class Application : public core::Object {
private:
  std::vector<std::string> _args;
  int _nExitCode;

private:
  static void CoSchedule();

protected:
  virtual void Initialize();

public:
  Application(int argc, char *argv[]);
  ~Application() override;
  int run();
  void exit(int nExitCode = 0);
};
} // namespace firefly::runtime