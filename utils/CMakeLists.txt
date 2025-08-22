cmake_minimum_required(VERSION 3.10)
project(lars_utils)

# 自动查找 src 目录下的所有 .cpp 文件
# 确保所有源文件（包括 Logger.cpp）都在这个目录里
file(GLOB UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 创建共享库
add_library(lars_utils SHARED ${UTILS_SOURCES})

# 设置头文件搜索路径
target_include_directories(lars_utils PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 链接 spdlog 库
target_link_libraries(lars_utils PUBLIC spdlog::spdlog)

# 安装库和头文件
install(TARGETS lars_utils
  DESTINATION lib
)
install(DIRECTORY include/
  DESTINATION include
)