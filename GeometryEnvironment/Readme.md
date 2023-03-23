## Техническое задание

Geometry - это иерархия классов для работы с геометрическими фигурами на плоскости.

* Структура Point - точка на плоскости.
* Класс Line - прямая.
* Абстрактный класс Shape - фигура.
* Класс Polygon - многоугольник.
* Класс Ellipse - эллипс.
* Класс Circle - круг. 
* Класс Rectangle - прямоугольник.
* Класс Square - квадрат.
* Класс Triangle - треугольник.

У любой фигуры можно спросить:
* double perimeter() - периметр;
* double area() - площадь;
* bool operator==(const Shape& another)
* bool isCongruentTo(const Shape& another)
* bool isSimilarTo(const Shape& another)
* bool containsPoint(const Point& point)

**Фигуры не обязаны иметь одинаковый тип, чтобы считаться равными, конгруэнтными или подобными!**

С любой фигурой можно сделать:
* rotate(const Point& center, double angle) - поворот на угол (в градусах, против часовой стрелки) относительно точки;
* reflect(const Point& center) - симметрию относительно точки;
* reflect(const Line& axis) - симметрию относительно прямой;
* scale(const Point& center, double coefficient) - гомотетию с коэффициентом coefficient и центром center.