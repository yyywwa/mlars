#include "logger.h"
#include "spdlog/common.h"
int main() {
  lars::LOG_SET_LEVEL_INFO();
  lars::LOG_INFO("qweqgqwfqwgqwf {} {} {}", 1, 2, 3);
  while (1) {
  }
  return 0;
}
