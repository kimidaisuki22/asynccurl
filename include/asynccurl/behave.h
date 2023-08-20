#pragma once

#include "asynccurl/request.h"
#include <curl/curl.h>
#include <curl/easy.h>
namespace asynccurl {
inline void follow_redirect(Request &request, bool on) {
  curl_easy_setopt(request.build_handle(), CURLOPT_FOLLOWLOCATION,
                   on ? 1L : 0L);
}
inline void set_proxy(Request &request, const char *address) {
  curl_easy_setopt(request.build_handle(), CURLOPT_PROXY, address);
}
} // namespace asynccurl