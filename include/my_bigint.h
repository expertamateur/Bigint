// my_bigint.h
#ifndef MY_BIGINT_H
#define MY_BIGINT_H
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>

// 小端序，裸指针配合数组   效率高于>>智能指针和vector
// 接受所有整数类和类string类的参数用于构造
class BigInt {
private:
  using value_type = std::int_fast8_t;
  using size_type = std::size_t;
  struct bigInt_division_result;
  size_type size_, length_;
  // 编译器优化前裸指针配合数组效率高于uniqe_ptr和vector，计算20000位pi时，时间差距达到20秒以上
  value_type *array_;
  bool negative_{false};
  auto begin() -> value_type * { return array_; }
  auto end() -> value_type * { return array_ + length_; }
  // 静态根据源整数类型推断数组容量
  template <std::integral T>
  [[nodiscard]] static constexpr size_type get_initial_size() noexcept;
  // 编译器取得初始哈数据大小
  template <std::integral T>
  static constexpr size_type initial_size = get_initial_size<T>();

  // 可以手动预分配内存,私有,人工保证_size参数合法性
  // o(n)，n：初始位数//容量至少为1,初始内容为0
  explicit BigInt(std::uintmax_t initialValue, const size_type _size,
                  const bool _negative = false);

public:
  ~BigInt() { delete[] array_; }
  BigInt() : BigInt(0, 1, false) {}
  // 接收不同整数，根据不同尺寸的整数初始化大小不同bigint
  template <typename T>
    requires std::integral<T> && std::is_signed_v<T>
  explicit BigInt(T initialValue)
      : BigInt{static_cast<std::uintmax_t>(std::abs(initialValue)),
               initial_size<T>, initialValue < T{0}} {}
  template <typename T>
    requires std::integral<T> && std::is_unsigned_v<T>
  explicit BigInt(T initialValue)
      : BigInt{static_cast<std::uintmax_t>(initialValue), initial_size<T>,
               false} {}
  // 从字符串初始化，没有非法符号检查
  explicit BigInt(std::string_view initialValue);

  // 不复制多余脏数据空间
  BigInt(const BigInt &N);
  // 移动构造
  BigInt(BigInt &&N);
  // assign赋值复制运算
  auto operator=(const BigInt &N) -> BigInt &;
  // 移动复制运算符
  auto operator=(BigInt &&N) -> BigInt &;
  // 计算算法
private:
  // 默认构造不初始化，内部实现其他函数的时候调用
  auto fillZero() -> void;
  // 计算后必须调用此函数，逻辑上去除计算后最高位的0，同时调试版本检查计算结果，避免-0的存在
  auto getLength(const size_type maxlength) -> size_type;
  // 没必要的私有函数
  // [[nodiscard]] auto getLength() const -> size_type { return length_; }

  // 判断是否为0，同时如果是-0则改为0
  auto un_negative_zero() -> bool;
  // 内部判断是否为0
  [[nodiscard]] auto Is_zero() const -> bool;

public:
  [[nodiscard]] auto pow_of_ten(const std::intmax_t n) const -> BigInt;

private:
  // x= x*10+n,x需要提前手动分配好足够空间
  auto back_inserter(const value_type n) -> BigInt;

  // 公共简单函数类，开销小的尽量内联，类内定义
public:
  // 逆序输出，
  friend auto operator<<(std::ostream &out, const BigInt &N) -> std::ostream & {
    out << (N.negative_ ? "-" : "");
    for (size_type i = N.length_; i-- > 0;)
      out << static_cast<int>(N[i]);
    return out;
  }
  // O(n)单目运算符取反，完整复制构造开销
  auto operator-() const -> BigInt;
  // 右值取反，用于处理临时对象,避免构造开销
  friend auto operator-(BigInt &&N) -> BigInt & {
    N.negative_ = !N.negative_;
    return N;
  }
  // 复制构造开销
  static auto abs(const BigInt &N) -> BigInt;
  // 右值取绝对值，避免复制构造开销
  static auto abs(BigInt &&N) -> BigInt {
    N.negative_ = false;
    return N;
  }
  // 比较算符
private:
  // 比较绝对值相同,从最低位开始比较，遇到不一样的数字输出false
  static auto cmp_abs_equal(const BigInt &A, const BigInt &B) -> bool;
  // 比较绝对值大小
  auto cmp_abs_less(const BigInt &N) const -> bool;

  // 效率降低，编译器O3优化后不影响,用于内部计算函数
  auto operator[](const size_type index) const -> value_type & {
    return array_[index];
  }

  // 定义下面的函数避免基本加减法互相委托的构造开销

  // 返回绝对值加法,运算结果为正
  auto absAdd(const BigInt &N) const -> BigInt;
  // 返回绝对值减法 要求大减小，否则出错，运算结果为正
  auto absSub(const BigInt &N) const -> BigInt;
  // 同号减法,无视N符号继承this符号
  auto samNSub(const BigInt &N) const -> BigInt;
  // 同号加法,无视N符号继承this符号
  auto samNAdd(const BigInt &N) const -> BigInt;

