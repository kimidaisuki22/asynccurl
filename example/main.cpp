#include "asynccurl/request.h"
#include <asynccurl/executor.h>
#include <spdlog/spdlog.h>
int main() {
  asynccurl::Request basic{"https://www.bing.com"};

  asynccurl::Executor exec;
  SPDLOG_INFO("test block version");
  basic.execute_with_block();
  SPDLOG_INFO("test async version");
  exec.add_handle(basic.build_handle());

  exec.run();
}