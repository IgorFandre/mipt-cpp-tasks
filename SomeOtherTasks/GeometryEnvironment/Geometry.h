#ifndef CPPCONTEST__GEOMETRY_H_
#define CPPCONTEST__GEOMETRY_H_

#include <iostream>
#include <cmath>
#include <vector>

////////////////////////////////////////////
/////////////// POINT //////////////////////
////////////////////////////////////////////
struct Point {
 public:
  static constexpr double eps = 1e-7;
  double x;
  double y;

  Point();
  Point(const double& x, const double& y);

  Point operator+(const Point&) const;
  Point operator-(const Point&) const;
  Point operator*(double) const;
  bool operator==(const Point&) const;
  bool operator!=(const Point&) const;

  static void swap(Point, Point);

  friend std::ostream& operator<<(std::ostream&, const Point&);
  friend std::istream& operator>>(std::istream&, Point&);
};

////////////////////////////////////////////
///////////////// LINE /////////////////////
////////////////////////////////////////////
class Line {
 public:
  double a;
  double b;
  Line(double angle, double shift);
  Line(const Point& point, double angle);
  Line(const Point&, const Point&);

  Point intesect(const Line&) const;

  bool operator==(const Line&) const;
  bool operator!=(const Line&) const;
};

////////////////////////////////////////////
/////////////// VECTOR /////////////////////
////////////////////////////////////////////
struct Vector {
  double x = 0;
  double y = 0;
  Vector(const Point&, const Point&);
  Vector(double x, double y);

  double len() const;
  double vector_mult(const Vector&) const;
  double sc_mult(const Vector&) const;
  double area(const Vector&) const;

  Vector operator*(double);
  Vector& operator*=(double);
  bool operator==(const Vector&) const;
  void operator()(const Point&, const Point&);
};

////////////////////////////////////////////
/////////////// SHAPE //////////////////////
////////////////////////////////////////////
class Shape {
 public:
  virtual ~Shape() = default;
  virtual bool operator==(const Shape&) const = 0;
  virtual bool operator!=(const Shape&) const = 0;
  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual void rotate(const Point&, double) = 0;
  virtual void reflect(const Point&) = 0;
  virtual void reflect(const Line&) = 0;
  virtual void scale(const Point&, double) = 0;
  virtual bool isCongruentTo(const Shape&) const = 0;
  virtual bool isSimilarTo(const Shape&) const = 0;
  virtual bool containsPoint(const Point&) const = 0;
};

////////////////////////////////////////////
////////////// POLYGON /////////////////////
////////////////////////////////////////////
class Polygon : public Shape {
 protected:
  std::vector<Point> points_;
  double perimeter_;
  double area_;
  void set_perimeter();
  void set_area();
 public:
  Polygon();
  explicit Polygon(const std::vector<Point>&);
  template<class... Args>
  Polygon(Args... args);

  ~Polygon() override = default;
  size_t verticesCount() const;
  std::vector<Point> getVertices() const;
  bool isConvex() const;

  bool operator==(const Shape& other) const override;
  bool operator==(const Polygon& other) const;
  bool operator!=(const Shape& other) const override;
  bool operator!=(const Polygon& other) const;
  double perimeter() const override;
  double area() const override;
  void rotate(const Point&, double) override;
  void reflect(const Point&) override;
  void reflect(const Line&) override;
  void scale(const Point&, double) override;
  bool isCongruentTo(const Shape&) const override;
  bool isSimilarTo(const Shape& another) const override;
  bool containsPoint(const Point&) const override;
};

////////////////////////////////////////////
///////////// RECTANGLE ////////////////////
////////////////////////////////////////////
class Rectangle : public Polygon {
 public:
  Rectangle(const Point& first, const Point& second, double coef);
  Rectangle(const Point& first, const Point& second, int coef);
  explicit Rectangle(const std::vector<Point>&);

  template<class... Args>
  Rectangle(Args... args);

  Point center();
  std::pair<Line, Line> diagonals();
};

////////////////////////////////////////////
////////////// ELLIPSE /////////////////////
////////////////////////////////////////////
class Ellipse : public Shape {
 protected:
  Point focus_1_;
  Point focus_2_;
  double length_;
  double area_;
  double perimeter_;
 public:
  Ellipse(const Point&, const Point&, double);
  ~Ellipse() = default;

  bool operator==(const Shape&) const override;
  bool operator==(const Ellipse&) const;
  bool operator!=(const Shape&) const override;
  bool operator!=(const Ellipse&) const;

  double perimeter() const override;
  double area() const override;
  void rotate(const Point& p, double angle) override;
  void reflect(const Point&) override;
  void reflect(const Line&) override;
  void scale(const Point&, double koef);
  bool isCongruentTo(const Shape&) const override;
  bool isSimilarTo(const Shape&) const override;
  bool containsPoint(const Point&) const override;
  std::pair<Point, Point> focuses();
  double eccentricity();
  std::pair<Line, Line> directrices() const;
  Point center() const;
};

////////////////////////////////////////////
/////////////// CIRCLE /////////////////////
////////////////////////////////////////////
class Circle : public Ellipse {
 public:
  Circle(const Point&, double);
  ~Circle() = default;
  double radius();
};

////////////////////////////////////////////
/////////////// SQUARE /////////////////////
////////////////////////////////////////////
class Square : public Rectangle {
 public:
  Square(const Point&, const Point&);
  Circle circumscribedCircle();
  Circle inscribedCircle();
};

////////////////////////////////////////////
////////////// TRIANGLE ////////////////////
////////////////////////////////////////////
class Triangle : public Polygon {
 public:
  explicit Triangle(const std::vector<Point>&);

  template<class... Args>
  Triangle(Args... args);

  ~Triangle() = default;

  Point centerCircumscribedCircle();
  Circle circumscribedCircle();
  Circle inscribedCircle();
  Point centroid();
  Point orthocenter();
  Line EulerLine();
  Circle ninePointsCircle();
};

#endif //CPPCONTEST__GEOMETRY_H_