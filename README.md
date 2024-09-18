### 大整数练习
- 使用C++11~20特性
- 通过裸指针管理原始数组实现
#### 8-14
- 改用 `std::string_view` 代替 `std::string` 处理 `const char*`
- 修复了乘法错误
- 改用 `std::size_t`
- 改用 `std::fill` 完成填充 0 操作
- 改用 `std::memcpy` 和 `std::memmove` 来完成复制和位运算

#### 9-1
- 分离构造函数，将可手动设置大小的构造函数设为私有，只接收 `uintmax_t`
- 加入 `divide` 函数，重载 `%` 运算符
- 取余运算符重载
#### 9-5
- 加入 `assert` 来进行调试

#### 9-12
- 加入 `concept`，严格限制只允许整数类和 `std::string` 类作为初始化参数
- 处理字符串负号和非法字符

### To Do
- 定义和实现分离
- 定义右值重载
- 重写bigint/bigint除法

