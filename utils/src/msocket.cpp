#include "msocket.h"
#include <cstring>
#include <stdexcept>

namespace lars {

// ------------------- InetAddress 类的实现 -------------------

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
}

InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr) {}

const struct sockaddr *InetAddress::getSockAddr() const {
  return reinterpret_cast<const struct sockaddr *>(&addr_);
}

std::string InetAddress::getIp() const {
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  return buf;
}

uint16_t InetAddress::getPort() const { return ntohs(addr_.sin_port); }

// ------------------- Socket 类的实现 -------------------

Socket::Socket() : sockfd_(::socket(AF_INET, SOCK_STREAM, 0)) {
  if (sockfd_ < 0) {
    throw std::runtime_error("Failed to create socket");
  }
}

Socket::~Socket() {
  if (sockfd_ >= 0) {
    ::close(sockfd_);
  }
}

Socket::Socket(Socket &&other) noexcept : sockfd_(other.sockfd_) {
  other.sockfd_ = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (sockfd_ >= 0) {
      ::close(sockfd_);
    }
    sockfd_ = other.sockfd_;
    other.sockfd_ = -1;
  }
  return *this;
}

void Socket::bind(const InetAddress &addr) {
  if (::bind(sockfd_, addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    throw std::runtime_error("Failed to bind socket");
  }
}

void Socket::listen(int backlog) {
  if (::listen(sockfd_, backlog) < 0) {
    throw std::runtime_error("Failed to listen on socket");
  }
}

Socket Socket::accept(InetAddress &client_addr) {
  socklen_t addr_len = sizeof(sockaddr_in);
  struct sockaddr_in client_sockaddr;
  int client_sockfd = ::accept(
      sockfd_, reinterpret_cast<sockaddr *>(&client_sockaddr), &addr_len);
  if (client_sockfd < 0) {
    throw std::runtime_error("Failed to accept connection");
  }
  client_addr = InetAddress(client_sockaddr); // 更新传入的客户端地址对象
  return Socket(client_sockfd);
}

void Socket::connect(const InetAddress &server_addr) {
  if (::connect(sockfd_, server_addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    throw std::runtime_error("Failed to connect to server");
  }
}

ssize_t Socket::send(const void *buf, size_t len) {
  return ::send(sockfd_, buf, len, 0);
}

ssize_t Socket::recv(void *buf, size_t len) {
  return ::recv(sockfd_, buf, len, 0);
}

int Socket::getFd() const { return sockfd_; }

Socket::Socket(int sockfd) : sockfd_(sockfd) {}
} // namespace lars