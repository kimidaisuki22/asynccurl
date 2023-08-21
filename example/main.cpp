#include "asynccurl/awaitable_request.h"
#include "asynccurl/behave.h"
#include "asynccurl/request.h"
#include "asynccurl/request_slot.h"
#include "asynccurl/spwan.h"
#include "asynccurl/task.h"
#include "asynccurl/write_buffer.h"
#include <asynccurl/executor.h>
#include <atomic>
#include <barrier>
#include <chrono>
#include <coroutine>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <string>
#include <thread>
#include <vector>

// Task<void> is_right() {
//   SPDLOG_INFO("That's all right");
//   co_return;
// }
Task<std::string> fetch(std::string url, asynccurl::Executor &executor) {
  SPDLOG_INFO("deploy coroutine.");
  auto buffer = std::make_shared<asynccurl::Write_string>();
  asynccurl::Request_slot req{url};
  req.set_write_buffer(buffer);
  static int i{};
  i++;
  if (i % 2 == 0) {
    asynccurl::follow_redirect(req, true);
  }

  co_await asynccurl::Awaitable_request{req, executor};
  SPDLOG_TRACE("coroutine resume:{}", buffer->buffer_);

  co_return buffer->buffer_;
}

Task<std::vector<std::string>> fetch_lots(std::vector<std::string> urls,
                                          asynccurl::Executor &executor) {
  std::vector<std::string> results;
  int i{};
  for (auto u : urls) {
    SPDLOG_INFO("start fetch url[{}]", i);
    auto r = co_await fetch(u, executor);
    results.push_back(r);
    i++;
  }
  SPDLOG_INFO("We fetched {} urls, fetch_lots finished.", results.size());
  for (auto u : results) {
    SPDLOG_TRACE("url: {}", u);
  }
  co_return results;
}
int main() {
  spdlog::set_level(spdlog::level::debug);
  const std::string url = "https://www.bing.com";
  asynccurl::Request basic{url};

  asynccurl::Executor exec;
  SPDLOG_INFO("test block version");
  basic.execute_with_block();

  const int loop_total = 4;
  spdlog::stopwatch block_stopwatch;
  for (int i = 0; i < loop_total; i++) {
    SPDLOG_INFO("loop {}", i);
    basic.execute_with_block(); // reuse connection here.
  }
  SPDLOG_INFO("simple version: {}", block_stopwatch);

  auto result = fetch(url, exec);
  asynccurl::spawn(exec, result);
  //   auto future = is_right();
  //   asynccurl::spawn(exec, future);
  std::vector<std::string> urls(loop_total, url);
  auto lots = fetch_lots(urls, exec);
  auto lots2 = fetch_lots(urls, exec);
  asynccurl::spawn(exec, lots);
  asynccurl::spawn(exec, lots2);

  SPDLOG_INFO("test async version");

  spdlog::stopwatch async_stopwatch;
  std::vector<std::shared_ptr<asynccurl::Request>> requests;
  for (int i = 0; i < loop_total; i++) {
    auto res = std::make_shared<asynccurl::Request>(url);
    requests.push_back(res);
    exec.add_handle(*res);
  }
  std::atomic_bool stoped{};
  std::thread input{[&] {
    char ch{};
    std::cin >> ch;
    if (ch) {
      stoped = true;
    }
  }};
  std::thread t{[&] {
    int loop_count{};
    while (true) {
      std::barrier barrier{2};
      auto tasks = fetch_lots(urls, exec);
      tasks.set_on_finished([&barrier] {
        SPDLOG_INFO("looping task finished.");
        barrier.arrive_and_drop();
      });
      asynccurl::spawn(exec, tasks);
      barrier.arrive_and_wait();
      SPDLOG_INFO("Looping: {} finished", loop_count++);
      if (stoped) {
        exec.request_stop();
        SPDLOG_INFO("request stoped");
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds{3});
      SPDLOG_INFO("start looping {}", loop_count);
    }
  }};
  SPDLOG_INFO("start run executor.");

  exec.run_looping();
  input.join();
  t.join();

  SPDLOG_INFO("async version: {}", async_stopwatch);

  SPDLOG_INFO("finished.");
}