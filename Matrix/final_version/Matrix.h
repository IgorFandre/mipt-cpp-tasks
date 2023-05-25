#ifndef MATRIX_H_
#define MATRIX_H_

#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Operators///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

namespace Operators {

////////////////////////////////Find sqrt///////////////////////////////////////////
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

////////////////////////////////Check simple////////////////////////////////////////
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
////////////////////////////////Residue/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t N, bool Simple = Operators::Simple<N>::simple>
class Residue {
  friend std::ostream& operator<<(std::ostream& os, const Residue<N>& r);
  friend bool operator>(const Residue<N>& r1, const Residue<N>& r2);
  friend bool operator==(const Residue<N>& r1, const Residue<N>& r2);
 public:
  Residue() = default;
  Residue(int num);
  explicit operator int() const { return number; }
  Residue abs() const { return *this; }
  Residue& operator+=(const Residue<N>& r);
  Residue& operator-=(const Residue<N>& r);
  Residue& operator/=(const Residue<N>& r);
  Residue& operator*=(const Residue<N>& r);
 private:
  size_t number;
};

template<size_t N>
bool operator>=(const Residue<N>& r1, const Residue<N>& r2);
template<size_t N>
bool operator!=(const Residue<N>& r1, const Residue<N>& r2);
template<size_t N>
Residue<N> operator+(Residue<N> r1, const Residue<N>& r2);
template<size_t N>
Residue<N> operator-(Residue<N> r1, const Residue<N>& r2);
template<size_t N>
Residue<N> operator*(Residue<N> r1, const Residue<N>& r2);
template<size_t N>
Residue<N> operator/(Residue<N> r1, const Residue<N>& r2);

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Matrix//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t M, size_t N, typename Field = int>
class Matrix {
 public:
  Matrix();
  Matrix(const std::initializer_list<std::initializer_list<Field>>& list);

  std::array<Field, N>& operator[](size_t);
  const std::array<Field, N>& operator[](size_t) const;

  Matrix& operator=(const Matrix<M, N, const Field>&);
  Matrix& operator+=(const Matrix<M, N, Field>&);
  Matrix& operator-=(const Matrix<M, N, Field>&);
  Matrix& operator*=(const Field&);
  Matrix<M, M, Field>& operator*=(const Matrix<M, M, Field>&);

  template<size_t K>
  friend Matrix<M, K, Field> operator*(const Matrix<M, N, Field>&, const Matrix<N, K, Field>&);
  friend bool operator==(const Matrix<M, N, Field>&, const Matrix<M, N, Field>&);
  friend std::ostream& operator<<(std::ostream&, const Matrix<M, N, Field>&);
  friend std::istream& operator>>(std::istream&, Matrix<M, N, Field>&);

  Field det() const;
  size_t rank() const;
  Field trace() const;
  Matrix<N, M, Field> transposed() const;
  void invert();
  Matrix<M, N, Field> inverted() const;

  std::array<Field, M> getRow(size_t) const;
  std::array<Field, N> getColumn(size_t) const;

 private:
  void GaussInvert(Matrix<M, N, Field>&);
  std::array<std::array<Field, N>, M> matrix;
};

template<size_t M, size_t N, typename Field>
bool operator!=(const Matrix<M, N, Field>&, const Matrix<M, N, Field>&);

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator+(Matrix<M, N, Field>, const Matrix<M, N, Field>&);

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator-(Matrix<M, N, Field>, const Matrix<M, N, Field>&);

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field>, int64_t);

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(int64_t, Matrix<M, N, Field>);

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field>, const Residue<K>&);

template<size_t M, size_t N, size_t K, typename Field>
Matrix<M, N, Field> operator*(const Residue<K>&, Matrix<M, N, Field>);

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Square Matrix///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t N, typename Field = int64_t>
using SquareMatrix = Matrix<N, N, Field>;

#endif //MATRIX_H_
