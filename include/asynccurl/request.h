#pragma once
#include "asynccurl/executor.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <vcruntime.h>
namespace asynccurl {
class Request {
public:
  Request(std::string url) {
    handle_ = curl_easy_init();
    curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
    // curl_easy_setopt(handle_, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, Request::write_callback);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, this);
  }
  Request(const Request &) = delete;
  Request(Request &&) = delete;

  CURL *build_handle() { return handle_; }
  void execute_with_block() { curl_easy_perform(build_handle()); }

  virtual size_t body_write(char *buffer, size_t size) { return size; }

private:
  static size_t write_callback(char *ptr, size_t size, size_t nmemb,
                               Request *userdata) {
    return userdata->body_write(ptr, size * nmemb);
  }
  CURL *handle_;
};
} // namespace asynccurl