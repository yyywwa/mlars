#include "logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

namespace lars {

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

Logger::Logger() {
  // 设置日志格式
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%!] %v");

  // 创建控制台 sink
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);

  // 创建文件 sink
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/lars.log", true);
  file_sink->set_level(spdlog::level::trace);

  // 将 sinks 放入 vector
  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  // 创建多 sink 的 logger
  m_logger = std::make_shared<spdlog::logger>("lars_logger", sinks.begin(),
                                              sinks.end());
  m_logger->set_level(spdlog::level::trace);
  m_logger->flush_on(spdlog::level::warn);
}

Logger::~Logger() {
  m_logger->flush();
  spdlog::drop_all(); // 释放所有注册的 logger
}

void Logger::flush() { m_logger->flush(); }

// 模板函数的实现必须放在头文件或包含在源文件中
template <typename... Args>
void Logger::log(spdlog::level::level_enum level, const char *file, int line,
                 const char *func, const char *fmt, const Args &...args) {
  m_logger->log(level, fmt::format(fmt, args...));
  // 也可以直接使用 spdlog 的内部格式化
  // m_logger->log(level, fmt, args...);
}

// 公共日志接口的实现
void Logger::trace(const char *file, int line, const char *func,
                   const std::string &msg) {
  m_logger->trace("[{}:{}] {} - {}", file, line, func, msg);
}

void Logger::debug(const char *file, int line, const char *func,
                   const std::string &msg) {
  m_logger->debug("[{}:{}] {} - {}", file, line, func, msg);
}

void Logger::info(const char *file, int line, const char *func,
                  const std::string &msg) {
  m_logger->info("[{}:{}] {} - {}", file, line, func, msg);
}

void Logger::warn(const char *file, int line, const char *func,
                  const std::string &msg) {
  m_logger->warn("[{}:{}] {} - {}", file, line, func, msg);
}

void Logger::error(const char *file, int line, const char *func,
                   const std::string &msg) {
  m_logger->error("[{}:{}] {} - {}", file, line, func, msg);
}

void Logger::critical(const char *file, int line, const char *func,
                      const std::string &msg) {
  m_logger->critical("[{}:{}] {} - {}", file, line, func, msg);
}

} // namespace lars