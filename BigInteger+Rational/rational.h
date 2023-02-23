#ifndef RATIONAL_H_
#define RATIONAL_H_

#include "biginteger.h"

class Rational {
  using Sign = BigInteger::Sign;
  Sign sign_ = Sign::Zero;
  BigInteger numerator_;
  BigInteger denominator_;
  void MakeSimple(bool);
  static BigInteger GCD(BigInteger, BigInteger);
 public:
  Rational(): numerator_(0), denominator_(1) {}
  Rational(int64_t);
  Rational(int64_t, int64_t);
  Rational(const BigInteger&);
  Rational(const BigInteger&, const BigInteger&);
  Rational(const Rational&);

  Rational& operator=(const Rational&) = default;
  Rational& operator+=(const Rational&);
  Rational& operator-=(Rational);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);
  Rational& operator-();

  std::string toString() const;
  std::string asDecimal(std::size_t);

  explicit operator double ();
  friend bool operator<(const Rational &a, const Rational &b);
};

bool operator>(const Rational&, const Rational&);
bool operator<=(const Rational&, const Rational&);
bool operator>=(const Rational&, const Rational&);
bool operator!=(const Rational&, const Rational&);
bool operator==(const Rational&, const Rational&);

Rational operator+(Rational, const Rational&);
Rational operator-(Rational, const Rational&);
Rational operator*(Rational, const Rational&);
Rational operator/(Rational, const Rational&);

#endif //RATIONAL_H_