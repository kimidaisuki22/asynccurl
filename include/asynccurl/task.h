#pragma once
// This code is from
// https://en.cppreference.com/mwiki/index.php?title=cpp/coroutine/coroutine_handle&oldid=156820

// with some modification.
#include <coroutine>
#include <iostream>
#include <memory>
#include <optional>
struct Resume_parent {
  constexpr bool await_ready() const noexcept { return false; }

  constexpr std::coroutine_handle<>
  await_suspend(std::coroutine_handle<>) const noexcept {
    if (parent_) {
      return parent_;
    }
    return std::noop_coroutine();
  }
  static constexpr void await_resume() noexcept {}
  std::coroutine_handle<> parent_{};
};

template <typename T> class Task {
public:
  struct promise_type {
    Task<T> get_return_object() { return Task{Handle::from_promise(*this)}; }
    constexpr std::suspend_always initial_suspend() noexcept { return {}; }
    Resume_parent final_suspend() noexcept { return {parent_}; }

    void return_value(T value) { *result_value_ = std::move(value); }

    [[noreturn]] static void unhandled_exception() { throw; }
    std::coroutine_handle<> parent_{};
    std::shared_ptr<T> result_value_ = std::make_shared<T>();
  };

  using Handle = std::coroutine_handle<promise_type>;

  explicit Task(const Handle coroutine) : coroutine_{coroutine} {
    result_ = coroutine_.promise().result_value_;
  }

  Task() = default;
  ~Task() {
    if (coroutine_)
      coroutine_.destroy();
  }

  Task(const Task &) = delete;
  Task &operator=(const Task &) = delete;

  Task(Task &&other) noexcept : coroutine_{other.coroutine_} {
    other.coroutine_ = {};
    result_ = other.result_;
  }
  Task &operator=(Task &&other) noexcept {
    if (this != &other) {
      if (coroutine_)
        coroutine_.destroy();
      coroutine_ = other.coroutine_;
      other.coroutine_ = {};
      result_ = other.result_;
    }
    return *this;
  }
  void resume() { coroutine_.resume(); }
  void set_parent(std::coroutine_handle<> hd) {
    coroutine_.promise().parent_ = hd;
  }
  T get_result() { return *result_; }

private:
  Handle coroutine_{};
  std::shared_ptr<T> result_;
};

// template <> class Task<void> {
// public:
//   struct promise_type {
//     Task get_return_object() { return Task{Handle::from_promise(*this)}; }
//     constexpr std::suspend_always initial_suspend() noexcept { return {}; }
//     constexpr std::suspend_always final_suspend() noexcept { return {}; }

//     void return_void() {}

//     [[noreturn]] static void unhandled_exception() { throw; }
//   };
//   using Handle = std::coroutine_handle<promise_type>;
//   explicit Task(const Handle coroutine) : coroutine_{coroutine} {}

//   Task() = default;
//   ~Task() {
//     if (coroutine_)
//       coroutine_.destroy();
//   }

//   Task(const Task &) = delete;
//   Task &operator=(const Task &) = delete;

//   Task(Task &&other) noexcept : coroutine_{other.coroutine_} {
//     other.coroutine_ = {};
//   }
//   Task &operator=(Task &&other) noexcept {
//     if (this != &other) {
//       if (coroutine_)
//         coroutine_.destroy();
//       coroutine_ = other.coroutine_;
//       other.coroutine_ = {};
//     }
//     return *this;
//   }
//   void resume() { coroutine_.resume(); }

// private:
//   Handle coroutine_;
// };