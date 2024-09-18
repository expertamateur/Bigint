cmake_minimum_required(VERSION 3.10)
project(MyBigIntLibrary)
set(CMAKE_VERBOSE_MAKEFILE ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子目录
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)
