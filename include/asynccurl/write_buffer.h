#pragma once

#include <cstdint>
#include <fstream>
#include <string>
namespace asynccurl {
class Write_buffer {
public:
  virtual size_t write(char *buffer, size_t size) = 0;
  virtual ~Write_buffer() = default;
};

class Write_string : public Write_buffer {
public:
  size_t write(char *buffer, size_t size) override {
    buffer_.append(buffer, buffer + size);
    return size;
  }
  std::string buffer_;
};
class Write_file : public Write_buffer {
public:
  size_t write(char *buffer, size_t size) override {
    file_.write(buffer, size);
    return file_.good() ? size : 0;
  }
  std::ofstream file_;
};
} // namespace asynccurl