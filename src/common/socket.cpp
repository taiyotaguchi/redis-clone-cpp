#include "../../include/socket.h"
#include "../../include/logger.h"
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Socket::~Socket() {
  if (fd != -1)
    close(fd);
}

int Socket::send(int fd, std::string buffer) {
  ssize_t send_res = ::send(fd, buffer.c_str(), buffer.length(), 0);
  if (send_res == -1) {
    LOG_ERROR("Failed to send buffer");
    return -1;
  }
  return send_res;
}

std::optional<std::string> Socket::read(int fd, size_t size) {
  std::string buffer(size, '\0');
  ssize_t bytes_read = ::read(fd, buffer.data(), size);
  if (bytes_read == -1) {
    if (errno == EAGAIN) {
      return std::nullopt;
    } else {
      LOG_ERROR("Failed to read buffer");
      LOG_ERROR("Failed to read buffer: " + std::string(strerror(errno)));
      return "";
    }
  }
  buffer.resize(bytes_read);
  return buffer;
}
