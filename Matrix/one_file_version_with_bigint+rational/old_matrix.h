#ifndef CPPCONTEST__MATRIX_H_
#define CPPCONTEST__MATRIX_H_

#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class BigInteger {
  friend std::ostream& operator<<(std::ostream&, const BigInteger&);
 public:
  enum class Sign {
    Minus = -1,
    Zero = 0,
    Plus = 1,
  };
  Sign GetSign() const {
    return sign_;
  }

  BigInteger() : sign_{Sign::Zero} {}
  explicit BigInteger(std::string);
  BigInteger(int64_t);
  BigInteger(size_t, bool);
  BigInteger(size_t, char);
  BigInteger(const BigInteger&);
  BigInteger(const std::vector<int64_t>&, Sign);

  explicit operator int() const;
  explicit operator bool() const;
  explicit operator double() const;

  BigInteger operator-() const;

  BigInteger& operator=(BigInteger);
  friend bool operator<(const BigInteger&, const BigInteger&);

  BigInteger& operator+=(const BigInteger&);
  BigInteger& operator-=(const BigInteger&);
  BigInteger& operator*=(const BigInteger&);
  BigInteger& operator/=(const BigInteger&);
  BigInteger& operator%=(const BigInteger&);
  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger operator--(int);
  BigInteger& operator--();
  std::string toString() const;
  void Abs();
  void AddZeros(size_t);
  void DeleteZeros(size_t);

  int GetBaseSymbols() const { return kBaseSymbols; }
  int64_t GetBase() const { return kBase; }
 private:
  void ZeroCheck();
  bool AbsLess(const BigInteger&) const;
  void Clear();
  void Substract(const BigInteger&);
  void Add(const BigInteger&);
  void EraseLeadZeros();
  void Divide(BigInteger, bool);
  static constexpr int64_t kBase = 1e9;
  static constexpr int kBaseSymbols = 9;
  std::vector<int64_t> integer_;
  Sign sign_;
};

BigInteger operator*(BigInteger b1, const BigInteger& b2);
BigInteger operator+(BigInteger b1, const BigInteger& b2);
BigInteger operator/(BigInteger b1, const BigInteger& b2);
BigInteger operator-(BigInteger b1, const BigInteger& b2);
BigInteger operator%(BigInteger b1, const BigInteger& b2);

bool BigInteger::AbsLess(const BigInteger& b) const {
  if (sign_ == Sign::Zero) {
    if (b.sign_ == Sign::Zero) {
      return false;
    }
    return true;
  }
  if (integer_.size() > b.integer_.size()) {
    return false;
  }
  if (integer_.size() < b.integer_.size()) {
    return true;
  }
  for (size_t i = integer_.size(); i > 0; --i) {
    if (integer_[i - 1] < b.integer_[i - 1]) {
      return true;
    }
    if (integer_[i - 1] > b.integer_[i - 1]) {
      return false;
    }
  }
  return false;
}

bool operator<(const BigInteger& b1, const BigInteger& b2) {
  if (static_cast<int>(b1.GetSign()) < static_cast<int>(b2.GetSign())) {
    return true;
  }
  if (static_cast<int>(b1.GetSign()) > static_cast<int>(b2.GetSign())) {
    return false;
  }
  if (b1.GetSign() == BigInteger::Sign::Minus) {
    return b2.AbsLess(b1);
  }
  return b1.AbsLess(b2);
}
bool operator!=(const BigInteger& a, const BigInteger& b) {
  return (a < b) || (b < a);
}
bool operator==(const BigInteger& a, const BigInteger& b) {
  return !(a != b);
}
bool operator<=(const BigInteger& a, const BigInteger& b) {
  return !(b < a);
}
bool operator>(const BigInteger& a, const BigInteger& b) {
  return b < a;
}
bool operator>=(const BigInteger& a, const BigInteger& b) {
  return !(a < b);
}

void BigInteger::AddZeros(size_t nulls) {
  EraseLeadZeros();
  std::reverse(integer_.begin(), integer_.end());
  for (size_t i = 0; i < nulls; ++i) {
    integer_.push_back(0);
  }
  std::reverse(integer_.begin(), integer_.end());
}

