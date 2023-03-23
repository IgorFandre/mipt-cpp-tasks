Функциональность Deque:
* Конструкторы: по умолчанию, конструктор копирования, конструктор от int, конструктор от (int, const T&). Оператор присваивания.
* Метод size()
* Обращение по индексу: квадратными скобками (без проверок выхода за границу) и at (кидающее std::out_of_range).
* Методы push_back, pop_back, push_front, pop_front.
* Внутренний тип iterator. Этот тип поддерживает:
  * Инкремент, декремент
  * Сложение с целыми числами
  * Сравнение
  * Взятие разности двух итераторов
  * Разыменование, возвращающее T&
  * Оператор ->, возвращающее T*
* Константный итератор const_iterator.
* Операции push_back, push_front, pop_back и pop_front должны не инвалидировать указатели и ссылки на остальные элементы дека. Операции pop_back и pop_front, кроме того, должны еще и не инвалидировать итераторы. При этом хранить в итераторе указатель на сам объект deque нельзя: разыменование итератора должно требовать в большинстве случаев лишь одного разыменования указателя, изредка может быть двух, но никогда не трех.
* reverse-итераторы, а также методы rbegin, rend, crbegin, crend.
* Метод insert(iterator, const T&),
* Метод erase(iterator)