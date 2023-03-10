#include "rational.h"

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
Rational::Rational(int64_t n) : numerator_(n), denominator_(1) {
  MakeSimple(true);
}
Rational::Rational(int64_t n1, int64_t n2) : numerator_(n1), denominator_(n2) {
  MakeSimple(true);
}
Rational::Rational(const BigInteger& b) : numerator_(b), denominator_(1) {
  MakeSimple(true);
}
Rational::Rational(const BigInteger& b1, const BigInteger& b2) : numerator_(b1), denominator_(b2) {
  MakeSimple(true);
}

Rational::Rational(const Rational& r) {
  numerator_ = r.numerator_;
  denominator_ = r.denominator_;
  sign_ = r.sign_;
}

Rational::operator double() {
  std::string s = asDecimal(20);
  return std::stod(s);
}

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

Rational& Rational::operator-=(Rational r) {
  return (*this += (-r));
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
  if (precision % BigInteger::GetBaseSymbols() != 0) {
    for (int i = static_cast<int>(precision) % BigInteger::GetBaseSymbols(); i > 0; --i) {
      multy *= 10;
    }
    numerator_ *= multy;
  }
  numerator_.AddZeros(precision / BigInteger::GetBaseSymbols());
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
  numerator_.DeleteZeros(precision / BigInteger::GetBaseSymbols());
  return s;
}

BigInteger Rational::GCD(BigInteger b1, BigInteger b2) {
  if (b1 < b2) { std::swap(b2, b1); }
  while(static_cast<bool>(b2)) {
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
  BigInteger res = GCD(numerator_, denominator_);
  numerator_ /= res;
  denominator_ /= res;
}
