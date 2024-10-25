#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "my_bigint.h"
#include <stdexcept>

auto PI(int) -> BigInt;

// 测试 BigInt 和基本整数的加减乘除法
TEST_CASE("BigInt and basic integer addition tests") {
  BigInt a("12345678901234567890");

  SUBCASE("BigInt + int") {
    int b = 12345;
    CHECK((a + b) == BigInt("12345678901234580235"));
  }

  SUBCASE("int + BigInt") {
    int b = 12345;
    CHECK((b + a) == BigInt("12345678901234580235"));
  }

  SUBCASE("BigInt + long") {
    long b = 987654321;
    CHECK((a + b) == BigInt("12345678902222222211"));
  }

  SUBCASE("long + BigInt") {
    long b = 987654321;
    CHECK((b + a) == BigInt("12345678902222222211"));
  }

  SUBCASE("BigInt + unsigned int") {
    unsigned int b = 1000;
    CHECK((a + b) == BigInt("12345678901234568890"));
  }

  SUBCASE("unsigned int + BigInt") {
    unsigned int b = 1000;
    CHECK((b + a) == BigInt("12345678901234568890"));
  }
}

TEST_CASE("BigInt and basic integer subtraction tests") {
  BigInt a("12345678901234567890");

  SUBCASE("BigInt - int") {
    int b = 12345;
    CHECK((a - b) == BigInt("12345678901234555545"));
  }

  SUBCASE("int - BigInt") {
    int b = 12345;
    CHECK((b - a) == BigInt("-12345678901234555545"));
  }

  SUBCASE("BigInt - long") {
    long b = 987654321;
    CHECK((a - b) == BigInt("12345678900246913569"));
  }

  SUBCASE("long - BigInt") {
    long b = 987654321;
    CHECK((b - a) == BigInt("-12345678900246913569"));
  }

  SUBCASE("BigInt - unsigned int") {
    unsigned int b = 1000;
    CHECK((a - b) == BigInt("12345678901234566890"));
  }

  SUBCASE("unsigned int - BigInt") {
    unsigned int b = 1000;
    CHECK((b - a) == BigInt("-12345678901234566890"));
  }
}

TEST_CASE("BigInt and basic integer multiplication tests") {
  BigInt a("12345678901234567890");

  SUBCASE("BigInt * int") {
    int b = 12345;
    CHECK((a * b) == BigInt("152407406035740740602050"));
  }

  SUBCASE("int * BigInt") {
    int b = 12345;
    CHECK((b * a) == BigInt("152407406035740740602050"));
  }

  SUBCASE("BigInt * long") {
    long b = 987654321;
    CHECK((a * b) == BigInt("12193263112482853211126352690"));
  }

  SUBCASE("long * BigInt") {
    long b = 987654321;
    CHECK((b * a) == BigInt("12193263112482853211126352690"));
  }

  SUBCASE("BigInt * unsigned int") {
    unsigned int b = 1000;
    CHECK((a * b) == BigInt("12345678901234567890000"));
  }

  SUBCASE("unsigned int * BigInt") {
    unsigned int b = 1000;
    CHECK((b * a) == BigInt("12345678901234567890000"));
  }
}

TEST_CASE("BigInt and basic integer division tests") {
  BigInt a("12345678901234567890");

  SUBCASE("BigInt / int") {
    int b = 12345;
    CHECK((a / b) == BigInt("1000054994024671"));
  }

  SUBCASE("int / BigInt") {
    int b = 12345;
    CHECK(b / a == BigInt{0});
  }

  SUBCASE("BigInt / long") {
    long b = 987654321;
    CHECK((a / b) == BigInt("12499999887"));
  }

  SUBCASE("long / BigInt") {
    long b = 987654321;
    CHECK((b / a) == BigInt{0});
  }

  SUBCASE("BigInt / unsigned int") {
    unsigned int b = 1000;
    CHECK((a / b) == BigInt("12345678901234567"));
  }

  SUBCASE("unsigned int / BigInt") {
    unsigned int b = 1000;

    CHECK((b / a) == BigInt{0});
  }
  SUBCASE("dived zero") {
    int b = 0;
    CHECK_THROWS_AS(a / b, std::invalid_argument);
  }
}

TEST_CASE("old test") {
  BigInt n1(7654321);
  BigInt n2(7891234);
  BigInt n3("12341234");
  CHECK(n1 + 9 == BigInt{7654330});
  CHECK(n2 + 10000 == BigInt{7901234});
  CHECK(n3 * 7888 == BigInt{97347653792});
  CHECK(n3 - 7888 == BigInt{12333346});
  CHECK(7888 - n3 == BigInt{-12333346});
}

