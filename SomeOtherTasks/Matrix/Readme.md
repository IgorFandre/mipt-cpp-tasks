## Техническое задание


1) Шаблонный класс Residue<size_t N>, реализующий концепцию конечного поля из N элементов. Реализованы:
   * Арифметические операции, кроме деления, а в случае простого N и деление тоже
   * Конструктор от int, явный каст от Residue к int
2) Шаблонный класс Matrix с тремя шаблонными параметрами: size_t M, size_t N, typename Field=int64_t. Матрицы поддерживают следующие операции:
   * Сложение, вычитание, операторы +=, -=.
   * Умножение на число типа Field.
   * Умножение двух матриц.
   * Метод det(), возвращающий определитель матрицы за O(N^3).
   * Метод transposed(), возвращающий транспонированную матрицу.
   * Метод rank(), возвращающий ранг матрицы.
   * Методы inverted() и invert() - вернуть обратную матрицу и обратить данную матрицу.
   * Метод trace() - вычислить след матрицы.
   * Методы getRow(unsigned) и getColumn(unsigned), возвращающие std::array из соответствующих значений.
   * К матрице дважды применим оператор [].

Квадратные матрицы размера N можно объявлять всего с одним обязательным шаблонным параметром: SquareMatrix<N>.