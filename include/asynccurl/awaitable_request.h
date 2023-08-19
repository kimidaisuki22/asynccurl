#pragma once
#include "asynccurl/executor.h"
#include "asynccurl/request.h"
#include "asynccurl/request_slot.h"
#include <coroutine>

namespace asynccurl {
class Awaitable_request {
public:
  Awaitable_request(Request_slot &request, Executor &executor)
      : slot_(&request), executor_(&executor) {}
  bool await_ready() const noexcept { return false; }
  void await_suspend(std::coroutine_handle<> handle) {
    slot_->set_on_finish_callback([handle] { handle(); });
    executor_->add_handle(*slot_);
  }

  void await_resume() const noexcept {}

  Request_slot *slot_;
  Executor *executor_;
};

} // namespace asynccurl