  // 用于/%重载
  auto divide(const BigInt &N) const -> bigInt_division_result;
  // 利用基本整数类加速运算
  auto divide_integer_uint(std::uintmax_t N,
                           bool N_is_negative) const -> bigInt_division_result;
  // 有符号整数和无符号整数处理方式不同
  template <typename T>
    requires std::integral<T> && std::is_signed_v<T>
  auto divide_integer(T N) const -> bigInt_division_result;
  template <typename T>
    requires std::integral<T> && std::is_unsigned_v<T>
  auto divide_integer(T N) const -> bigInt_division_result;

public:
  auto operator+(const BigInt &N) const -> BigInt;
  auto operator-(const BigInt &N) const -> BigInt;
  auto operator*(const BigInt &N) const -> BigInt;
  // 大整数之间除法，效率很低
  auto operator/(const BigInt &N) const -> BigInt;
  auto operator%(const BigInt &N) const -> BigInt;
  // 不支持负数幂运算
  friend auto pow(const BigInt &N, const size_type &exponent) -> BigInt;

  // 算符重载没有

  template <class T> auto operator+(const T &N) const -> BigInt {
    return *this + BigInt{N};
  }
  template <class T> auto operator-(const T &N) const -> BigInt {
    return *this - BigInt{N};
  }
  template <class T> auto operator*(const T &N) const -> BigInt {
    return *this * BigInt{N};
  }
  // 除整数重载 利用基本整数类加速运算
  template <std::integral T> auto operator/(T N) const -> BigInt {
    return divide_integer(N).quotient;
  }
  // 字符串重载
  auto operator/(std::string_view N) const -> BigInt {
    return *this / BigInt{N};
  }
  template <std::integral T> auto operator%(T N) const -> BigInt {
    return divide_integer(N).remainder;
  }
  auto operator%(std::string_view N) const -> BigInt {
    return *this % BigInt{N};
  }

  template <class T>
  friend auto operator+(const T &N, const BigInt &B) -> BigInt {
    return BigInt{N} + B;
  }
  template <class T>
  friend auto operator-(const T &N, const BigInt &B) -> BigInt {
    return BigInt{N} - B;
  }
  template <class T>
  friend auto operator*(const T &N, const BigInt &B) -> BigInt {
    return BigInt{N} * B;
  }
  template <class T>
  friend auto operator/(const T &N, const BigInt &B) -> BigInt {
    return BigInt{N} / B;
  }
  template <class T>
  friend auto operator%(const T &N, const BigInt &B) -> BigInt {
    return BigInt{N} % B;
  }
  template <class T> auto operator=(const T &N) -> BigInt & {
    return *this = BigInt{N};
  }
  template <class T> auto operator+=(const T &N) -> BigInt & {
    return *this = *this + N;
  }
  template <class T> auto operator-=(const T &N) -> BigInt & {
    return *this = *this - N;
  }
  template <class T> auto operator*=(const T &N) -> BigInt & {
    return *this = *this * N;
  }
  template <class T> auto operator/=(const T &N) -> BigInt & {
    return *this = *this / N;
  }
  template <class T> auto operator%=(const T &N) -> BigInt & {
    return *this = *this % N;
  }

public:
  auto operator==(const BigInt &N) const noexcept -> bool {
    if (negative_ != N.negative_)
      return false;
    return cmp_abs_equal(*this, N);
  }
  // 从最高位开始依次比较，输出第一个不同数字位的比较结果
  auto operator<(const BigInt &N) const -> bool {
    if (!negative_ && N.negative_) // 正数大于负数
      return false;
    if (negative_ && !N.negative_) // 负数小于正数
      return true;
    // 同为负数转换为正数比较//return !(-*this < -N);
    if (negative_ && N.negative_)
      return !cmp_abs_less(N);
    return cmp_abs_less(N);
  }
  auto operator!=(const BigInt &N) const -> bool { return !(*this == N); }
  auto operator>(const BigInt &N) const -> bool { return N < *this; }
  auto operator>=(const BigInt &N) const -> bool { return !(*this < N); }
  auto operator<=(const BigInt &N) const -> bool { return !(*this > N); }

  // // 以下为未使用函数

  //   static auto isTenOfExponent-> bool(const BigInt &N);
  //   static auto crateTenExponent->BigInt(const size_type len);
};

struct BigInt::bigInt_division_result {
  BigInt quotient{0};  // 商
  BigInt remainder{0}; // 余数
};

template <std::integral T>
[[nodiscard]] constexpr size_t BigInt::get_initial_size() noexcept {
  static_assert(sizeof(T) <= size_t{8}, "uknow int type ,add it to code");
  switch (sizeof(T)) {
  case 1:
    return 3;
  case 2:
    return 5;
  case 3:
    return 8;
  case 4:
    return 10;
  case 5:
    return 13;
  case 6:
    return 15;
  case 7:
    return 17;
  case 8:
    return 20;
    // default:
    //   return 20;
  }
}

template <typename T>
  requires std::integral<T> && std::is_unsigned_v<T>
auto BigInt::divide_integer(T N) const -> bigInt_division_result {
  return divide_integer_uint(static_cast<std::uintmax_t>(N), false);
}
template <typename T>
  requires std::integral<T> && std::is_signed_v<T>
auto BigInt::divide_integer(T N) const -> bigInt_division_result {
  return divide_integer_uint(static_cast<std::uintmax_t>(std::abs(N)),
                             N < T{0});
}

#endif //  MY_BIGINT_H