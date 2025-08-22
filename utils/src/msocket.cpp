#include "msocket.h"
#include <cstring>
#include "spdlog/spdlog.h"

namespace lars {

// ------------------- InetAddress 类的实现 -------------------

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr) <= 0) {
    spdlog::error("Invalid address/ Address not supported for IP: {}, Port: {}", ip, port);
  }
}

InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr) {}

const struct sockaddr *InetAddress::getSockAddr() const {
  return reinterpret_cast<const struct sockaddr *>(&addr_);
}

std::string InetAddress::getIp() const {
  char buf[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf)) == nullptr) {
    spdlog::error("Error converting IP address to string");
    return ""; // Or some other error indication
  }
  return buf;
}

uint16_t InetAddress::getPort() const { return ntohs(addr_.sin_port); }

// ------------------- Socket 类的实现 -------------------

Socket::Socket() : sockfd_(::socket(AF_INET, SOCK_STREAM, 0)) {
  if (sockfd_ < 0) {
    spdlog::error("Failed to create socket: {}", strerror(errno));
  }
}

Socket::~Socket() {
  if (sockfd_ >= 0) {
    ::close(sockfd_);
    spdlog::debug("Socket {} closed", sockfd_);
  }
}

Socket::Socket(Socket &&other) noexcept : sockfd_(other.sockfd_) {
  other.sockfd_ = -1;
  spdlog::debug("Socket {} moved", sockfd_);
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (sockfd_ >= 0) {
      ::close(sockfd_);
      spdlog::debug("Socket {} closed", sockfd_);
    }
    sockfd_ = other.sockfd_;
    other.sockfd_ = -1;
    spdlog::debug("Socket {} moved", sockfd_);
  }
  return *this;
}

void Socket::bind(const InetAddress &addr) {
  if (::bind(sockfd_, addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    spdlog::error("Failed to bind socket: {}", strerror(errno));
  }
  spdlog::debug("Socket {} binded to {}:{}", sockfd_, addr.getIp(), addr.getPort());
}

void Socket::listen(int backlog) {
  if (::listen(sockfd_, backlog) < 0) {
    spdlog::error("Failed to listen on socket: {}", strerror(errno));
  }
  spdlog::debug("Socket {} listening on backlog {}", sockfd_, backlog);
}

Socket Socket::accept(InetAddress &client_addr) {
  socklen_t addr_len = sizeof(sockaddr_in);
  struct sockaddr_in client_sockaddr;
  int client_sockfd = ::accept(
      sockfd_, reinterpret_cast<sockaddr *>(&client_sockaddr), &addr_len);
  if (client_sockfd < 0) {
    spdlog::error("Failed to accept connection: {}", strerror(errno));
    // Consider returning an invalid Socket or handling the error differently
    return Socket(-1); // 返回一个无效的Socket
  }
  client_addr = InetAddress(client_sockaddr); // 更新传入的客户端地址对象
  spdlog::debug("Accepted connection from {}:{} on socket {}", client_addr.getIp(), client_addr.getPort(), client_sockfd);
  return Socket(client_sockfd);
}

void Socket::connect(const InetAddress &server_addr) {
  if (::connect(sockfd_, server_addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    spdlog::error("Failed to connect to server: {}", strerror(errno));
  }
  spdlog::debug("Socket {} connected to {}:{}", sockfd_, server_addr.getIp(), server_addr.getPort());
}

ssize_t Socket::send(const void *buf, size_t len) {
  ssize_t bytes_sent = ::send(sockfd_, buf, len, 0);
  if (bytes_sent < 0) {
    spdlog::error("Failed to send data: {}", strerror(errno));
  } else {
    spdlog::debug("Socket {} sent {} bytes", sockfd_, bytes_sent);
  }
  return bytes_sent;
}

ssize_t Socket::recv(void *buf, size_t len) {
  ssize_t bytes_received = ::recv(sockfd_, buf, len, 0);
  if (bytes_received < 0) {
    spdlog::error("Failed to receive data: {}", strerror(errno));
  } else {
    spdlog::debug("Socket {} received {} bytes", sockfd_, bytes_received);
  }
  return bytes_received;
}

int Socket::getFd() const { return sockfd_; }

Socket::Socket(int sockfd) : sockfd_(sockfd) {
  spdlog::debug("Socket created from existing fd: {}", sockfd_);
}
} // namespace lars