void BigInteger::DeleteZeros(size_t nulls) {
  EraseLeadZeros();
  std::reverse(integer_.begin(), integer_.end());
  for (size_t i = 0; i < nulls; ++i) {
    integer_.pop_back();
  }
  std::reverse(integer_.begin(), integer_.end());
}

void BigInteger::Add(const BigInteger& b) {
  int64_t move = 0;
  std::vector<int64_t> new_int;
  size_t n = (integer_.size() > b.integer_.size()) ? integer_.size() : b.integer_.size();
  for (size_t i = 0; i < n; ++i) {
    if (i < integer_.size()) {
      move += integer_[i];
    }
    if (i < b.integer_.size()) {
      move += b.integer_[i];
    }
    new_int.push_back(move % kBase);
    move /= kBase;
  }
  if (move != 0) {
    new_int.push_back(move);
  }
  integer_ = new_int;
}

void BigInteger::Substract(const BigInteger& b) {
  int64_t move = 0;
  std::vector<int64_t> new_int;
  for (size_t i = 0; i < integer_.size(); ++i) {
    if (i < integer_.size()) {
      move += integer_[i];
    }
    if (i < b.integer_.size()) {
      move -= b.integer_[i];
    }
    if (move < 0) {
      new_int.push_back(move + kBase);
      move = -1;
    } else {
      new_int.push_back(move % kBase);
      move /= kBase;
    }
  }
  integer_ = new_int;
  EraseLeadZeros();
}

void BigInteger::Divide(BigInteger b, bool div = true) {
  Sign save_sign = sign_;
  if (b.sign_ == Sign::Minus) {
    b.sign_ = Sign::Plus;
  }
  BigInteger numerator = integer_[integer_.size() - 1];
  BigInteger result = 0;
  for (size_t i = integer_.size(); i > 0; --i) {
    result.integer_.insert(result.integer_.begin(), 0);
    if (numerator >= b) {
      int64_t ans;
      int64_t r = kBase - 1;
      int64_t l = 0;
      while (l <= r) {
        ans = (l + r) / 2;
        if (ans * b <= numerator) {
          if ((ans + 1) * b > numerator) {
            numerator -= ans * b;
            result += ans;
            break;
          }
          l = ans + 1;
        } else {
          r = ans - 1;
        }
      }
    }
    if (i > 1) {
      numerator.AddZeros(1);
      numerator += integer_[i - 2];
      numerator.EraseLeadZeros();
    }
  }
  if (div) {
    integer_ = result.integer_;
  } else {
    integer_ = numerator.integer_;
  }
  sign_ = save_sign;
  EraseLeadZeros();
}

void BigInteger::EraseLeadZeros() {
  while (!integer_.empty() && integer_.back() == 0) {
    integer_.pop_back();
  }
  if (integer_.empty()) {
    sign_ = Sign::Zero;
  }
}

void BigInteger::Abs() {
  sign_ = Sign(abs(int(sign_)));
}

void BigInteger::Clear() {
  sign_ = Sign::Zero;
  integer_.clear();
}

