#pragma once

#include "core/AutoPtr.hpp"
#include "core/Object.hpp"
#include "runtime/Event.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace firefly::runtime {
    class EventBus : public core::Object {
    private:
        struct Listener {
            std::function<void(Event &)> callback;
            std::string identity;
            bool enable;
        };

    private:
        std::unordered_map<std::string, std::vector<Listener>> _listeners;

    public:
        template<class L, class E>
        void on(core::AutoPtr<L> listener, void (L::*func)(E &)) {
            std::string type = typeid(E).name();
            auto &listeners = _listeners[type];
            listeners.push_back({.callback = [=](Event &e) -> void {
                (const_cast<L &>(*listener).*func)((E &) e);
            },
                                        .identity = typeid(L).name(),
                                        .enbale = true});
        }

        template<class L, class E>
        void off(core::AutoPtr<L> listener, void (L::*)(E &)) {
            std::string type = typeid(E).name();
            if (_listeners.contains(type)) {
                auto &listeners = _listeners.at(type);
                for (auto it = listeners.begin(); it != listeners.end(); it++) {
                    if (it->identity == typeid(E).name()) {
                        it->enable = false;
                        return;
                    }
                }
            }
        }

        template<class E, class... ARGS>
        void emit(ARGS... args) {
            std::string type = typeid(E).name();
            if (_listeners.contains(type)) {
                auto &listeners = _listeners.at(type);
                E event(args...);
                for (auto &[callback, _, enbale]: listeners) {
                    if (enbale) {
                        if (!event.isCanceled()) {
                            callback(event);
                        } else {
                            break;
                        }
                    }
                }
                for (auto it = listeners.begin(); it != listeners.end(); it++) {
                    if (!it->enable) {
                        listeners.erase(it);
                        it = listeners.begin();
                    }
                }
            }
        };
    };
} // namespace firefly::runtime