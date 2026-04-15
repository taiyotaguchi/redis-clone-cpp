#include "../../include/store.h"
#include <charconv>
#include <optional>

void Store::set(const std::string &key, const std::string &value) {
  data[key] = value;
}
std::optional<std::string> Store::get(const std::string &key) {
  auto i = data.find(key);
  if (i != data.end()) {
    return i->second;
  } else {
    return std::nullopt;
  }
}
bool Store::del(const std::string &key) { return data.erase(key); }
bool Store::exists(const std::string &key) { return data.count(key); }
std::optional<int> Store::incr(const std::string &key) {
  int num;
  std::optional<std::string> output = get(key);
  if (output.has_value()) {
    auto from_chars_res =
        std::from_chars(output.value().data(),
                        output.value().data() + output.value().size(), num);
    if (from_chars_res.ec == std::errc()) {
      set(key, std::to_string(num + 1));
      return num + 1;
    } else {
      return std::nullopt;
    }
  } else {
    set(key, "1");
    return 1;
  }
}
std::optional<int> Store::decr(const std::string &key) {
  int num;
  std::optional<std::string> output = get(key);
  if (output.has_value()) {
    auto from_chars_res =
        std::from_chars(output.value().data(),
                        output.value().data() + output.value().size(), num);
    if (from_chars_res.ec == std::errc()) {
      set(key, std::to_string(num - 1));
      return num - 1;
    } else {
      return std::nullopt;
    }
  } else {
    set(key, "-1");
    return -1;
  }
}
