#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "BigInt_Rational/rational.h"


void hard_test() {
  std::cout << "Biginteger hard test started!" << std::endl;
  BigInteger b, k;
  b = 0, k = 1234567;
  std::ostringstream oss;
  oss << b << ' ' << k;
  if (oss.str() != "0 1234567") {
    std::cerr << ("Assignment from int, or stream output failed.\n");
  }

  BigInteger a = b;
  a = -a;
  std::string testString = a.toString() + " " + (-k).toString();
  if (testString != "0 -1234567") {
    std::cerr << ("Unary minus or toString method failed.\n");
  }

  a = 999, b = 1000;
  a = a += a;
  testString = a.toString();
  if (testString != "1998") {
    std::cerr << ("Operator = or += failed.\n");
  }
  ++a %= a /= a *= a -= b++;
  std::ostringstream oss2;
  oss2 << 5 + a << ' ' << 1 - b;  // 5 -1000
  if (oss2.str() != "5 -1000") {
    std::cerr << oss2.str();
  }

  std::ostringstream oss3;
  oss3 << (a = (bool(a) ? -1 : -2));
  if (oss3.str() != "-2") {
    std::cerr << ("BigIntegerests failed.\n");
  }

  std::istringstream iss("26 5");
  iss >> a >> b;
  std::ostringstream oss4;
  oss4 << b << ' ' << a << ' ';
  if (oss4.str() != "5 26 ") {
    std::cerr << ("Stream input or output failed.\n");
  }

  oss4 << a + b << ' ' << a - b << ' ' << a * b << ' ' << a / b << ' ' << a % b
       << '\n';
  oss4 << b + a << ' ' << b - a << ' ' << b * a << ' ' << b / a << ' ' << b % a;
  if (oss4.str() != "5 26 31 21 130 5 1\n31 -21 130 0 5") {
    std::cerr << ("Arithmetic operations failed.\n");
  }

  std::vector<BigInteger> v;
  for (size_t i = 0; i < 1000; ++i) {
    v.push_back(1000 - i);
  }
  std::sort(v.begin(), v.end());

  std::ostringstream oss5;
  oss5 << v[0] << ' ' << v[500] << ' ' << v[999] << ' ';

  oss5 << (a != b) << ' ' << (a < b) << ' ' << (a > b) << ' ';
  oss5 << (a <= b) << ' ' << (a >= b);
  if (oss5.str() != "1 501 1000 1 0 1 0 1") {
    std::cerr << ("Rationalelation operations failed.\n");
  }
  std::istringstream iss2("1000000000000000000000000000000000 -1000000");
  iss2 >> a >> b;
  std::ostringstream oss6;
  oss6 << b << a;
  if (oss6.str() != "-10000001000000000000000000000000000000000") {
    std::cerr << ("Stream input or output failed.\n");
  }

  std::istringstream iss3(
      "453234523460009834520987234598234502345987029345436345634563 "
      "234523452034623049872345234520983475325345234232578");
  BigInteger c, d;
  iss3 >> c >> d;

  std::istringstream iss4(
      "-23534576554950000000000000009999990000999900000 "
      "8888888888884444444444433333333333332222222222222111112222777777777");
  BigInteger e, f;
  iss4 >> e >> f;

  std::ostringstream oss7;

  oss7 << a + b << ' ' << c + d << ' ' << e + f;
  std::cout << (a + b).toString() << '\n';
  std::cout << (a).toString() << '\n';
  std::cout << (b).toString() << '\n';

  if (oss7.str() !=
      "999999999999999999999999999000000 "
      "453234523694533286555610284470579736866970504670781579867141 "
      "8888888888884444444420898756778383332222222222212111122221777877777") {
    std::cerr << ("Operator + failed.\n");
  }

  std::ostringstream oss8;
  oss8 << a - b << ' ' << c - d << ' ' << e - f;
  if (oss8.str() !=
      "1000000000000000000000000001000000 "
      "453234523225486382486364184725889267825003554020091111401985 "
      "-8888888888884444444467967909888283332222222222232111102223777677777") {
    std::cerr << ("Operator - failed.\n");
  }

  std::ostringstream oss9;
  oss9 << a * b << ' ' << c * d << ' ' << e * f;
  if (oss9.str() !=
      "-1000000000000000000000000000000000000000 "
      "106294125023108851855435239407742287054686671354449187194200406809777590"
      "661604024572718537860109672117737393414 "
      "-20919623604389540188197773859359374498269402609449282996221204314912334"
      "5328234038901116544451103777729999222300000") {
    std::cerr << ("Operator * failed.\n");
  }

  std::ostringstream oss10;
  oss10 << a / b << ' ' << c / d << ' ' << e / f;
  if (oss10.str() != "-1000000000000000000000000000 1932576548 0") {
    std::cerr << ("Operator / failed.\n");
  }
  std::ostringstream oss11;
  oss11 << a % b << ' ' << c % d << ' ' << e % f;
  if (oss11.str() !=
      "0 101894444317458440603421824036688159663989325253819 "
      "-23534576554950000000000000009999990000999900000") {
  }

  std::cout << "Rational hard test started!" << std::endl;

  Rational r;
  r = 5;
  r += 3;
  r *= 7;
  b = 15;
  (r /= 8) -= b;
  if (-r != 8) std::cerr << ("Rational 1 failed.\n");

  Rational s, t;
  s = Rational(85) / 37, t = Rational(29) / BigInteger(-163);
  s += t;
  t = 1;
  for (int i = 0; i < 15; ++i) t *= s;
  if ((1 / t).toString() !=
      "507972178875842800075597772950831264898404875587494819951"
      "/39717526884730183825748150063721595142668632339785349901549568")
    std::cerr << ("Rational 2 failed.\n");
  s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11;  // 2^2×3×7×13×19×41×43×11
  t = -17 * 13 * 23 * 79;
  s *= s * s, t *= t * t;
  Rational q = s / t;
  if (q.toString() != "-29650611427828166204352/29472131485369")
    std::cerr << ("Rational 3 failed.\n");
  if (q / 1000000000 >= 1) std::cerr << ("Rational 4 failed.\n");
  if (0 / q != 0) std::cerr << ("Rational 5 failed.\n");
  q *= t / s;
  if (q != 1 || q.toString() != "1") std::cerr << ("Rational 6 failed.\n");
  s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11;
  t = s - 25;  // t=402365939
  s = 1000000007;
  s *= 1000000009;
  s *= 2147483647;
  if ((s / t).asDecimal(10) != "5337140829307966068.3989202202")
    std::cerr << ("Rational 7 failed.\n");
  t = -t;
  if ((t / s).asDecimal(25) != "-0.0000000000000000001873662")
    std::cerr << ("Rational 8 failed.\n");
}

