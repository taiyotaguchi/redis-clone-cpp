#include "../../include/dispatch.h"
#include "../../include/resp2.h"
#include "../../include/server.h"
#include "../../include/server_event_loop.h"
#include "../../include/store.h"
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

int main() {
  Store store;

  Server server("127.0.0.1", "6969", 100);

  epoll_event ready_fds[64];

  int server_fd = server.create();

  int epoll_fd = create_epoll(server_fd);

  std::unordered_map<int, std::string> command_buffer_map;

  std::unordered_map<int, std::string> write_buffer_map;

  while (true) {
    int n = epoll_wait(epoll_fd, ready_fds, 64, -1);
    for (int i = 0; i < n; i++) {
      if (ready_fds[i].data.fd == server_fd) {
        handle_accept(epoll_fd, server);
      } else {
        if (ready_fds[i].events & EPOLLOUT) {
          handle_write(epoll_fd, server, ready_fds[i].data.fd,
                       command_buffer_map, write_buffer_map);
        }
        if (ready_fds[i].events & EPOLLIN) {
          handle_client(epoll_fd, ready_fds[i].data.fd, server, store,
                        command_buffer_map, write_buffer_map);
        }
      }
    }
  }
}
