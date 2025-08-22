#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <iostream>

int main() {
    try {
        // 创建一个名为 "file_logger" 的日志器，并将日志输出到 logs/my_app.log 文件
        // 这里的 spdlog::basic_logger_mt 是一个多线程安全的日志器
        auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/my_app.log");
        
        // 设置日志级别，只有 info 级别及以上的日志才会被记录
        file_logger->set_level(spdlog::level::info);
        
        // 设置默认日志器，这样我们就可以像 spdlog::info(...) 一样使用它
        spdlog::set_default_logger(file_logger);

        // 使用日志器记录日志
        spdlog::info("应用程序启动...");
        spdlog::warn("配置加载失败，使用默认配置。");
        spdlog::error("数据库连接错误！");
        
        // 设置不同的日志级别，可以看到哪些日志被过滤了
        spdlog::debug("这是一个调试信息，不会被写入日志文件。");

    } catch (const spdlog::spdlog_ex& ex) {
        // 捕获可能发生的异常，比如文件无法创建
        std::cerr << "日志初始化失败: " << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}
