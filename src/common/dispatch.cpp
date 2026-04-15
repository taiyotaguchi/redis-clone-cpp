#include "../../include/dispatch.h"
#include "../../include/logger.h"
#include "../../include/resp2.h"
#include <algorithm>
#include <charconv>

RespValue handle_ping() { return RespValue{simple_string, "PONG"}; }
RespValue handle_echo(std::string arg) { return RespValue{bulk_string, arg}; }
RespValue handle_set(std::vector<std::string> args, Store &store) {
  store.set(args[0], args[1]);
  return RespValue{simple_string, "OK"};
}
RespValue handle_get(std::vector<std::string> args, Store &store) {
  std::optional<std::string> output = store.get(args[0]);
  if (output.has_value()) {
    return RespValue{bulk_string, output.value()};
  } else {
    return RespValue{null, ""};
  }
}
RespValue handle_del(std::vector<std::string> args, Store &store) {
  int count = 0;
  for (int i = 0; i < args.size(); i++) {
    if (store.del(args[i])) {
      count++;
    }
  }
  return RespValue{integer, count};
}
RespValue handle_exists(std::vector<std::string> args, Store &store) {
  int count = 0;
  for (int i = 0; i < args.size(); i++) {
    if (store.exists(args[i])) {
      count++;
    }
  }
  return RespValue{integer, count};
}
RespValue handle_incr(std::vector<std::string> args, Store &store) {
  std::optional<int> output = store.incr(args[0]);
  if (output.has_value()) {
    return RespValue{integer, output.value()};
  } else {
    return RespValue{error, "Value is not an integer or out of range"};
  }
}
RespValue handle_decr(std::vector<std::string> args, Store &store) {
  std::optional<int> output = store.decr(args[0]);
  if (output.has_value()) {
    return RespValue{integer, output.value()};
  } else {
    return RespValue{error, "Value is not an integer or out of range"};
  }
}
RespValue make_error(std::string msg) {
  RespValue resp_value;
  resp_value.type = error;
  resp_value.value = msg;
  return resp_value;
}

RespValue execute(std::vector<std::string> cmd, Store &store) {
  if (cmd.size() < 1) {
    return make_error("Command is empty");
  }

  std::string cmd_name = cmd[0];
  std::transform(cmd_name.begin(), cmd_name.end(), cmd_name.begin(), ::toupper);

  if (cmd_name == "PING") {
    if (cmd.size() != 1) {
      return make_error("Argument count is wrong");
    } else {
      return handle_ping();
    }
  } else if (cmd_name == "ECHO") {
    if (cmd.size() != 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_echo(cmd[1]);
    }
  } else if (cmd_name == "SET") {
    if (cmd.size() != 3) {
      return make_error("Argument count is wrong");
    } else {
      return handle_set(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                        store);
    }
  } else if (cmd_name == "GET") {
    if (cmd.size() != 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_get(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                        store);
    }
  } else if (cmd_name == "DEL") {
    if (cmd.size() < 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_del(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                        store);
    }
  } else if (cmd_name == "EXISTS") {
    if (cmd.size() < 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_exists(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                           store);
    }
  } else if (cmd_name == "INCR") {
    if (cmd.size() != 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_incr(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                         store);
    }
  } else if (cmd_name == "DECR") {
    if (cmd.size() != 2) {
      return make_error("Argument count is wrong");
    } else {
      return handle_decr(std::vector<std::string>(cmd.begin() + 1, cmd.end()),
                         store);
    }
  } else {
    return make_error("Command does not exist");
  }
}
