#include "msocket.h"
#include "logger.h" // 使用自定义的 logger.h
#include <cstring>

namespace lars {

// ------------------- InetAddress 类的实现 -------------------

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);
  if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr) <= 0) {
    LOG_ERROR("Invalid address/ Address not supported for IP: {}, Port: {}", ip,
              port);
    is_valid_ = false;
  } else {
    is_valid_ = true;
  }
}

void InetAddress::set(const struct sockaddr_in &addr) {
  addr_ = addr;
  is_valid_ = true; // 从 accept() 接受的地址是有效的
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
    : addr_(addr), is_valid_(true) {}

const struct sockaddr *InetAddress::getSockAddr() const {
  return reinterpret_cast<const struct sockaddr *>(&addr_);
}

std::string InetAddress::getIp() const {
  char buf[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf)) == nullptr) {
    LOG_ERROR("Error converting IP address to string");
    return ""; // Or some other error indication
  }
  return buf;
}

uint16_t InetAddress::getPort() const { return ntohs(addr_.sin_port); }

// ------------------- Socket 类的实现 -------------------

Socket::Socket()
    : sockfd_(::socket(AF_INET, SOCK_STREAM, 0)), is_bound_(false),
      is_listening_(false) {
  if (sockfd_ < 0) {
    LOG_ERROR("Failed to create socket: {}", strerror(errno));
  }
}

Socket::~Socket() {
  if (sockfd_ >= 0) {
    ::close(sockfd_);
  }
}

Socket::Socket(Socket &&other) noexcept
    : sockfd_(other.sockfd_), is_bound_(other.is_bound_),
      is_listening_(other.is_listening_) {
  other.sockfd_ = -1;
  other.is_bound_ = false;
  other.is_listening_ = false;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
    if (sockfd_ >= 0) {
      ::close(sockfd_);
    }
    sockfd_ = other.sockfd_;
    is_bound_ = other.is_bound_;
    is_listening_ = other.is_listening_;
    other.sockfd_ = -1;
    other.is_bound_ = false;
    other.is_listening_ = false;
  }
  return *this;
}

void Socket::bind(const InetAddress &addr) {
  if (::bind(sockfd_, addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    LOG_ERROR("Failed to bind socket: {}, addr is {}:{}", strerror(errno),
              addr.getIp(), addr.getPort());
  } else {
    is_bound_ = true;
  }
  addr_ = addr;
  LOG_DEBUG("Socket {} binded to {}:{}", sockfd_, addr.getIp(), addr.getPort());
}

void Socket::bind(const std::string &ip, uint16_t port) { bind({ip, port}); }

void Socket::listen(int backlog) {
  if (::listen(sockfd_, backlog) < 0) {
    LOG_ERROR("Failed to listen on socket: {}, addr is {}:{}", strerror(errno),
              addr_.getIp(), addr_.getPort());
  } else {
    is_listening_ = true;
  }
}

InetAddress &Socket::getSocketAddr() { return addr_; }

Socket Socket::accept() {
  socklen_t addr_len = sizeof(sockaddr_in);
  struct sockaddr_in client_sockaddr;
  int client_sockfd = ::accept(
      sockfd_, reinterpret_cast<sockaddr *>(&client_sockaddr), &addr_len);
  if (client_sockfd < 0) {
    LOG_ERROR("Failed to accept connection: {}", strerror(errno));
    return Socket(-1); // 返回一个无效的Socket
  }
  Socket client{};
  client.sockfd_ = client_sockfd;
  client.addr_.set(client_sockaddr);
  LOG_DEBUG("Accepted connection from {}:{} on socket {}", client.addr_.getIp(),
            client.addr_.getPort(), client_sockfd);
  return client;
}

void Socket::connect(const InetAddress &server_addr) {
  if (::connect(sockfd_, server_addr.getSockAddr(), sizeof(sockaddr_in)) < 0) {
    LOG_ERROR("Failed to connect to server: {}", strerror(errno));
  } else {
    is_bound_ = true; // 连接成功后，套接字也相当于绑定到了一个随机端口
  }
  struct sockaddr_in local_addr;
  socklen_t addr_len = sizeof(local_addr);
  memset(&local_addr, 0, addr_len);

  if (::getsockname(sockfd_, reinterpret_cast<sockaddr *>(&local_addr),
                    &addr_len) < 0) {
    LOG_ERROR("Failed to get local address for socket {}: {}", sockfd_,
              strerror(errno));
  }
  addr_.set(local_addr);
  LOG_DEBUG("Socket {} connected to {}:{}", sockfd_, server_addr.getIp(),
            server_addr.getPort());
}

ssize_t Socket::send(const void *buf, size_t len) {
  ssize_t bytes_sent = ::send(sockfd_, buf, len, 0);
  if (bytes_sent < 0) {
    LOG_ERROR("Failed to send data: {}", strerror(errno));
  }
  return bytes_sent;
}

ssize_t Socket::recv(void *buf, size_t len) {
  ssize_t bytes_received = ::recv(sockfd_, buf, len, 0);
  if (bytes_received < 0) {
    LOG_ERROR("Failed to receive data: {}", strerror(errno));
  }
  return bytes_received;
}

int Socket::getFd() const { return sockfd_; }

Socket::Socket(int sockfd)
    : sockfd_(sockfd), is_bound_(true), is_listening_(false) {}

// 新增函数的实现
bool Socket::isBound() const { return is_bound_; }

bool Socket::isListening() const { return is_listening_; }

bool Socket::isAddrValid() const { return addr_.isValid(); }

} // namespace lars