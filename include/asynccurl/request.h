#pragma once
#include "asynccurl/request.h"
#include <cassert>
#include <curl/curl.h>
#include <curl/easy.h>
#include <functional>
#include <stdexcept>
#include <string>

namespace asynccurl {
class Request {
public:
  Request(std::string url) : Request(curl_easy_init()) {
    curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
  }
  Request(CURL *handle) : handle_(handle) {
    assert(handle);
    if (!handle) {
      throw std::invalid_argument{"curl handle is nullptr"};
    }
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, Request::write_callback);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(handle_, CURLOPT_PRIVATE, this);
  }
  Request(const Request &) = delete;
  Request(Request &&) = delete;

  ~Request() { curl_easy_cleanup(handle_); }

  CURL *build_handle() { return handle_; }
  void execute_with_block() {
    auto code = curl_easy_perform(build_handle());
    on_finish(code);
  }

  virtual size_t body_write(char *buffer, size_t size) { return size; }
  virtual void on_finish(CURLcode result_code) {}

private:
  static size_t write_callback(char *ptr, size_t size, size_t nmemb,
                               Request *userdata) {
    return userdata->body_write(ptr, size * nmemb);
  }
  CURL *handle_;
};

} // namespace asynccurl