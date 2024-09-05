#ifdef _WIN32
#include "core/Coroutine.hpp"
#include <list>
#include <windows.h>

#ifndef NEO_COROUTINE_STACK_SIZE
#define NEO_COROUTINE_STACK_SIZE 4096
#endif
using namespace firefly;
using namespace firefly::core;

struct CoContext {
  LPVOID fiber;
  std::function<void()> exec;
  bool enable;
};

static LPVOID master;
static LPVOID current;

static std::list<CoContext> routines;
static std::list<CoContext>::iterator currentRoutine;
void run(void *context) {
  CoContext *coroutine = (CoContext *)context;
  coroutine->exec();
  coroutine->enable = false;
  SwitchToFiber(master);
}
void Coroutine::init() {
  master = ConvertThreadToFiber(NULL);
  currentRoutine = routines.end();
  current = master;
}
void Coroutine::start(const std::function<void()> &func) {
  routines.push_back({nullptr, func, true});
  auto &last = *routines.rbegin();
  last.fiber = CreateFiber(NEO_COROUTINE_STACK_SIZE,
                           (LPFIBER_START_ROUTINE)&run, (void *)&last);
}
void Coroutine::yield() {
  if (current == master) {
    currentRoutine++;
    if (currentRoutine == routines.end()) {
      currentRoutine = routines.begin();
    }
    while (!routines.empty() && !currentRoutine->enable) {
      auto next = currentRoutine;
      next++;
      DeleteFiber(currentRoutine->fiber);
      routines.erase(currentRoutine);
      currentRoutine = next;
      if (currentRoutine == routines.end()) {
        currentRoutine = routines.begin();
      }
    }
    if (routines.empty()) {
      return;
    }
    current = currentRoutine->fiber;
  } else {
    current = master;
  }
  SwitchToFiber(current);
}
bool Coroutine::done() { return routines.empty(); }
#endif