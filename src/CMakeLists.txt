# 创建库
add_library(my_bigint STATIC
    my_bigint.cpp
)

# 设定头文件目录
target_include_directories(my_bigint PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# 目标链接库（如果有的话）
# target_link_libraries(my_bigint PRIVATE some_other_library)
