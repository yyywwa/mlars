# lars/CMakeLists.txt
cmake_minimum_required(VERSION 3.10.0)
project(lars VERSION 0.1.0 LANGUAGES C CXX)

# 为所有目标设置 -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 设置 C++20 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加 spdlog 库目录
add_subdirectory(third_party/spdlog)

# 添加你的 utils 库目录
add_subdirectory(utils)

add_executable(lars main.cpp)

# 只链接到你的 utils 库，spdlog 的依赖会从 utils 传递过来
target_link_libraries(lars PRIVATE lars_utils)