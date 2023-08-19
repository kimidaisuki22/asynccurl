#pragma once
#include "asynccurl/executor.h"
#include "asynccurl/request.h"
#include <memory>
namespace asynccurl {

inline void spawn(Executor &executor, auto &resumable) {
  executor.add_task([&resumable] { resumable.resume(); });
}
} // namespace asynccurl