#include "logger.h"

int main() {
  // 正确的宏调用方式，不带命名空间
  lars::LARS_TRACE("Hello world");

  return 0;
}