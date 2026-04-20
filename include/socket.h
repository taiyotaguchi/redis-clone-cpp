#pragma once
#include <optional>
#include <string>

class Socket {
public:
  Socket(std::string ip, std::string port) : ip(ip), port(port) {}

  ~Socket();

  static int send(int fd, std::string buffer);

  static std::optional<std::string> read(int fd, size_t size);

protected:
  std::string ip;
  std::string port;
  int fd = -1;
};
