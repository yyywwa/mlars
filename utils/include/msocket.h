#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
namespace lars {
// 地址类：封装 IP 地址和端口
class InetAddress {
public:
  InetAddress() : is_valid_(false) {}
  InetAddress(const std::string &ip, uint16_t port);
  explicit InetAddress(const struct sockaddr_in &addr);
  void set(const struct sockaddr_in &addr);

  const struct sockaddr *getSockAddr() const;
  std::string getIp() const;
  uint16_t getPort() const;

  bool isValid() const { return is_valid_; }

private:
  struct sockaddr_in addr_;
  bool is_valid_;
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
  void bind(const std::string &ip, uint16_t port);
  void listen(int backlog = 1024);
  Socket accept();
  void connect(const InetAddress &server_addr);

  ssize_t send(const void *buf, size_t len);
  ssize_t recv(void *buf, size_t len);

  int getFd() const;
  InetAddress &getSocketAddr();
  // 新增的状态检查函数
  bool isBound() const;
  bool isListening() const;
  bool isAddrValid() const;

private:
  // 私有构造函数，用于 accept 返回时创建新的 Socket 对象
  explicit Socket(int sockfd);

  int sockfd_;
  InetAddress addr_;
  bool is_bound_;
  bool is_listening_;
};
} // namespace lars