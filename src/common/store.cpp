#include "../../include/store.h"

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
