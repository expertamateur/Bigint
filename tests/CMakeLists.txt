find_package(doctest REQUIRED)

# 创建测试可执行文件
add_executable(my_bigint_tests test.cpp)
target_compile_options(my_bigint_tests PRIVATE
    -Wall
    -Wpedantic
    -Wextra
    -Wshadow
)
# 链接库
target_link_libraries(my_bigint_tests PRIVATE my_bigint doctest::doctest)

# 添加测试
add_test(NAME MyBigIntTests COMMAND my_bigint_tests)
