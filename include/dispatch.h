#pragma once
#include "./resp2.h"
#include "./store.h"
#include <string>
#include <vector>

RespValue execute(std::vector<std::string> cmd, Store &store);
RespValue handle_ping();
RespValue handle_echo(std::string arg);
RespValue handle_set(std::vector<std::string> args, Store &store);
RespValue handle_get(std::vector<std::string> args, Store &store);
RespValue handle_del(std::vector<std::string> args, Store &store);
RespValue handle_exists(std::vector<std::string> args, Store &store);
RespValue handle_incr(std::vector<std::string> args, Store &store);
RespValue handle_decr(std::vector<std::string> args, Store &store);
RespValue handle_incr(std::vector<std::string> args, Store &store);
