#include "core/EventLoop.hpp"
#include <chrono>
#include <thread>

using namespace firefly::core;

void EventLoop::cancel(const uint32_t &id) {
    for (auto &task: _tasks) {
        if (task.id == id) {
            task.canceled = true;
            break;
        }
    }
}

void EventLoop::nextTick() {
    auto task = _tasks.begin();
    while (task != _tasks.end() && task->canceled) {
        _tasks.erase(task);
        task = _tasks.begin();
    }
    if (task == _tasks.end()) {
        return;
    }
    if (system_clock::now() - task->start >= task->timeout) {
        task->callee();
        _tasks.erase(task);
    } else {
        _tasks.push_back(*task);
        _tasks.erase(task);
        std::this_thread::sleep_for(4ms);
    }
}

const uint32_t &EventLoop::start(const std::function<void()> &callee,
                                 system_clock::duration timeout) {
    static uint32_t counter = 0;
    _tasks.push_back({callee, ++counter, timeout, system_clock::now(), false});
    return _tasks.rbegin()->id;
}