BigInteger::BigInteger(std::string str) : integer_() {
  if (str.empty()) {
    sign_ = Sign::Zero;
    return;
  }
  if (str[0] == '-') {
    sign_ = Sign::Minus;
    str.erase(str.begin());
  } else {
    sign_ = Sign::Plus;
  }
  int idx = static_cast<int>(str.size());
  while (idx - kBaseSymbols > 0) {
    integer_.push_back(stoi(str.substr(idx - kBaseSymbols, kBaseSymbols)));
    idx -= kBaseSymbols;
  }
  if (stoi(str.substr(0, idx)) != 0) {
    integer_.push_back(
        stoi(str.substr(0, idx)));
  }
  EraseLeadZeros();
}
BigInteger::BigInteger(int64_t n) {
  if (n < 0) {
    sign_ = Sign::Minus;
  } else if (n > 0) {
    sign_ = Sign::Plus;
  } else {
    sign_ = Sign::Zero;
  }
  if (n < 0) {
    n *= -1;
  }
  while (n != 0) {
    integer_.push_back(n % kBase);
    n /= kBase;
  }
}
BigInteger::BigInteger(size_t n, bool sign) {
  if (n == 0) {
    sign_ = Sign::Zero;
  } else if (sign) {
    sign_ = Sign::Plus;
  } else {
    sign_ = Sign::Minus;
  }
  do {
    integer_.push_back(static_cast<int64_t>(n) % kBase);
    n /= kBase;
  } while (n != 0);
}
BigInteger::BigInteger(size_t n, char sign) : integer_() {
  if (n == 0) {
    sign_ = Sign::Zero;
  } else if (sign == '+') {
    sign_ = Sign::Plus;
  } else {
    sign_ = Sign::Minus;
  }
  do {
    integer_.push_back(static_cast<int64_t>(n) % kBase);
    n /= kBase;
  } while (n != 0);
}
BigInteger::BigInteger(const BigInteger& b) {
  integer_ = b.integer_;
  sign_ = b.sign_;
}
BigInteger::BigInteger(const std::vector<int64_t>& numbers, Sign sign) {
  sign_ = sign;
  integer_ = numbers;
}
BigInteger::operator int() const {
  if (sign_ == Sign::Zero) {
    return 0;
  }
  int bigint = static_cast<int>(integer_[integer_.size() - 1]);
  for (size_t i = integer_.size() - 2; i > 0; --i) {
    bigint *= static_cast<int>(kBase);
    bigint += static_cast<int>(integer_[i]);
  }
  if (sign_ == Sign::Minus) {
    bigint *= -1;
  }
  return bigint;
}
BigInteger::operator double() const {
  if (sign_ == Sign::Zero) {
    return 0;
  }
  auto bigint = static_cast<double>(integer_[integer_.size() - 1]);
  for (size_t i = integer_.size() - 2; i > 0; --i) {
    bigint *= static_cast<double>(kBase);
    bigint += static_cast<double>(integer_[i]);
  }
  if (sign_ == Sign::Minus) {
    bigint *= -1;
  }
  return bigint;
}
BigInteger::operator bool() const {
  return static_cast<bool>(sign_);
}
BigInteger& BigInteger::operator=(BigInteger b) {
  integer_ = b.integer_;
  sign_ = b.sign_;
  return *this;
}
BigInteger& BigInteger::operator+=(const BigInteger& b) {
  if (b.sign_ == Sign::Zero) {
    return *this;
  }
  if (sign_ == Sign::Zero) {
    *this = b;
    return *this;
  }
  if (sign_ == b.sign_) {
    Add(b);
  } else {
    if (!AbsLess(b)) {
      Substract(b);
    } else {
      BigInteger new_this = b;
      new_this.Substract(*this);
      *this = new_this;
    }
  }
  return *this;
}
BigInteger& BigInteger::operator-=(const BigInteger& b) {
  *this += (-b);
  return *this;
}
BigInteger& BigInteger::operator*=(const BigInteger& b) {
  if (b.sign_ == Sign::Zero || sign_ == Sign::Zero) {
    Clear();
    return *this;
  }
  if (b.integer_.size() == 1 && b.integer_[0] == 1) {
    sign_ = Sign(static_cast<int>(sign_) * static_cast<int>(b.sign_));
    return *this;
  }
  BigInteger multiply = 0;
  size_t size = integer_.size() + b.integer_.size();
  for (size_t i = 0; i < size + 2; ++i) {
    multiply.integer_.push_back(0);
  }
  for (size_t i = 0; i < integer_.size(); ++i) {
    for (size_t j = 0; j < b.integer_.size(); ++j) {
      multiply.integer_[i + j] += (integer_[i] * b.integer_[j]) % kBase;
      multiply.integer_[i + j + 1] += (integer_[i] * b.integer_[j]) / kBase;
      if (multiply.integer_[i + j] >= kBase) {
        multiply.integer_[i + j + 1] += multiply.integer_[i + j] / kBase;
        multiply.integer_[i + j] = multiply.integer_[i + j] % kBase;
      }
      if (multiply.integer_[i + j + 1] >= kBase) {
        multiply.integer_[i + j + 2] += multiply.integer_[i + j + 1] / kBase;
        multiply.integer_[i + j + 1] = multiply.integer_[i + j + 1] % kBase;
      }
    }
  }
  integer_ = multiply.integer_;
  sign_ = Sign(static_cast<int>(sign_) * static_cast<int>(b.sign_));
  EraseLeadZeros();
  return *this;
}
BigInteger& BigInteger::operator/=(const BigInteger& b) {
  sign_ = Sign(static_cast<int>(sign_) * static_cast<int>(b.sign_));
  Divide(b, true);
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& b) {
  Divide(b, false);
  return *this;
}
BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;
  copy.sign_ = Sign(static_cast<int>(sign_) * (-1));
  return copy;
}
BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}
BigInteger BigInteger::operator++(int) {
  BigInteger copy = *this;
  *this += 1;
  return copy;
}
BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}
BigInteger BigInteger::operator--(int) {
  BigInteger copy = *this;
  *this -= 1;
  return copy;
}
BigInteger operator*(BigInteger b1, const BigInteger& b2) {
  return (b1 *= b2);
}
BigInteger operator+(BigInteger b1, const BigInteger& b2) {
  return (b1 += b2);
}
BigInteger operator/(BigInteger b1, const BigInteger& b2) {
  return (b1 /= b2);
}
BigInteger operator-(BigInteger b1, const BigInteger& b2) {
  return (b1 -= b2);
}
BigInteger operator%(BigInteger b1, const BigInteger& b2) {
  return (b1 %= b2);
}
std::string BigInteger::toString() const {
  std::string res;
  if (sign_ == BigInteger::Sign::Zero || integer_.empty()) {
    return "0";
  }
  if (sign_ == BigInteger::Sign::Minus) {
    res = "-";
  }
  res += std::to_string(integer_[integer_.size() - 1]);
  for (size_t i = integer_.size() - 1; i > 0; --i) {
    std::string num = std::to_string(integer_[i - 1]);
    res += std::string(kBaseSymbols - num.size(), '0') + num;
  }
  return res;
}
std::ostream& operator<<(std::ostream& out, const BigInteger& b) {
  out << b.toString();
  return out;
}

