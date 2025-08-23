#include "logger.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
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

  // 创建文件 sink，使其以追加模式打开
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/lars.log", false);

  // 创建一个临时的、仅用于写入启动分隔符的 logger
  {
    auto startup_logger =
        std::make_shared<spdlog::logger>("startup_logger", file_sink);
    startup_logger->set_level(spdlog::level::info); // 只需 info 级别
    // 写入分隔符。这里我们使用一个简单的模式以避免在文件中重复显示时间戳等信息
    startup_logger->set_pattern("%v");
    startup_logger->info(
        "================================================================");
  } // 临时 logger 在此超出作用域并销毁

  // 将所有 sinks 放入 vector，用于主 logger
  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  // 创建主 logger，它会继承上面设置的全局模式
  logger_ = std::make_shared<spdlog::logger>("lars_logger", sinks.begin(),
                                             sinks.end());

  spdlog::set_default_logger(logger_);
  logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%!]\n%v");
  logger_->set_level(spdlog::level::info);
  logger_->flush_on(spdlog::level::info);
}

Logger::~Logger() {
  logger_->flush();
  spdlog::drop_all();
}

} // namespace lars