#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace lars {

// 宏定义，在日志中自动添加文件名、行号和函数名
#define LARS_TRACE(...)                                                        \
  Logger::getInstance().trace(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LARS_DEBUG(...)                                                        \
  Logger::getInstance().debug(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LARS_INFO(...)                                                         \
  Logger::getInstance().info(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LARS_WARN(...)                                                         \
  Logger::getInstance().warn(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LARS_ERROR(...)                                                        \
  Logger::getInstance().error(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LARS_CRITICAL(...)                                                     \
  Logger::getInstance().critical(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

class Logger {
public:
  // 获取 Logger 单例的静态方法
  static Logger &getInstance();

  // 删除拷贝构造函数和赋值运算符
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // 公共日志接口
  void trace(const char *file, int line, const char *func,
             const std::string &msg);
  void debug(const char *file, int line, const char *func,
             const std::string &msg);
  void info(const char *file, int line, const char *func,
            const std::string &msg);
  void warn(const char *file, int line, const char *func,
            const std::string &msg);
  void error(const char *file, int line, const char *func,
             const std::string &msg);
  void critical(const char *file, int line, const char *func,
                const std::string &msg);

  // 刷新日志缓冲区
  void flush();

private:
  // 私有构造函数，保证只能通过 getInstance() 创建
  Logger();
  ~Logger(); // 私有析构函数

  // 格式化并记录日志的内部模板函数
  template <typename... Args>
  void log(spdlog::level::level_enum level, const char *file, int line,
           const char *func, const char *fmt, const Args &...args);

  std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace lars