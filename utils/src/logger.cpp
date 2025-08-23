#include "logger.h"
#include "spdlog/common.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>

namespace lars {

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

Logger::Logger() {
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
  logger_ = std::make_shared<spdlog::logger>("lars_logger", sinks.begin(),
                                             sinks.end());

  // 重要：在这里为你的 logger 实例设置模式，而不是在全局设置
  logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%!]\n%v");

  logger_->set_level(spdlog::level::trace);
  logger_->flush_on(spdlog::level::trace);

  spdlog::set_default_logger(logger_);
}

Logger::~Logger() {
  logger_->flush();
  spdlog::drop_all(); // 释放所有注册的 logger
}

} // namespace lars