#include <spdlog/spdlog.h>

int main() {
  spdlog::debug("Message");
  spdlog::info("server started on port {}", 6379);
  spdlog::warn("something unexpected: {}", "MyWarning");
  spdlog::error("connection failed: {}", "MyError");
  return 0;
}
