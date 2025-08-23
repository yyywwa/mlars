#pragma once
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace lars {
class Logger {
public:
  static Logger &getInstance();

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
  auto getLogger() { return logger_; }

private:
  Logger();
  ~Logger();

  std::shared_ptr<spdlog::logger> logger_;
};

// 修正后的宏定义，将 source_loc 存储在局部变量中
#define LOG_INTERNAL(level, format, ...)                                       \
  Logger::getInstance().getLogger()->log({__FILE__, __LINE__, __func__},       \
                                         level, format, ##__VA_ARGS__);

// 保持这些宏不变，它们将调用新的 LOG_INTERNAL
#define LOG_TRACE(format, ...)                                                 \
  LOG_INTERNAL(spdlog::level::trace, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)                                                 \
  LOG_INTERNAL(spdlog::level::debug, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)                                                  \
  LOG_INTERNAL(spdlog::level::info, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)                                                  \
  LOG_INTERNAL(spdlog::level::warn, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)                                                 \
  LOG_INTERNAL(spdlog::level::error, format, ##__VA_ARGS__)
#define LOG_CRITICAL(format, ...)                                              \
  LOG_INTERNAL(spdlog::level::critical, format, ##__VA_ARGS__)

} // namespace lars