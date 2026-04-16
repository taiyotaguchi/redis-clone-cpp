#include "../../include/server.h"
#include "../../include/logger.h"
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int Server::create() {
  struct addrinfo hints;
  struct addrinfo *res;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  int getaddrinfo_res = getaddrinfo(ip.empty() ? nullptr : ip.c_str(),
                                    port.c_str(), &hints, &res);

  if (getaddrinfo_res == -1) {
    LOG_ERROR("Failed to run getaddrinfo");
    return -1;
  }

  fd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (fd == -1) {
    LOG_ERROR("Failed to create socket");
    return -1;
  }

  int bind_res = bind(fd, res->ai_addr, res->ai_addrlen);

  if (bind_res == -1) {
    LOG_ERROR("Failed to bind to socket");
    return -1;
  }

  freeaddrinfo(res);

  int listen_res = listen(fd, backlog);

  if (listen_res == -1) {
    LOG_ERROR("Failed to run listen");
    return -1;
  }

  LOG_INFO("Server listening");

  return fd;
}

int Server::accept() {
  int client_fd = ::accept(fd, NULL, NULL);

  if (client_fd == -1) {
    LOG_ERROR("Failed to accept client");
    return -1;
  }

  return client_fd;
}
