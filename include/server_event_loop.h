#pragma once
#include "./server.h"
#include "./store.h"
#include <string>
#include <unordered_map>

int create_epoll(int server_fd);

int handle_accept(int epoll_fd, Server &server);

void remove_client(int epoll_fd, int client_fd,
                   std::unordered_map<int, std::string> &command_buffer_map);

void handle_client(int epoll_fd, int client_fd, Server &server, Store &store,
                   std::unordered_map<int, std::string> &command_buffer_map,
                   std::unordered_map<int, std::string> &write_buffer_map);
void handle_write(int epoll_fd, Server &server, int client_fd,
                  std::unordered_map<int, std::string> &command_buffer_map,
                  std::unordered_map<int, std::string> &write_buffer_map);
