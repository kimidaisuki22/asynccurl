#include "asynccurl/request.h"
#include <asynccurl/executor.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>
#include <vector>
int main() {
  const std::string url = "https://www.bing.com";
  asynccurl::Request basic{url};

  asynccurl::Executor exec;
  SPDLOG_INFO("test block version");
  basic.execute_with_block();

  const int loop_total = 20;
  spdlog::stopwatch block_stopwatch;
  for (int i = 0; i < loop_total; i++) {
    SPDLOG_INFO("loop {}", i);
    basic.execute_with_block();
  }
  SPDLOG_INFO("simple version: {}", block_stopwatch);

  SPDLOG_INFO("test async version");

  spdlog::stopwatch async_stopwatch;
  std::vector<std::shared_ptr<asynccurl::Request>> requests;
  for (int i = 0; i < loop_total; i++) {
    auto res = std::make_shared<asynccurl::Request>(url);
    requests.push_back(res);
    exec.add_handle(res->build_handle());
  }

  exec.run();
  SPDLOG_INFO("async version: {}", async_stopwatch);
  SPDLOG_INFO("finished.");
}