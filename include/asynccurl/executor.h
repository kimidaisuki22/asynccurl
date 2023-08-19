#pragma once
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <curl/curl.h>
#include <thread>

namespace asynccurl {
class Executor {
public:
  Executor();

private:
  CURLM *multi_handle_{};
  asio::io_context multi_context_;
  asio::io_context request_context_;
  asio::io_context::work multi_work_{multi_context_};
  asio::io_context::work request_work_{request_context_};
};
} // namespace asynccurl