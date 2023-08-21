#pragma once
#include "asynccurl/request.h"
#include <atomic>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>
#include <deque>
#include <functional>
#include <mutex>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

namespace asynccurl {
class Executor {
public:
  Executor();
  void add_handle(Request &request) {
    // curl_multi_add_handle(multi_handle_, request.build_handle());
    add_task([this, &request] {
      curl_multi_add_handle(multi_handle_, request.build_handle());
    });
  }
  void run() {
    int transfers_running{};
    int task;
    do {
      curl_multi_wait(multi_handle_, NULL, 0, 100, NULL);
      curl_multi_perform(multi_handle_, &transfers_running);
      task_info(multi_handle_);
      task = {};
      {
        std::unique_lock lock{queue_mutex_};
        while (!back_queue_.empty()) {
          function_queue_.push_back(back_queue_.front());
          back_queue_.pop_front();
        }
      }
      while (!function_queue_.empty()) {
        function_queue_.front()();
        function_queue_.pop_front();
        task++;
      }
      SPDLOG_DEBUG("running state transfer {} task {}", transfers_running,
                   task);
    } while (transfers_running || task);
  }
  void run_looping(){
    while(!stoped_){
      run();
      std::unique_lock lock{queue_mutex_};
      wait_new_task_.wait(lock,[this]{return stoped_.load();});
    }
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

  void add_task(std::function<void()> task) {
    std::unique_lock lock{queue_mutex_};
    back_queue_.push_back(task);
    wait_new_task_.notify_one();
  }

  void request_stop(){
    // failed to notify?
    stoped_ = true;
    wait_new_task_.notify_one();
  }

private:
  CURLM *multi_handle_{};

  std::deque<std::function<void()>> function_queue_;
  std::deque<std::function<void()>> back_queue_;
  std::mutex queue_mutex_;
  std::atomic_bool stoped_{false};
  std::condition_variable wait_new_task_;

};
} // namespace asynccurl