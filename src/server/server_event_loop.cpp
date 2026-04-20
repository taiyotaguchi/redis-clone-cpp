#include "../../include/server_event_loop.h"
#include "../../include/dispatch.h"
#include "../../include/resp2.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

int create_epoll(int server_fd) {
  int epoll_fd = epoll_create1(0);
  epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = server_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
  return epoll_fd;
}

int handle_accept(int epoll_fd, Server &server) {
  int new_client_fd = server.accept();
  epoll_event new_ev;
  new_ev.events = EPOLLIN;
  new_ev.data.fd = new_client_fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client_fd, &new_ev);
  return new_client_fd;
}

void remove_client(int epoll_fd, int client_fd,
                   std::unordered_map<int, std::string> &command_buffer_map) {
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
  command_buffer_map.erase(client_fd);
  close(client_fd);
}

void handle_client(int epoll_fd, int client_fd, Server &server, Store &store,
                   std::unordered_map<int, std::string> &command_buffer_map,
                   std::unordered_map<int, std::string> &write_buffer_map) {
  std::string commands_encoded = server.read(client_fd, 2048);
  if (commands_encoded.empty()) {
    remove_client(epoll_fd, client_fd, command_buffer_map);
    return;
  }
  command_buffer_map[client_fd] += commands_encoded;
  ParseResult parsed_result = parse(command_buffer_map[client_fd]);
  if (parsed_result.resp_value.type == incomplete) {
    return;
  } else if (parsed_result.resp_value.type == failed) {
    remove_client(epoll_fd, client_fd, command_buffer_map);
    return;
  }
  auto commands_parsed = std::get<std::vector<std::shared_ptr<RespValue>>>(
      parsed_result.resp_value.value);
  std::vector<std::string> commands;
  for (int i = 0; i < commands_parsed.size(); i++) {
    commands.push_back(std::get<std::string>(commands_parsed[i]->value));
  }
  RespValue resp_value = execute(commands, store);
  std::string return_encoded = serialize(resp_value);
  int send_res = server.send(client_fd, return_encoded);
  if (send_res < return_encoded.size() && send_res != -1) {
    write_buffer_map[client_fd] = return_encoded.substr(send_res);
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
  } else if (send_res == -1) {
    if (errno == EAGAIN) {
      write_buffer_map[client_fd] = return_encoded;
      epoll_event ev;
      ev.events = EPOLLIN | EPOLLOUT;
      ev.data.fd = client_fd;
      epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
    } else {
      remove_client(epoll_fd, client_fd, command_buffer_map);
    }
  }
  command_buffer_map.erase(client_fd);
}

void handle_write(int epoll_fd, Server &server, int client_fd,
                  std::unordered_map<int, std::string> &command_buffer_map,
                  std::unordered_map<int, std::string> &write_buffer_map) {
  int send_res = server.send(client_fd, write_buffer_map[client_fd]);
  if (send_res < write_buffer_map[client_fd].size() && send_res != -1) {
    write_buffer_map[client_fd] = write_buffer_map[client_fd].substr(send_res);
  } else if (send_res == -1) {
    if (errno != EAGAIN) {
      remove_client(epoll_fd, client_fd, command_buffer_map);
    }
  } else {
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &ev);
    write_buffer_map.erase(client_fd);
  }
}
