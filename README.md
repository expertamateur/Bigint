裸指针版本
8-14
改用string_view代替string处理 const char *
修复了乘法错误
改用std::size_t
改用std::fill完成填充0操作
改用std::memcpy 和 std::menmove来完成复制和位运算
9-1
分离构造函数，将可手动设置大小的构造函数设为私有,只接收uintmax_t
9-5
加入断言来进行调试
9-12
加入concept 严格限制只允许整数类和string类作为初始化参数
to do
定义和实现分离
加入divede函数，重载%运算符over
定义右值-重载over
取余运算符重载
未完成事项
1.复制函数改成内存块复制,写零同理
2.处理字符串负号和非法字符
3.bigint间除法重写，除法多态优化
4.进制自定义，优化
5.大整数算法加入
6.variant加入
7.为const_get_10_digit和其他私有函数创建namespace
