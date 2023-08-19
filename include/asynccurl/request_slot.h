#pragma once
#include "asynccurl/request.h"
#include "asynccurl/write_buffer.h"
#include <functional>
#include <memory>
#include <string>
namespace asynccurl {
class Request_slot : public Request {
public:
  Request_slot(std::string url) : Request(url) {}

  void on_finish(CURLcode result_code) override { finish_callback_(); }
  void set_on_finish_callback(std::function<void()> function) {
    finish_callback_ = function;
  }
  size_t body_write(char *buffer, size_t size) override {
    if (write_buffer_) {
      return write_buffer_->write(buffer, size);
    }
    return size;
  }
  void set_write_buffer(std::shared_ptr<Write_buffer> buffer) {
    write_buffer_ = buffer;
  }

private:
  std::function<void()> finish_callback_;
  std::shared_ptr<Write_buffer> write_buffer_;
};
} // namespace asynccurl