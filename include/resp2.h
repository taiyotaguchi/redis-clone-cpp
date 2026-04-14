#pragma once

#include <cinttypes>
#include <memory>
#include <string>
#include <variant>
#include <vector>

enum RespType {
  simple_string,
  error,
  integer,
  bulk_string,
  array,
  null,
  failed,
  incomplete
};

class RespValue {
public:
  RespType type;
  std::variant<std::string, int, std::vector<std::shared_ptr<RespValue>>> value;
};

class ParseResult {
public:
  RespValue resp_value;
  ssize_t consumed;
};

std::string encode_simple_string(std::string input);
std::string encode_error(std::string input);
std::string encode_integer(int input);
std::string encode_bulk_string(std::string input);
std::string encode_array(std::vector<std::shared_ptr<RespValue>> input);
std::string encode_null();
std::string serialize(RespValue);

ParseResult parse_simple_string(std::string_view input);
ParseResult parse_bulk_string(std::string_view input);
ParseResult parse_integer(std::string_view input);
ParseResult parse_error(std::string_view input);
ParseResult parse_array(std::string_view input);
ParseResult parse_null(std::string_view input);
ParseResult parse_failed();
ParseResult parse_incomplete();
ParseResult parse(std::string_view input);
