// my_bigint.cpp
#include "my_bigint.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <utility>

BigInt::BigInt(std::uintmax_t initialValue, const size_type _size,
               const bool _negative)
    : size_{_size}, length_{1},
      array_{new value_type[std::max(size_, size_type{1})]},
      negative_{_negative} {
  if (!initialValue) { // 无初始值的构造
    if (size_ < 1)
      size_ = 1;
    array_[0] = 0;
    return;
  }
  size_type i = 0;
  while (initialValue) {
    array_[i++] = initialValue % 10;
    initialValue /= 10;
  }
  this->getLength(i);
}
BigInt::BigInt(const BigInt &N)
    : size_(N.size_), length_(N.length_), array_(new value_type[N.size_]),
      negative_(N.negative_) {
  std::memcpy(array_, N.array_, length_ * sizeof(value_type));
}

BigInt::BigInt(BigInt &&N)
    : size_(N.size_), length_(N.length_),
      array_(std::exchange(N.array_, nullptr)), negative_(N.negative_) {}

BigInt::BigInt(std::string_view initialValue)
    : size_(initialValue.size()), length_(initialValue.size()),
      array_(new value_type[std::max(size_, size_type{1})]) {
  if (!size_) { // 处理空字符串的特殊情况
    size_ = 1;
    length_ = 1;
    array_[0] = 0;
    return;
  }
  if (initialValue.front() == '-') {
    negative_ = true;
    initialValue.remove_prefix(1); // 删除开头的‘-’号
    --length_;
    array_[0] = 0; // 避免字符串为单‘-’的情况
  }
  // 调试时检测初始字符串是否为数字
  assert(std::all_of(initialValue.cbegin(), initialValue.cend(), isdigit));
  // 运行时检测初始字符串是否都是数字
  if (!std::all_of(initialValue.cbegin(), initialValue.cend(), isdigit)) {
    delete[] array_;
    array_ = nullptr;
    throw std::invalid_argument("string is't pure digtil");
  }
  // 字符串大端序，转小端序bigint需要逆序遍历
  std::transform(initialValue.crbegin(), initialValue.crend(), this->begin(),
                 [](char x) -> value_type { return x - '0'; });
  this->getLength(length_);
}
auto BigInt::operator=(const BigInt &N) -> BigInt & {
  if (this == &N)
    return *this;
  if (size_ < N.size_)
    return *this = std::move(BigInt{N});
  negative_ = N.negative_;
  length_ = N.length_;
  std::memcpy(array_, N.array_, length_ * sizeof(value_type));
  return *this;
}
auto BigInt::operator=(BigInt &&N) -> BigInt & {
  if (this == &N)
    return *this;
  length_ = N.length_;
  size_ = N.size_;
  negative_ = N.negative_;
  if (array_)
    delete[] array_;
  array_ = std::exchange(N.array_, nullptr);
  return *this;
}
auto BigInt::divide_integer_uint(std::uintmax_t N, bool N_is_negative) const
    -> bigInt_division_result {
  if (Is_zero()) {
    return bigInt_division_result{};
  }
  if (!N) {
    throw std::invalid_argument{"can't divide by zero"};
  }
  std::intmax_t remainder{0};
  BigInt quotient(0, this->length_);
  for (size_type i = this->length_; i-- > 0;) {
    remainder = (remainder * 10) + this->array_[i];
    if (remainder < N)
      quotient[i] = 0;
    else {
      quotient[i] = remainder / N;
      remainder %= N;
    }
  }
  quotient.negative_ = (negative_ != N_is_negative);
  quotient.getLength(quotient.size_);
  if (this->negative_) {
    remainder = -remainder;
  }
  // return result;
  return bigInt_division_result{quotient, BigInt{remainder}};
}

