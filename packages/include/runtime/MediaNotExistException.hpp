#pragma once
#include "core/Exception.hpp"
namespace firefly::runtime {
using MediaNotExistException = core::RuntimeException<"MediaNotExistException">;
}