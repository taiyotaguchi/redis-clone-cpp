#include "../../include/resp2.h"
#include "../../include/logger.h"
#include <fmt/base.h>
#include <memory>
#include <string>
#include <variant>
#include <vector>

std::string encode_simple_string(std::string input) {
  return "+" + input + "\r\n";
}

std::string encode_error(std::string input) { return "-" + input + "\r\n"; }

std::string encode_integer(std::string input) { return ":" + input + "\r\n"; }

std::string encode_bulk_string(std::string input) {
  return "$" + std::to_string(input.length()) + "\r\n" + input + "\r\n";
}

std::string encode_array(std::vector<std::string> input) {
  std::string output = "*" + std::to_string(input.size()) + "\r\n";
  for (int i = 0; i < input.size(); i++) {
    output += encode_bulk_string(input[i]);
  }
  return output;
}

std::string encode_null() { return "$-1\r\n"; }

RespValue parse_simple_string(std::string input) {
  std::string output = "";
  for (int i = 1; i < input.length() - 2; i++) {
    output += input[i];
  }
  RespValue value;
  value.type = simple_string;
  value.value = output;
  return value;
}

RespValue parse_bulk_string(std::string input) {
  std::string len = "";
  int startIndex;
  for (int i = 1; i < input.length() - 2; i++) {
    len += input[i];
    if (input[i + 1] == '\r') {
      startIndex = i + 3;
      break;
    }
  }
  int lenInInt = std::stoi(len);
  std::string output = "";
  for (int i = startIndex; i < startIndex + lenInInt; i++) {
    output += input[i];
  }
  RespValue value;
  value.type = bulk_string;
  value.value = output;
  return value;
}

RespValue parse_integer(std::string input) {
  std::string output = "";
  for (int i = 1; i < input.length() - 2; i++) {
    output += input[i];
  }
  RespValue value;
  value.type = integer;
  value.value = output;
  return value;
}

RespValue parse_error(std::string input) {
  std::string output = "";
  for (int i = 1; i < input.length() - 2; i++) {
    output += input[i];
  }
  RespValue value;
  value.type = error;
  value.value = output;
  return value;
}

RespValue parse_array(std::string input) {
  std::vector<std::string> string_vec;

  // 1. Get array length
  std::string array_length_string = "";
  int current_index = 0;
  for (int i = 1; i < input.length() - 2; i++) {
    array_length_string += input[i];
    if (input[i + 1] == '\r') {
      current_index = i + 4;
      break;
    }
  }
  int array_length = std::stoi(array_length_string);

  // 2. For loop of array length
  for (int i = 0; i < array_length; i++) {
    // 3. Get length of each section
    std::string string_length_string = "";
    for (int j = current_index; j < input.length() - 2; j++) {
      string_length_string += input[j];
      if (input[j + 1] == '\r') {
        current_index += 3;
        break;
      }
    }
    int string_length = std::stoi(string_length_string);

    // 4. Capture string
    std::string output_string = "";
    for (int j = current_index; j < current_index + string_length; j++) {
      output_string += input[j];
    }
    string_vec.push_back(output_string);
    current_index = current_index + string_length + 3;
  }

  std::vector<std::shared_ptr<RespValue>> output_vec;
  for (int i = 0; i < string_vec.size(); i++) {
    RespValue new_bulk_string;
    new_bulk_string.type = bulk_string;
    new_bulk_string.value = string_vec[i];
    output_vec.push_back(std::make_shared<RespValue>(new_bulk_string));
  }

  RespValue output;
  output.type = array;
  output.value = output_vec;
  return output;
}

RespValue parse_null() {
  RespValue output;
  output.type = null;
  output.value = "";
  return output;
}

RespValue parse_failed() {
  RespValue output;
  output.type = failed;
  output.value = "";
  return output;
}

RespValue parse(std::string input) {
  switch (input[0]) {
  case '$':
    if (input[1] == '-') {
      return parse_null();
    } else {
      return parse_bulk_string(input);
    }
    break;
  case '+':
    return parse_simple_string(input);
    break;
  case '-':
    return parse_error(input);
    break;
  case ':':
    return parse_integer(input);
    break;
  case '*':
    return parse_array(input);
    break;
  default:
    LOG_ERROR("Couldn't pass");
    return parse_failed();
    break;
  }
}