auto BigInt::fillZero() -> void {
  std::fill(array_, array_ + size_, 0);
  length_ = 1;
  negative_ = false;
}
auto BigInt::getLength(const size_type maxlength) -> size_type {
  for (length_ = maxlength; array_[length_ - 1] == 0 && length_ > 1; length_--)
    ;
  un_negative_zero();
  // 断言出现错误说明其他地方实现存在问题
  assert(std::all_of(begin(), end(), [](auto x) { return 0 <= x && x <= 9; }));
  return length_;
}
auto BigInt::un_negative_zero() -> bool {
  if (length_ == 1 && array_[0] == 0) {
    negative_ = false;
    return true;
  }
  return false;
}
[[nodiscard]] auto BigInt::Is_zero() const -> bool {
  if (length_ == 1 && array_[0] == 0) {
    return true;
  }
  return false;
}
[[nodiscard]] auto BigInt::pow_of_ten(const std::intmax_t n) const -> BigInt {
  if (!n) {
    return BigInt{*this};
  }
  if (n < 0 && static_cast<size_type>(-n) >= this->length_) {
    return BigInt{0};
  }
  BigInt result(0, length_ + n);
  // （result.zise_，max(0,n）】段赋值为（this->sise_-n，max(-n,0)】
  // （max(0-n,0)，0】段赋值为0  lenth=this->size_-n-max(0-n,0)
  //

  const size_type begin_copy = n > 0 ? static_cast<size_type>(n) : 0;
  const size_type copy_length = result.size_ - begin_copy;
  if (copy_length) {
    const size_type origin_begin = n < 0 ? static_cast<size_type>(-n) : 0;
    std::memcpy(result.array_ + begin_copy, this->array_ + origin_begin,
                (copy_length) * sizeof(value_type));
  }
  if (begin_copy)
    std::fill(result.array_, result.array_ + begin_copy, 0);

  // 原始版本
  // （result.zise_，max(0,n）】段赋值为（this->sise_-n，max(0-n,0)】
  // （max(0-n,0)，0】段赋值为0

  // // size_type j = std::max(n, static_cast<std::intmax_t>(0));
  // for (size_type i = result.size_; i-- != 0;)
  //   // result[i] = (i >= j) ? this->array_[i - n] : 0;
  //   result[i] = std::cmp_greater_equal(i, n) ? this->array_[i - n] : 0;
  // // result[i] = ((i <=> n) >= 0) ? this->array_[i - n] : 0;

  result.getLength(result.size_);
  return result;
}
auto BigInt::back_inserter(const value_type n) -> BigInt {
  // 如果断言失败说明其他地方实现有问题
  assert(length_ < size_);
  for (size_type i = length_; i-- > 0;)
    array_[i + 1] = array_[i];
  array_[0] = n;
  getLength(length_ + 1);
  return *this;
}
auto BigInt::operator-() const -> BigInt {
  if (Is_zero()) {
    return BigInt{0};
  }
  BigInt result{*this};
  result.negative_ = !result.negative_;
  return result;
}
auto BigInt::abs(const BigInt &N) -> BigInt {
  BigInt temp{N};
  temp.negative_ = false;
  return temp;
}
auto BigInt::absAdd(const BigInt &N) const -> BigInt {
  BigInt result(0, std::max(length_, N.length_) + 1);
  value_type temp = 0;
  for (size_type i = 0; i < result.size_; i++) {
    temp =
        temp / 10 + (i < length_ ? array_[i] : 0) + (i < N.length_ ? N[i] : 0);
    result[i] = temp % 10;
  }
  result.getLength(result.size_);
  return result;
}
auto BigInt::absSub(const BigInt &N) const -> BigInt {
  if (cmp_abs_equal(*this, N)) // 相等直接输出0，避免减法产生负0
    // return BigInt(0, 1);//为了优化除法，改成如下形式
    return BigInt(0, size_);
  // BigInt result(0, length);//为了优化除法，改成如下形式
  BigInt result(0, size_);
  value_type temp = 0;
  // 优化0.03秒
  temp = array_[0] - N[0];
  for (size_type i = 0; i < length_;) // 大减去小,所以用this的长度
  {
    // 1.08s，编译器优化前，下面代码最快
    // if (temp < 0)
    // {
    //   result[i++] = temp + 10;
    //   temp = (i < N.length ? array[i] - N[i] : array[i]) - 1;
    // }
    // else
    // {
    //   result[i++] = temp;
    //   temp = i < N.length ? array[i] - N[i] : array[i];
    // }

    // 1.09,编译器优化后，下面代码最快
    result[i++] = (temp < 0) ? (temp + 10) : temp;
    temp = (i < N.length_ ? array_[i] - N[i] : array_[i]) - (temp < 0 ? 1 : 0);
  }

  // 原始版本// 优化前1.11
  // for (size_type i = 0; i < length; i++) // 大减去小,所以用this的长度
  // {
  //   temp = array[i] - (i < N.length ? N[i] : 0) + (temp < 0 ? -1 : 0);
  //   result[i] = temp + (temp < 0 ? 10 : 0);
  // }

  result.getLength(length_);
  return result;
}
auto BigInt::samNSub(const BigInt &N) const -> BigInt {
  // 绝对值较大的减去绝对值较小的
  if (this->cmp_abs_less(N)) { // return -(N - *this);
    BigInt result(N.absSub(*this));
    result.negative_ = !this->negative_; // 无视N符号来继承
    result.un_negative_zero();           // 防止-0
    return result;
  }
  BigInt result(this->absSub(N));
  result.negative_ = this->negative_;
  result.un_negative_zero(); // 防止-0
  return result;
}
// 同号加法,无视N符号继承this符号
auto BigInt::samNAdd(const BigInt &N) const -> BigInt {
  BigInt result(this->absAdd(N));
  result.negative_ = this->negative_;
  result.un_negative_zero(); // 防止-0
  return result;
}
auto BigInt::divide(const BigInt &N) const -> bigInt_division_result {
  // 大整数之间除法，效率很低
  if (N.Is_zero()) {
    throw std::invalid_argument{"can't divide by zero"};
  }
  // 被除数更大的时候直接返回0
  if (this->cmp_abs_less(N))
    return bigInt_division_result{}; // 可能在内部调用时造成泄露?
  // 针对求pi函数优化
  if (N == BigInt("10000000000")) {
    BigInt remainder{0, 10};
    std::copy(this->array_, this->array_ + 10, remainder.array_);
    return bigInt_division_result{this->pow_of_ten(-10), remainder};
  }

  // 下面除法最好重写,效率过低
  BigInt quotient(0, length_ - N.length_ + 1),
      remainder(0, N.length_ + 1); // （1）余数为正
  quotient.fillZero();
  for (size_type i = length_; i-- > 0;) // T(9n)
  {
    remainder.back_inserter(array_[i]);
    // 如果后面的减法运算重置remainder的容量，造成内存溢出错误
    while (!remainder.cmp_abs_less(N)) {
      // 减法需要重新开辟空间，不合算，考虑二分查找猜被除数
      remainder = remainder.absSub(N);
      // 涉及到减法，所以之前要将涉及的数全部转换为正数计算（1）
      ++quotient[i];
    }
  }
  quotient.negative_ = (negative_ != N.negative_); // 同号为正
  quotient.getLength(quotient.size_);
  remainder.negative_ = this->negative_;
  return bigInt_division_result{quotient, remainder};
}

