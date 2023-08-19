#pragma once
#include <functional>
#include <mutex>
#include <thread>
namespace asynccurl {
class Thread {
public:
  Thread(
      std::function<void()> loop_call, std::function<void()> wake_up = [] {})
      : wake_up_(wake_up) {
    t_ = std::thread{[this, loop_call] {
      while (true) {
        {
          std::unique_lock lock{mutex_};
          if (stop_) {
            break;
          }
          loop_call();
        }
      }
    }};
  }
  ~Thread() {
    {
      std::unique_lock lock{mutex_};
      stop_ = true;
    }
    wake_up_();
    t_.join();
  }

private:
  std::thread t_;
  std::function<void()> wake_up_;
  std::mutex mutex_;
  bool stop_{false};
};
} // namespace asynccurl