#pragma once
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <curl/curl.h>
#include <curl/easy.h>
#include <spdlog/spdlog.h>
#include <thread>

namespace asynccurl {
class Executor {
public:
  Executor();
  void add_handle(CURL *handle) {
    curl_multi_add_handle(multi_handle_, handle);
  }
  void run() {
    int transfers_running;
    do {
      curl_multi_wait(multi_handle_, NULL, 0, 1000, NULL);
      curl_multi_perform(multi_handle_, &transfers_running);
      int info_count{};
      auto info = curl_multi_info_read(multi_handle_, &info_count);
      for (int i = 0; i < info_count; i++) {
        auto handle = info[i].easy_handle;
        char *url{};
        curl_easy_getinfo(handle, CURLINFO_EFFECTIVE_URL, url);
        SPDLOG_INFO("request finished: {}", url);
      }
    } while (transfers_running);
  }

private:
  CURLM *multi_handle_{};
  asio::io_context multi_context_;
  asio::io_context request_context_;
  asio::io_context::work multi_work_{multi_context_};
  asio::io_context::work request_work_{request_context_};
};
} // namespace asynccurl