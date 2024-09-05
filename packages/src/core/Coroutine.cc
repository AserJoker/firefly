
#ifdef __unix__
#include "core/Coroutine.hpp"
#include <list>
#include <stddef.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#ifndef NEO_COROUTINE_STACK_SIZE
#define NEO_COROUTINE_STACK_SIZE 4096
#endif

using namespace firefly;
using namespace firefly::core;

struct CoContext {
  ucontext_t cpuinfo;
  char stack[NEO_COROUTINE_STACK_SIZE];
  std::function<void()> exec;
  bool enable;
};

static ucontext_t master;
static ucontext_t *current;

static std::list<CoContext> routines;
static std::list<CoContext>::iterator currentRoutine;
void run(void *context) {
  CoContext *coroutine = (CoContext *)context;
  coroutine->exec();
  coroutine->enable = false;
  swapcontext(&coroutine->cpuinfo, &master);
}
void Coroutine::init() {
  getcontext(&master);
  currentRoutine = routines.end();
  current = &master;
}
void Coroutine::start(const std::function<void()> &func) {
  routines.push_back({.exec = func, .enable = true});
  auto &last = *routines.rbegin();
  getcontext(&last.cpuinfo);
  last.cpuinfo.uc_stack.ss_sp = last.stack;
  last.cpuinfo.uc_stack.ss_size = NEO_COROUTINE_STACK_SIZE;
  last.cpuinfo.uc_stack.ss_flags = 0;
  makecontext(&last.cpuinfo, (void (*)())run, 1, (void *)&last);
}
void Coroutine::yield() {
  auto now = current;
  if (current == &master) {
    currentRoutine++;
    if (currentRoutine == routines.end()) {
      currentRoutine = routines.begin();
    }
    while (!routines.empty() && !currentRoutine->enable) {
      auto next = currentRoutine;
      next++;
      routines.erase(currentRoutine);
      currentRoutine = next;
      if (currentRoutine == routines.end()) {
        currentRoutine = routines.begin();
      }
    }
    if (routines.empty()) {
      return;
    }
    current = &currentRoutine->cpuinfo;
  } else {
    current = &master;
  }
  swapcontext(now, current);
}
bool Coroutine::done() { return routines.empty(); }

#endif