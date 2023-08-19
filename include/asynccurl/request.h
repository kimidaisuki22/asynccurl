#pragma once
#include "asynccurl/executor.h"
namespace asynccurl {
class Request {
public:
  Request(Executor &executor);

private:
  Executor *executor_{};
};
} // namespace asynccurl