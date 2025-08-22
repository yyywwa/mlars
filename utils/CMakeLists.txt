# utils/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(lars_utils)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 自动查找 src 目录下的所有 .cpp 文件
file(GLOB UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 创建共享库
# add_library 的参数现在是自动发现的源文件列表
add_library(lars_utils SHARED ${UTILS_SOURCES})

# 设置头文件搜索路径
# PUBLIC 包含是最佳实践，它将 include 目录暴露给所有链接 lars_utils 的目标
target_include_directories(lars_utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 链接 spdlog
# INTERFACE 意味着 spdlog 依赖将传递给所有使用 lars_utils 的目标
target_link_libraries(lars_utils INTERFACE spdlog::spdlog)

# 安装库和头文件
install(TARGETS lars_utils
  DESTINATION lib
)
install(DIRECTORY include/
  DESTINATION include
)