std::istream& operator>>(std::istream& in, BigInteger& b) {
  std::string s;
  in >> s;
  b = BigInteger(s);
  return in;
}
BigInteger operator ""_bi(unsigned long long number) {
  return {number, true};
}
BigInteger operator ""_bi(const char* str) {
  return BigInteger(std::string(str));
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class Rational {
  using Sign = BigInteger::Sign;
  Sign sign_ = Sign::Zero;
  BigInteger numerator_;
  BigInteger denominator_;
  void MakeSimple(bool);
  static BigInteger GCD(BigInteger, BigInteger);
 public:
  Rational() : numerator_(0), denominator_(1) {}
  Rational(int64_t, int64_t);
  Rational(const BigInteger&, const BigInteger&);
  Rational(const Rational&);

  Rational& operator=(const Rational&);
  Rational& operator+=(const Rational&);
  Rational& operator-=(const Rational&);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);
  Rational& operator-();

  std::string toString() const;
  std::string asDecimal(std::size_t);

  Rational abs() const { return {numerator_, denominator_}; }

  explicit operator double();
  friend bool operator<(const Rational& a, const Rational& b);
  friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
    return os << r.toString();
  }
  friend std::istream& operator>>(std::istream& in, Rational& r) {
    int number;
    in >> number;
    r = Rational{number, 1};
    return in;
  }
};

