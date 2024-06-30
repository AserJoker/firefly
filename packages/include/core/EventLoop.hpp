#pragma once
#include "core/Object.hpp"
#include <chrono>
#include <functional>
#include <list>
namespace firefly::core {
using namespace std::chrono;
class EventLoop : public Object {
private:
  struct Task {
    std::function<void()> callee;
    uint32_t id;
    system_clock::duration timeout;
    system_clock::time_point start;
    bool canceled;
  };
  std::list<Task> _tasks;
  bool _running;

public:
  EventLoop();
  void run();
  void stop();
  const uint32_t &start(const std::function<void()> &callee,
                        system_clock::duration timeout = 0s);
  void cancel(const uint32_t &id);
};
} // namespace firefly::core