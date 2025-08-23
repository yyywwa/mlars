#pragma once
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
namespace lars {
// 地址类：封装 IP 地址和端口
class InetAddress {
public:
  InetAddress() {}
  InetAddress(const std::string &ip, uint16_t port);
  explicit InetAddress(const struct sockaddr_in &addr);

  const struct sockaddr *getSockAddr() const;
  std::string getIp() const;
  uint16_t getPort() const;

private:
  struct sockaddr_in addr_;
};

// 套接字类：封装文件描述符和系统调用
class Socket {
public:
  Socket();
  ~Socket();

  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;

  Socket(Socket &&other) noexcept;
  Socket &operator=(Socket &&other) noexcept;

  void bind(const InetAddress &addr);
  void listen(int backlog = 1024);
  Socket accept(InetAddress &client_addr);
  void connect(const InetAddress &server_addr);

  ssize_t send(const void *buf, size_t len);
  ssize_t recv(void *buf, size_t len);

  int getFd() const;

private:
  // 私有构造函数，用于 accept 返回时创建新的 Socket 对象
  explicit Socket(int sockfd);

  int sockfd_;
};
} // namespace lars