void random_test() {
  std::random_device dev;
  std::mt19937 rnd(dev());
  std::uniform_int_distribution<std::mt19937::result_type> number(1,999'999);

  size_t count = 3000;

  std::cout << "Biginteger random test started!" << std::endl;
  for (size_t i = 0; i < count; ++i) {
    int64_t first = number(rnd);
    int64_t second = number(rnd);
    first *= (number(rnd) % 2 == 0 ? -1 : 1);
    second *= (number(rnd) % 2 == 0 ? -1 : 1);
    BigInteger a(first);
    BigInteger b(second);
    assert((first + second) == (a + b));
    assert((-first + second) == (-a + b));
    assert((first + (-second)) == (a + (-b)));
    assert((first - second) == (a - b));
    assert((-first - second) == (-a - b));
    assert((first < 0) == (a < 0));
    assert((first > 0) == (a > 0));
    assert((first >= 0) == (a >= 0));
    assert((first <= 0) == (a <= 0));
    assert((first > second) == (a > b));
    assert((first >= second) == (a >= b));
    assert((first == second) == (a == b));
    assert((first < second) == (a < b));
    assert((first <= second) == (a <= b));
    assert((first * second) == (a * b));
    assert((first / second) == (a / b));
    assert((first % second) == (a % b));
  }
  std::cout << "Rational random test started!" << std::endl;
  for (size_t i = 0; i < count; ++i) {
    int64_t first = number(rnd);
    int64_t second = number(rnd);
    int64_t third = number(rnd);
    int64_t fourth = number(rnd);
    first *= (number(rnd) % 2 == 0 ? -1 : 1);
    second *= (number(rnd) % 2 == 0 ? -1 : 1);
    third *= (number(rnd) % 2 == 0 ? -1 : 1);
    fourth *= (number(rnd) % 2 == 0 ? -1 : 1);
    Rational a(first, second);
    Rational b(third, fourth);
    if (second * fourth != 0) {
      assert((a - b) ==
          Rational(first * fourth - third * second, second * fourth));
      assert(a + b ==
          Rational(first * fourth + third * second, second * fourth));
      assert(a * b == Rational(first * third, second * fourth));
      if (second * third != 0) {
        assert(a / b == Rational(first * fourth, second * third));
      }
    }
  }
}

void long_divide_test() {
  std::random_device dev;
  std::mt19937 rnd(dev());
  std::uniform_int_distribution<std::mt19937::result_type> number(1,9); // distribution in range [1, 9]

  std::string first{};
  std::string second{};
  for (size_t i = 0; i < 30'000; ++i) {
    first += static_cast<char>('0' + number(rnd));
  }
  for (size_t j = 0; j < 5'000; ++j) {
    second += static_cast<char>('0' + number(rnd));
  }
  std::cout << "Biginteger long division test started!" << std::endl;
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  BigInteger(first) / BigInteger(second);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [µs]" << std::endl;
}

int main() {
  random_test();
  hard_test();
  long_divide_test();

  std::cout << "Finished testing!" << std::endl;
}