bool operator<(const Rational& a, const Rational& b) {
  if (static_cast<int>(a.sign_) < static_cast<int>(b.sign_)) {
    return true;
  }
  if (static_cast<int>(a.sign_) > static_cast<int>(b.sign_)) {
    return false;
  }
  if (a.sign_ == BigInteger::Sign::Zero) {
    return (static_cast<int>(b.sign_) > 0);
  }
  if (a.sign_ == BigInteger::Sign::Minus) {
    return a.numerator_ * b.denominator_ > b.numerator_ * a.denominator_;
  }
  return a.numerator_ * b.denominator_ < b.numerator_ * a.denominator_;
}
bool operator>(const Rational& a, const Rational& b) {
  return b < a;
}
bool operator<=(const Rational& a, const Rational& b) {
  return !(a > b);
}
bool operator>=(const Rational& a, const Rational& b) {
  return !(a < b);
}
bool operator!=(const Rational& a, const Rational& b) {
  return a < b || b < a;
}
bool operator==(const Rational& a, const Rational& b) {
  return !(a != b);
}

Rational::Rational(int64_t n1, int64_t n2 = 1) : numerator_(n1), denominator_(n2) {
  MakeSimple(true);
}

Rational::Rational(const BigInteger& b1, const BigInteger& b2 = 1) : numerator_(b1), denominator_(b2) {
  MakeSimple(true);
}

Rational::Rational(const Rational& r) = default;

Rational::operator double() {
  std::string s = asDecimal(8);
  return std::stod(s);
}

Rational& Rational::operator=(const Rational& r) = default;

Rational& Rational::operator-() {
  sign_ = Sign(static_cast<int>(sign_) * -1);
  return *this;
}

Rational& Rational::operator+=(const Rational& r) {
  numerator_ = static_cast<int>(sign_) * numerator_ * r.denominator_
      + static_cast<int>(r.sign_) * r.numerator_ * denominator_;
  denominator_ *= r.denominator_;
  MakeSimple(true);
  return *this;
}

Rational operator+(Rational r1, const Rational& r2) {
  return (r1 += r2);
}

Rational& Rational::operator-=(const Rational& r) {
  numerator_ = static_cast<int>(sign_) * numerator_ * r.denominator_
      - static_cast<int>(r.sign_) * r.numerator_ * denominator_;
  denominator_ *= r.denominator_;
  MakeSimple(true);
  return *this;
}

Rational operator-(Rational r1, const Rational& r2) {
  return (r1 -= r2);
}

Rational& Rational::operator*=(const Rational& r) {
  sign_ = Sign(static_cast<int>(sign_) * static_cast<int>(r.sign_));
  numerator_ *= r.numerator_;
  denominator_ *= r.denominator_;
  MakeSimple(false);
  return *this;
}

Rational operator*(Rational r1, const Rational& r2) {
  return (r1 *= r2);
}

Rational& Rational::operator/=(const Rational& r) {
  sign_ = Sign(static_cast<int>(sign_) * static_cast<int>(r.sign_));
  numerator_ *= r.denominator_;
  denominator_ *= r.numerator_;
  MakeSimple(false);
  return *this;
}

Rational operator/(Rational r1, const Rational& r2) {
  return (r1 /= r2);
}

std::string Rational::toString() const {
  std::string rational;
  if (sign_ == BigInteger::Sign::Minus) {
    rational = "-";
  }
  rational += numerator_.toString();
  if (denominator_ != BigInteger(1)) {
    rational += "/", rational += denominator_.toString();
  }
  return rational;
}

std::string Rational::asDecimal(std::size_t precision) {
  int64_t multy = 1;
  if (precision % numerator_.GetBaseSymbols() != 0) {
    for (int i = static_cast<int>(precision) % numerator_.GetBaseSymbols(); i > 0; --i) {
      multy *= 10;
    }
    numerator_ *= multy;
  }
  numerator_.AddZeros(precision / numerator_.GetBaseSymbols());
  std::string s = (numerator_ / denominator_).toString();
  if (s.size() > precision) {
    s = s.substr(0, s.size() - precision) + "." + s.substr(s.size() - precision, precision);
  } else {
    s = "0." + std::string(precision - s.size(), '0') + s;
  }
  if (sign_ == BigInteger::Sign::Minus) {
    s = "-" + s;
  }
  numerator_ /= multy;
  numerator_.DeleteZeros(precision / numerator_.GetBaseSymbols());
  return s;
}

