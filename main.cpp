#include "logger.h"
#include "msocket.h"
#include "spdlog/common.h"
#include "spdlog/fmt/bundled/base.h"
int main() {
  lars::LOG_INFO("begin");
  lars::Socket server;
  server.bind({"127.0.0.1", 7777});
  server.listen();

  lars::Socket client;
  client.bind("127.0.0.1", 7772);
  client.connect(server.getSocketAddr());
  fmt::print("socketfd size is :{}, addr size is :{}", sizeof(client.getFd()),
             sizeof(client.getSocketAddr()));
  return 0;
}
