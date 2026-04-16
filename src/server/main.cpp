#include "../../include/dispatch.h"
#include "../../include/resp2.h"
#include "../../include/server.h"
#include "../../include/store.h"
#include <iostream>
#include <unistd.h>

int main() {
  Store store;
  Server server("127.0.0.1", "6969", 100);
  int server_fd = server.create();
  while (true) {
    int client_fd = server.accept();
    while (true) {
      std::string command_encoded = server.read(client_fd, 2048);
      if (command_encoded.empty()) {
        break;
      }
      auto commands_parsed = std::get<std::vector<std::shared_ptr<RespValue>>>(
          parse(command_encoded).resp_value.value);
      std::vector<std::string> commands;
      for (int i = 0; i < commands_parsed.size(); i++) {
        commands.push_back(std::get<std::string>(commands_parsed[i]->value));
      }
      RespValue resp_value = execute(commands, store);
      std::string return_encoded = serialize(resp_value);
      int send_res = server.send(client_fd, return_encoded);
    }
    close(client_fd);
  }
  return 1;
}