BigInteger Rational::GCD(BigInteger b1, BigInteger b2) {
  b1.Abs();
  b2.Abs();
  if (b1 < b2) { std::swap(b2, b1); }
  while (static_cast<bool>(b2)) {
    b1 %= b2;
    std::swap(b2, b1);
  }
  return b1;
}

void Rational::MakeSimple(bool change_sign) {
  if (numerator_.GetSign() == BigInteger::Sign::Zero) {
    sign_ = BigInteger::Sign::Zero;
    denominator_ = 1;
    return;
  } else if (change_sign) {
    sign_ = Sign(static_cast<int>(numerator_.GetSign()) * static_cast<int>(denominator_.GetSign()));
  }
  numerator_.Abs();
  denominator_.Abs();
  if (denominator_ == 1) {
    return;
  }
  BigInteger res = GCD(numerator_, denominator_);
  if (res > 100000)
    std::cerr << res << std::endl;
  numerator_ /= res;
  denominator_ /= res;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

namespace Operators {

template<size_t Num, size_t Cur, bool Stop = (Cur * Cur > Num)>
struct SqrtSearch {
  static const size_t value = SqrtSearch<Num, Cur + 1, ((Cur + 1) * (Cur + 1) > Num)>::value;
};
template<size_t Num, size_t I>
struct SqrtSearch<Num, I, true> {
  static const size_t value = I - 1;
};
template<size_t Num>
struct Sqrt {
  static const size_t value = SqrtSearch<Num, 0>::value;
};

//////////////////////////////////////////////

template<size_t Num, size_t Cur, bool Stop = (Cur * Cur > Num)>
struct SimpleSearch {
  static const bool simple = (Num % Cur != 0) && SimpleSearch<Num, Cur + 1, ((Cur + 1) * (Cur + 1) > Num)>::simple;
};
template<size_t Num, size_t I>
struct SimpleSearch<Num, I, true> {
  static const bool simple = true;
};
template<size_t Num>
struct Simple {
  static const size_t simple = SimpleSearch<Num, 2>::simple;
};
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t N, bool Simple = Operators::Simple<N>::simple>
class Residue {
  friend std::ostream& operator<<(std::ostream& os, const Residue<N>& r) {
    os << r.number;
    return os;
  }
 public:
  Residue() = default;
  Residue(int num) {
    static_assert(static_cast<size_t>(0) != N);
    if (num < 0) {
      num += N * ((-num) / N + 1);
    }
    number = num % N;
  }
  operator int() const {
    return number;
  }
  Residue abs() const { return *this; }
  Residue& operator+=(const Residue<N>& r) {
    number += r.number;
    if (number >= N) {
      number %= N;
    }
    return *this;
  }
  Residue& operator-=(const Residue<N>& r) {
    (number += N) -= r.number;
    if (number >= N) {
      number %= N;
    }
    return *this;
  }
  Residue& operator/=(const Residue<N>& r) {
    static_assert(Simple);
    if (number >= N) {
      number %= N;
    }
    for (int64_t i = 1;; ++i) {
      if ((r.number * i) % N == number) {
        number = i;
        break;
      }
    }
    return *this;
  }
  Residue& operator*=(const Residue<N>& r) {
    number *= r.number;
    if (number >= N) {
      number %= N;
    }
    return *this;
  }

  friend bool operator>(const Residue<N>& r1, const Residue<N>& r2) {
    return r1.number > r2.number;
  }

  friend bool operator==(const Residue<N>& r1, const Residue<N>& r2) {
    return r1.number == r2.number;
  }

 private:
  size_t number;
};

template<size_t N>
bool operator>=(const Residue<N>& r1, const Residue<N>& r2) {
  return !(r2 > r1);
}

template<size_t N>
bool operator!=(const Residue<N>& r1, const Residue<N>& r2) {
  return !(r1 == r2);
}

template<size_t N>
Residue<N> operator+(Residue<N> r1, const Residue<N>& r2) {
  return r1 += r2;
}
template<size_t N>
Residue<N> operator-(Residue<N> r1, const Residue<N>& r2) {
  return r1 -= r2;
}
template<size_t N>
Residue<N> operator*(Residue<N> r1, const Residue<N>& r2) {
  return r1 *= r2;
}
template<size_t N>
Residue<N> operator/(Residue<N> r1, const Residue<N>& r2) {
  return r1 /= r2;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t M, size_t N, typename Field = Rational>
class Matrix {
 public:
  Matrix() : matrix() {}
  Matrix(const std::initializer_list<std::initializer_list<Field>>& list) {
    size_t i = 0;
    for (auto& row : list) {
      size_t k = 0;
      for (auto& field : row) {
        matrix[i][k] = field;
        ++k;
      }
      ++i;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////

  std::array<Field, N>& operator[](size_t idx) {
    return matrix[idx];
  }
  const std::array<Field, N>& operator[](size_t idx) const {
    return matrix[idx];
  }

  Matrix& operator+=(const Matrix<M, N, Field>& m) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        matrix[row][col] += m[row][col];
      }
    }
    return *this;
  }
  Matrix& operator-=(const Matrix<M, N, Field>& m) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        matrix[row][col] -= m[row][col];
      }
    }
    return *this;
  }

  Matrix& operator*=(const Field& f) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        matrix[row][col] *= f;
      }
    }
    return *this;
  }

  Matrix<M, M, Field>& operator*=(const Matrix<M, M, Field>& m) {
    std::array<std::array<Field, M>, M> new_matrix;
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < M; ++col) {
        new_matrix[row][col] = Field(0);
        for (size_t in = 0; in < N; ++in) {
          new_matrix[row][col] += matrix[row][in] * m.matrix[in][col];
        }
      }
    }
    matrix = new_matrix;
    return *this;
  }

  template<size_t K>
  friend Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& m1, const Matrix<N, K, Field>& m2) {
    Matrix<M, K, Field> new_matrix;
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < K; ++col) {
        for (size_t in = 0; in < N; ++in) {
          new_matrix[row][col] += m1[row][in] * m2[in][col];
        }
      }
    }
    return new_matrix;
  }

  Matrix& operator=(const Matrix<N, M, const Field>& m) {
    matrix = m.matrix;
    return *this;
  }

  friend bool operator==(const Matrix<M, N, Field>& m1, const Matrix<M, N, Field>& m2) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        if (m1.matrix[row][col] != m2.matrix[row][col]) {
          return false;
        }
      }
    }
    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////

  Field det() const {
    static_assert(N == M);
    Matrix copy = *this;
    Field det = 1;
    for (size_t cnt = 0; cnt < N; ++cnt) {
      Field max_el = copy.matrix[cnt][cnt].abs();
      size_t max_idx = cnt;
      for (size_t row = cnt + 1; row < N; ++row) {
        if (max_el < copy.matrix[row][cnt].abs()) {
          max_el = copy.matrix[row][cnt].abs();
          max_idx = row;
        }
      }
      if (max_el == Field(0)) {
        return 0;
      }
      if (cnt != max_idx) {
        det *= (-1);
        std::swap(copy.matrix[cnt], copy.matrix[max_idx]);
      }
      det *= copy.matrix[cnt][cnt];
      Field koef = 0;
      for (size_t row = cnt + 1; row < N; ++row) {
        koef = copy.matrix[row][cnt] / copy.matrix[cnt][cnt];
        for (size_t col = cnt; col < N; ++col) {
          copy.matrix[row][col] -= koef * copy.matrix[cnt][col];
        }
      }
    }
    return det;
  }

  size_t rank() const {
    Matrix m = *this;
    size_t row = 0;
    size_t col = 0;
    while (row < M && col < N) {
      size_t idx = row;
      while (idx < M) {
        if (m.matrix[idx][col] != Field(0)) {
          break;
        }
        ++idx;
      }
      if (M == idx) {
        ++col;
        continue;
      }
      if (idx != row) {
        for (size_t i = row; i < N; ++i) {
          std::swap(m.matrix[row][i], m.matrix[idx][i]);
        }
      }
      Field k = m.matrix[row][col];
      m.matrix[row][col] = Field(1);
      for (size_t i = col + 1; i < N; ++i) {
        m.matrix[row][i] /= k;
      }
      for (size_t other_row = row + 1; other_row < M; ++other_row) {
        if (!(m.matrix[other_row][col] == Field(0))) {
          Field koef = m.matrix[other_row][col] / m.matrix[col][col];
          for (size_t i = 0; i < N; ++i) {
            m.matrix[other_row][i] -= (m.matrix[row][i] * koef);
          }
        }
      }
      ++row;
      ++col;
    }
    return (col < row ? col : row);
  }

  Field trace() const {
    static_assert(N == M);
    Field result = 0;
    for (size_t col = 0; col < N; ++col) {
      result += matrix[col][col];
    }
    return result;
  }

  Matrix<N, M, Field> transposed() const {
    Matrix<N, M, Field> result;
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        result[col][row] = matrix[row][col];
      }
    }
    return result;
  }

  void invert() {
    static_assert(N == M);
    Matrix<M, N, Field> E;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (i == j) {
          E.matrix[i][j] = 1;
        } else {
          E.matrix[i][j] = 0;
        }
      }
    }
    GaussInvert(E);
  }

  Matrix inverted() const {
    Matrix result = *this;
    result.invert();
    return result;
  }

  ////////////////////////////////////////////////////////////////////////////////

  friend void Print(const Matrix<M, N, Field>& m) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        std::cout << m.matrix[row][col] << " ";
      }
      std::cout << "\n";
    }
  }

  friend std::istream& operator>>(std::istream& in, Matrix<M, N, Field>& m) {
    for (size_t row = 0; row < M; ++row) {
      for (size_t col = 0; col < N; ++col) {
        in >> m.matrix[row][col];
      }
    }
    return in;
  }

  std::array<Field, M> getRow(size_t idx) const {
    std::array<Field, M> arr;
    for (size_t i = 0; i < M; ++i) {
      arr[i] = matrix[idx][i];
    }
    return arr;
  }

  std::array<Field, N> getColumn(size_t idx) const {
    return matrix[idx];
  }

 private:
  void GaussInvert(Matrix<M, N, Field>&);
  std::array<std::array<Field, N>, M> matrix;
};