auto pow(const BigInt &N, const std::size_t &exponent) -> BigInt {
  // 针对求pi函数的优化
  if (N == BigInt(10))
    return N.pow_of_ten(exponent - 1);
  BigInt result(1, N.length_ * exponent);
  for (std::size_t i = 0; i < exponent; i++)
    result *= N;
  return result;
}

auto BigInt::operator+(const BigInt &N) const -> BigInt {
  // O(n)// 处理同号加法，异号转变为同号减法
  if (N.Is_zero()) {
    return BigInt(*this);
  }
  if (Is_zero()) {
    return BigInt{N};
  }
  if (N.negative_ != negative_) // 异号转变为同号减法
    return this->samNSub(N);    // return *this - (-N);
  return this->samNAdd(N);
}
auto BigInt::operator-(const BigInt &N) const -> BigInt {
  // O(n) 处理同号减法，异号转变为同号加法
  if (N.Is_zero()) {
    return BigInt{0};
  }
  if (Is_zero()) {
    BigInt result{N};
    result.negative_ = !result.negative_;
    // result.Is_zero();//多余，函数已经在上面返回了
    return result;
  }
  if (N.negative_ != negative_) // 异号转变为同号加法
    return this->samNAdd(N);    // return *this + (-N);
  return this->samNSub(N);
}
auto BigInt::operator*(const BigInt &N) const -> BigInt {
  // O(n^2) //
  if (N.Is_zero() || Is_zero()) {
    return BigInt{0};
  }
  // 乘法结果长度最多为两乘数之和，最多进1位
  BigInt result{0, length_ + N.length_};
  result.fillZero();
  value_type temp = 0;
  // size_type s{0};
  size_type i{0}, j{0};
  for (i = 0; i < N.length_; i++) {
    for (j = 0; j < length_; j++) {
      temp = result[i + j] + temp / 10 + N[i] * array_[j];
      // temp最大值为（9+9+9*9=99）<100,进位不会超过1位
      result[i + j] = temp % 10;

      // cout << "s,i" << i << "j" << j << "=" << (s = N[i] * array[j]) <<
      // '\n'; cout << "temp=" << (s = temp) << '\n'; cout << "R" << i + j
      // <<
      // "=" << (s = temp % 10) << '\n' << '\n';
    }
    // result[i + j] = result[i + j] + temp / 10;
    // 处理多余进位，因为前面的注释，进位最多不会超过1位，所以只需要处理1次
    // i + j 位 从来没有被访问过，为0，优化如下
    result[i + j] = temp / 10;
    temp = 0;
    // cout << "result=" << result << '\n';
    // cout << "*************" << '\n';
  }

  result.getLength(result.size_);
  result.negative_ = (negative_ != N.negative_); // 同号为正
  return result;
}
auto BigInt::operator/(const BigInt &N) const -> BigInt {
  return divide(N).quotient;
}
auto BigInt::operator%(const BigInt &N) const -> BigInt {
  return divide(N).remainder;
}
auto BigInt::cmp_abs_equal(const BigInt &A, const BigInt &B) -> bool {
  if (A.length_ != B.length_)
    return false;
  for (size_type i = 0; i < A.length_; i++)
    if (A[i] != B[i])
      return false;
  return true;
}
auto BigInt::cmp_abs_less(const BigInt &N) const -> bool {
  if (length_ < N.length_)
    return true;
  if (length_ > N.length_)
    return false;
  for (size_type i = length_; i-- > 0;)
    if (this->array_[i] != N[i])
      return this->array_[i] < N[i];
  return false;
}