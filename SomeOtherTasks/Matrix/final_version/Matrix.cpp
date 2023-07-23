#include "Matrix.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Operators///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

namespace Operators {

template<size_t Num, size_t Cur, bool Stop>
struct SqrtSearch;

template<size_t Num>
struct Sqrt;

template<size_t Num, size_t Cur, bool Stop>
struct SimpleSearch;

template<size_t Num>
struct Simple;

}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Residue/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t N, bool Simple>
Residue<N, Simple>::Residue(int num) {
  static_assert(static_cast<size_t>(0) != N);
  if (num < 0) {
    num += N * ((-num) / N + 1);
  }
  number = num % N;
}

////////////////////////////////Residue operators///////////////////////////////////
template<size_t N, bool Simple>
Residue<N, Simple>& Residue<N, Simple>::operator+=(const Residue<N>& r) {
  number += r.number;
  if (number >= N) {
    number %= N;
  }
  return *this;
}
template<size_t N, bool Simple>
Residue<N, Simple>& Residue<N, Simple>::operator-=(const Residue<N>& r) {
  (number += N) -= r.number;
  if (number >= N) {
    number %= N;
  }
  return *this;
}
template<size_t N, bool Simple>
Residue<N, Simple>& Residue<N, Simple>::operator/=(const Residue<N>& r) {
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
template<size_t N, bool Simple>
Residue<N, Simple>& Residue<N, Simple>::operator*=(const Residue<N>& r) {
  number *= r.number;
  if (number >= N) {
    number %= N;
  }
  return *this;
}
template<size_t N>
bool operator>(const Residue<N>& r1, const Residue<N>& r2) {
  return r1.number > r2.number;
}
template<size_t N>
bool operator==(const Residue<N>& r1, const Residue<N>& r2) {
  return r1.number == r2.number;
}
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
template<size_t N>
std::ostream& operator<<(std::ostream& os, const Residue<N>& r) {
  os << r.number;
  return os;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Matrix//////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix() : matrix() {}

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>::Matrix(const std::initializer_list<std::initializer_list<Field>>& list) {
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

////////////////////////////////Matrix functions////////////////////////////////////
template<size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::det() const {
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

template<size_t M, size_t N, typename Field>
size_t Matrix<M, N, Field>::rank() const {
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
      if (m.matrix[other_row][col] != Field(0)) {
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

template<size_t M, size_t N, typename Field>
Field Matrix<M, N, Field>::trace() const {
  static_assert(N == M);
  Field result = 0;
  for (size_t col = 0; col < N; ++col) {
    result += matrix[col][col];
  }
  return result;
}

template<size_t M, size_t N, typename Field>
Matrix<N, M, Field> Matrix<M, N, Field>::transposed() const {
  Matrix<N, M, Field> result;
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      result[col][row] = matrix[row][col];
    }
  }
  return result;
}

template<size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::invert() {
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

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> Matrix<M, N, Field>::inverted() const {
  Matrix<M, N, Field> result = *this;
  result.invert();
  return result;
}

template<size_t M, size_t N, typename Field>
void Matrix<M, N, Field>::GaussInvert(Matrix<M, N, Field>& E) {
  for (size_t cnt = 0; cnt < M; ++cnt) {
    for (size_t other_row = 0; other_row < M; ++other_row) {
      if (other_row == cnt) {
        continue;
      }
      Field koef = 0;
      if (matrix[other_row][cnt] != Field(0)) {
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

////////////////////////////////Matrix operators////////////////////////////////////
template<size_t M, size_t N, typename Field>
std::array<Field, N>& Matrix<M, N, Field>::operator[](size_t idx) {
  return matrix[idx];
}
template<size_t M, size_t N, typename Field>
const std::array<Field, N>& Matrix<M, N, Field>::operator[](size_t idx) const {
  return matrix[idx];
}
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator=(const Matrix<M, N, const Field>& m) {
  matrix = m.matrix;
  return *this;
}

template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator+=(const Matrix<M, N, Field>& m) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      matrix[row][col] += m[row][col];
    }
  }
  return *this;
}
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator-=(const Matrix<M, N, Field>& m) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      matrix[row][col] -= m[row][col];
    }
  }
  return *this;
}
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field>& Matrix<M, N, Field>::operator*=(const Field& f) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      matrix[row][col] *= f;
    }
  }
  return *this;
}
template<size_t M, size_t N, typename Field>
Matrix<M, M, Field>& Matrix<M, N, Field>::operator*=(const Matrix<M, M, Field>& m) {
  std::array<std::array<Field, M>, M> new_matrix;
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < M; ++col) {
      new_matrix[row][col] = Field(0);
      for (size_t in = 0; in < M; ++in) {
        new_matrix[row][col] += matrix[row][in] * m.matrix[in][col];
      }
    }
  }
  matrix = new_matrix;
  return *this;
}
template<size_t M, size_t N, typename Field, size_t K>
Matrix<M, K, Field> operator*(const Matrix<M, N, Field>& m1, const Matrix<N, K, Field>& m2) {
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
template<size_t M, size_t N, typename Field>
bool operator==(const Matrix<M, N, Field>& m1, const Matrix<M, N, Field>& m2) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      if (m1.matrix[row][col] != m2.matrix[row][col]) {
        return false;
      }
    }
  }
  return true;
}
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
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> m, int64_t r) {
  return m *= r;
}
template<size_t M, size_t N, typename Field>
Matrix<M, N, Field> operator*(int64_t r, Matrix<M, N, Field> m) {
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
std::ostream& operator<<(std::ostream& out, const Matrix<M, N, Field>& m) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      out << m.matrix[row][col] << " ";
    }
    out << "\n";
  }
}
template<size_t M, size_t N, typename Field>
std::istream& operator>>(std::istream& in, Matrix<M, N, Field>& m) {
  for (size_t row = 0; row < M; ++row) {
    for (size_t col = 0; col < N; ++col) {
      in >> m.matrix[row][col];
    }
  }
  return in;
}

////////////////////////////////Matrix getters//////////////////////////////////////
template<size_t M, size_t N, typename Field>
std::array<Field, M> Matrix<M, N, Field>::getRow(size_t idx) const {
  std::array<Field, M> arr;
  for (size_t i = 0; i < M; ++i) {
    arr[i] = matrix[idx][i];
  }
  return arr;
}
template<size_t M, size_t N, typename Field>
std::array<Field, N> Matrix<M, N, Field>::getColumn(size_t idx) const {
  return matrix[idx];
}