template<size_t M, size_t N, typename Field>
bool operator!=(const Matrix<M, N, Field>& m1, const Matrix<M, N, Field>& m2) {
  return !(m1 == m2);
}

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator+(Matrix<M, N, Field> m1, const Matrix<M, N, Field>& m2) {
  return m1 += m2;
}
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator-(Matrix<M, N, Field> m1, const Matrix<M, N, Field>& m2) {
  return m1 -= m2;
}

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, const Rational& r) {
  return m *= r;
}

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(const Rational& r, Matrix<M, N, Field> m) {
  return m *= r;
}

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, const Residue<K>& r) {
  return m *= r;
}

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(const Residue<K>& r, Matrix<M, N, Field> m) {
  return m *= r;
}

template<size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::GaussInvert(Matrix<M, N, Field>& E) {
  for (size_t cnt = 0; cnt < M; ++cnt) {
    for (size_t other_row = 0; other_row < M; ++other_row) {
      if (other_row == cnt) {
        continue;
      }
      Field koef = 0;
      if (!(matrix[other_row][cnt] == Field(0))) {
        koef = matrix[other_row][cnt] / matrix[cnt][cnt];
        for (size_t col = 0; col < M; ++col) {
          if (col >= cnt) {
            matrix[other_row][col] -= matrix[cnt][col] * koef;
          }
          E.matrix[other_row][col] -= E.matrix[cnt][col] * koef;
        }
      }
    }
  }
  for (size_t row = 0; row < N; ++row) {
    for (size_t col = 0; col < N; ++col) {
      E.matrix[row][col] /= matrix[row][row];
    }
  }
  *this = E;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t N, typename Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;

#endif //CPPCONTEST__MATRIX_H_