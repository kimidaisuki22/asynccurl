#pragma once
#include "asynccurl/request.h"
#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <asio/io_context.hpp>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>
#include <spdlog/spdlog.h>
#include <thread>

namespace asynccurl {
class Executor {
public:
  Executor();
  void add_handle(Request &request) {
    curl_multi_add_handle(multi_handle_, request.build_handle());
  }
  void run() {
    int transfers_running;
    do {
      curl_multi_wait(multi_handle_, NULL, 0, 1000, NULL);
      curl_multi_perform(multi_handle_, &transfers_running);
      task_info(multi_handle_);
    } while (transfers_running);
  }
  void task_info(CURLM *handle) {
    CURLMsg *info{};
    do {
      int info_count{};
      info = curl_multi_info_read(handle, &info_count);
      if (info && (info->msg == CURLMSG_DONE)) {
        auto easy_handle = info->easy_handle;
        auto code = info->data.result;
        char *url;
        curl_easy_getinfo(easy_handle, CURLINFO_EFFECTIVE_URL, &url);
        asynccurl::Request *result;
        curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &result);
        if (result) {
          result->on_finish(code);
        }
        SPDLOG_DEBUG("request finished:[{}] {}", (void *)easy_handle, url);

        curl_multi_remove_handle(handle, easy_handle);
      }
    } while (info);
  }

private:
  CURLM *multi_handle_{};
  asio::io_context multi_context_;
  asio::io_context request_context_;
  asio::io_context::work multi_work_{multi_context_};
  asio::io_context::work request_work_{request_context_};
};
} // namespace asynccurl