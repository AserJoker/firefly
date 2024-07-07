#pragma once
#include "core/AutoPtr.hpp"
#include "core/Coroutine.hpp"
#include "core/EventLoop.hpp"
#include "core/Injector.hpp"
#include "core/Object.hpp"
#include "core/Provider.hpp"
#include "core/Singleton.hpp"

#include "runtime/Application.hpp"
#include "runtime/BaseApplication.hpp"
#include "runtime/CmdLine.hpp"
#include "runtime/ConfigProvider.hpp"
#include "runtime/Event.hpp"
#include "runtime/EventBus.hpp"
#include "runtime/Logger.hpp"

#include "injector.hpp"

namespace firefly {
void InitFirefly();
}