TEST_CASE("test order") {
  BigInt n1(7654321);
  BigInt n2(7891234);
  CHECK(!(n1 == n2));
  CHECK(n1 != n2);
  CHECK(n1 < n2);
  CHECK(n1 <= n2);
  CHECK(!(n1 > n2));
  CHECK(!(n1 >= n2));
}
TEST_CASE("Addition basic tests") {
  BigInt a = BigInt("123");
  BigInt b = BigInt("456");
  CHECK(12 + a == BigInt{"135"});
  CHECK((a + b) == BigInt("579"));
}
TEST_CASE("Addition with negatives") {
  BigInt a = BigInt("-123");
  BigInt b = BigInt("456");

  CHECK((a + b) == BigInt("333"));
}
TEST_CASE("Addition edge cases") {
  BigInt a = BigInt("0");
  BigInt b = BigInt("999999999999999999");
  BigInt n3("99999999999999999999999999999");

  CHECK((a + b) == BigInt("999999999999999999"));
  CHECK(n3 + 1 == BigInt{"100000000000000000000000000000"});
  CHECK(BigInt{"999999999999999999999999999999999999999999999999999999999999999"
               "99999"} /
            BigInt{"-99999999999999999999999999999"} ==
        BigInt{"-1000000000000000000000000000010000000000"});
}
TEST_CASE("pow_of_ten") {
  BigInt a{54378};
  CHECK(a.pow_of_ten(10) == BigInt{54378'00000'00000});
  CHECK(a.pow_of_ten(-1) == BigInt{5437});
  CHECK(a.pow_of_ten(-2) == BigInt{543});
  CHECK(a.pow_of_ten(-5) == BigInt{0});
}
TEST_CASE("divide integer") {
  BigInt a{12341234};
  CHECK(a / 12341234 == BigInt{1});
  CHECK(a / -12341234 == BigInt{-1});
  CHECK(a / 7 == BigInt{1763033});
  CHECK(a / -7 == BigInt{-1763033});
  a = "12345678901234567890";
  BigInt b{12345};
  CHECK((a / b) == BigInt("1000054994024671"));
  CHECK(((-a) / b) == BigInt("-1000054994024671"));
  CHECK((a / (-b)) == BigInt("-1000054994024671"));
  CHECK(b / a == BigInt{0});
  b = 987654321;
  CHECK((a / b) == BigInt("12499999887"));
  CHECK((b / a) == BigInt{0});
  b = 1000;
  CHECK((a / b) == BigInt("12345678901234567"));
  CHECK((b / a) == BigInt{0});

  SUBCASE("dived zero") {
    BigInt c{0};
    CHECK_THROWS_AS(a / c, std::invalid_argument);
  }
}
TEST_CASE("divide Bigint") {
  BigInt a{12341234};

  BigInt b{7};
  CHECK(a / a == BigInt{1});
  CHECK(a / -a == BigInt{-1});
  CHECK(a / b == BigInt{"1763033"});
  CHECK((-a) / b == BigInt{"-1763033"});
  CHECK(a / -b == BigInt{"-1763033"});
}
TEST_CASE("mod Bigint") {
  BigInt a{12341234};
  BigInt b{7};
  CHECK(a % a == BigInt{0});
  CHECK(a % -a == BigInt{0});
  CHECK(a % b == BigInt{"3"});
  CHECK((-a) % b == BigInt{"-3"});
  CHECK((-a) % (-b) == BigInt{"-3"});
  CHECK(a % -b == BigInt{"3"});
}
TEST_CASE("mod integer") {
  BigInt a{12341234};
  int b{7};
  CHECK(a % a == BigInt{0});
  CHECK(a % -a == BigInt{0});
  CHECK(a % b == BigInt{"3"});
  CHECK((-a) % b == BigInt{"-3"});
  CHECK((-a) % (-b) == BigInt{"-3"});
  CHECK(a % -b == BigInt{"3"});
}
TEST_CASE("test pi") {
  CHECK(PI(3) == BigInt{314});
  CHECK(PI(8) == BigInt{"31415926"});
  CHECK(PI(100) == BigInt{"3141592653589793238462643383279502884197169399375105"
                          "820974944592307816406286208998628034825342117067"});
}
auto PI(int N) -> BigInt {

  // cout << "initial_size<int_fast8_t>:"
  //      << BigInt::initial_size<int_fast8_t> << "\n";
  int i;
  const BigInt n{10};
  BigInt b, x1, x2, s, t, pi;
  // auto start = std::chrono::steady_clock::now();
  N--;
  b = pow(n, N + 10);
  x1 = b * 4 / 5;
  x2 = b / -239;
  s = x1 + x2;
  for (i = 3; i <= N * 2; i += 2) {
    x1 /= -25;
    x2 /= -57121;
    t = (x1 + x2) / i;
    s += t;
  }
  pi = s * 4;
  BigInt result{pi / pow(n, 10)};
  // cout << result << '\n';
  // if (test) {
  //   auto end = std::chrono::steady_clock::now();
  //   std::chrono::duration<double> diff = end - start;
  //   cout << ++N << "位PI计算用时：" << diff.count() << " 秒" << '\n';
  // }
  return result;
}
