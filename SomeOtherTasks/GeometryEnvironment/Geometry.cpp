#include "Geometry.h"

////////////////////////////////////////////
/////////////// POINT //////////////////////
////////////////////////////////////////////

Point::Point() : x(0), y(0) {}
Point::Point(const double& x, const double& y) : x(x), y(y) {}

Point Point::operator+(const Point& p) const { return {x + p.x, y + p.y}; }
Point Point::operator-(const Point& p) const { return {x - p.x, y - p.y}; }
Point Point::operator*(double coef) const { return {x * coef, y * coef}; }
bool Point::operator==(const Point& p) const {
  bool x_pred = ((p.x >= x) ? p.x - x : x - p.x) < eps;
  bool y_pred = ((p.y >= y) ? p.y - y : y - p.y) < eps;
  return x_pred && y_pred;
}
bool Point::operator!=(const Point& p) const { return !(*this == p); }

void Point::swap(Point p1, Point p2) {
  std::swap(p1.x, p2.x);
  std::swap(p1.y, p2.y);
}

std::ostream& operator<<(std::ostream& out, const Point& point) {
  out << point.x << ' ' << point.y;
  return out;
}
std::istream& operator>>(std::istream& in, Point& point) {
  in >> point.x >> point.y;
  return in;
}

////////////////////////////////////////////
///////////////// LINE /////////////////////
////////////////////////////////////////////

Line::Line(double angle, double shift)
    : a(angle), b(shift) {}
Line::Line(const Point& point, double angle)
    : a(angle), b(point.y - a * point.x) {}
Line::Line(const Point& p1, const Point& p2)
    : a((p1.y - p2.y) / (p1.x - p2.x)), b(p1.y - a * p1.x) {
  if (std::isinf(a)) {
    b = 0;
  }
}
Point Line::intesect(const Line& line) const {
  double new_x = (b - line.b) / (line.a - a);
  double new_y = a * new_x + b;
  return {new_x, new_y};
}
bool Line::operator==(const Line& line) const {
  bool a_pred = ((line.a > a) ? line.a - a : a - line.a) < Point::eps;
  bool b_pred = ((line.b > b) ? line.b - b : b - line.b) < Point::eps;
  return a_pred && b_pred;
}
bool Line::operator!=(const Line& line) const { return !(*this == line); }

////////////////////////////////////////////
/////////////// VECTOR /////////////////////
////////////////////////////////////////////

Vector::Vector(const Point& p1, const Point& p2) : x{p2.x - p1.x}, y{p2.y - p1.y} {}
Vector::Vector(double x, double y) : x{x}, y{y} {}
double Vector::len() const { return sqrt(x * x + y * y); }
double Vector::vector_mult(const Vector& v) const { return x * v.y - y * v.x; }
double Vector::sc_mult(const Vector& v) const { return x * v.x + y * v.y; }
double Vector::area(const Vector& v) const {
  double area = x * v.y - y * v.x;
  return (area >= 0) ? area : -area;
}
Vector Vector::operator*(double koef) { return {x * koef, y * koef}; }
Vector& Vector::operator*=(double koef) {
  x /= koef;
  y /= koef;
  return *this;
}
bool Vector::operator==(const Vector& v) const { return Point(x, y) == Point(v.x, v.y); }
void Vector::operator()(const Point& p1, const Point& p2) {
  x = p2.x - p1.x;
  y = p2.y - p1.y;
}

////////////////////////////////////////////
////////////// POLYGON /////////////////////
////////////////////////////////////////////

Polygon::Polygon() : points_(), perimeter_(0), area_(0) {}
Polygon::Polygon(const std::vector<Point>& points) : points_{points}, perimeter_(0), area_(0) {
  set_perimeter();
  set_area();
}
template<class... Args>
Polygon::Polygon(Args... args) : points_(), perimeter_(0), area_(0) {
  static_assert(sizeof...(args) >= 3);
  (..., points_.push_back(args));
  set_perimeter();
  set_area();
}
void Polygon::set_perimeter() {
  Vector v{points_.back(), points_[0]};
  perimeter_ = v.len();
  for (size_t i = 1; i < points_.size(); ++i) {
    v(points_[i - 1], points_[i]);
    perimeter_ += v.len();
  }
}
void Polygon::set_area() {
  Vector prev{points_[0], points_[1]};
  Vector cur{points_[0], points_[2]};
  area_ = prev.area(cur);
  prev = cur;
  for (size_t i = 3; i < points_.size(); i++) {
    cur(points_[0], points_[i]);
    area_ += prev.area(cur);
    prev = cur;
  }
  area_ /= 2;
}
size_t Polygon::verticesCount() const { return points_.size(); };
std::vector<Point> Polygon::getVertices() const { return points_; };
bool Polygon::isConvex() const {
  Vector first{points_.back(), points_[0]};
  Vector second{points_[0], points_[1]};
  bool sign = (first.vector_mult(second) > 0);
  first = second;
  for (size_t i = 1; i < points_.size() - 1; ++i) {
    second = {points_[i], points_[i + 1]};
    if (sign != first.vector_mult(second) > 0) {
      return false;
    }
    first = second;
  }
  return true;
}

bool Polygon::operator==(const Shape& other) const {
  auto* poly = dynamic_cast<const Polygon*>(&other);
  if (poly == nullptr || points_.size() != poly->points_.size()) {
    return false;
  }
  return (*this == *poly);
}
bool Polygon::operator==(const Polygon& other) const {
  if (points_.size() != other.points_.size()) {
    return false;
  }
  bool found = false;
  size_t start = 0;
  for (size_t i = 0; i < points_.size(); ++i) {
    if (points_[i] == other.points_[0]) {
      start = i;
      found = true;
      break;
    }
  }
  if (!found) {
    return false;
  }
  for (size_t i = 0; i < points_.size(); ++i) {
    size_t idx = (i + start >= points_.size()) ? i + start - points_.size() : i + start;
    if (points_[idx] != other.points_[i]) {
      found = false;
      break;
    }
  }
  if (found) {
    return true;
  }
  for (size_t i = 1; i < points_.size(); ++i) {
    size_t idx = (start < i) ? start + points_.size() - i : start - i;
    if (points_[idx] != other.points_[i]) {
      return false;
    }
  }
  return true;
}
bool Polygon::operator!=(const Shape& other) const { return !(*this == other); }
bool Polygon::operator!=(const Polygon& other) const { return !(*this == other); }

double Polygon::perimeter() const { return perimeter_; }
double Polygon::area() const { return area_; }
void Polygon::rotate(const Point& p, double angle) {
  angle /= 180;
  angle *= M_PI;
  for (size_t i = 0; i < points_.size(); ++i) {
    double new_x = (points_[i].x - p.x) * cos(angle) - (points_[i].y - p.y) * sin(angle) + p.x;
    double new_y = (points_[i].x - p.x) * sin(angle) + (points_[i].y - p.y) * cos(angle) + p.y;
    points_[i].x = new_x;
    points_[i].y = new_y;
  }
}
void Polygon::reflect(const Point& p) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].x = 2 * p.x - points_[i].x;
    points_[i].y = 2 * p.y - points_[i].y;
  }
}
void Polygon::reflect(const Line& line) {
  Point line1{0, line.b};
  Vector s{1, line.a};
  double len = s.len();
  for (size_t i = 0; i < points_.size(); ++i) {
    Vector f{line1, points_[i]};
    double koef = 2 * f.vector_mult(s) / (len * sqrt(1 + line.a * line.a));
    points_[i].x += (-line.a) * koef;
    points_[i].y += koef;
  }
}
void Polygon::scale(const Point& p, double coef) {
  for (size_t i = 0; i < points_.size(); ++i) {
    points_[i].x = p.x + (points_[i].x - p.x) * coef;
    points_[i].y = p.y + (points_[i].y - p.y) * coef;
  }
  area_ *= (coef * coef);
  perimeter_ *= coef;
}
bool Polygon::isCongruentTo(const Shape& other) const {
  auto* poly = dynamic_cast<const Polygon*>(&other);
  if (poly == nullptr || points_.size() != poly->points_.size()) {
    return false;
  }
  std::vector<double> angles1(points_.size(), 0);
  std::vector<double> lengths1(points_.size(), 0);
  Vector first_vec{points_.back(), points_[0]};
  Vector cur_vec{points_[0], points_[1]};
  for (size_t i = 1; i < points_.size() - 1; ++i) {
    angles1[i - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
    lengths1[i - 1] = cur_vec.len();
    first_vec = cur_vec;
    cur_vec(points_[i], points_[i + 1]);
  }
  angles1[points_.size() - 2] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  lengths1[points_.size() - 2] = cur_vec.len();
  first_vec = cur_vec;
  cur_vec(points_.back(), points_[0]);
  angles1[points_.size() - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  lengths1[points_.size() - 1] = cur_vec.len();

  std::vector<double> angles2(points_.size(), 0);
  std::vector<double> lengths2(points_.size(), 0);
  first_vec(poly->points_.back(), poly->points_[0]);
  cur_vec(poly->points_[0], poly->points_[1]);
  for (size_t i = 1; i < points_.size() - 1; ++i) {
    angles2[i - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
    lengths2[i - 1] = cur_vec.len();
    first_vec = cur_vec;
    cur_vec(poly->points_[i], poly->points_[i + 1]);
  }
  angles2[points_.size() - 2] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  lengths2[points_.size() - 2] = cur_vec.len();
  first_vec = cur_vec;
  cur_vec(poly->points_.back(), poly->points_[0]);
  angles2[points_.size() - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  lengths2[points_.size() - 1] = cur_vec.len();

  for (size_t i = 0; i < points_.size(); ++i) {
    if (fabs(angles1[i] - angles2[0]) < Point::eps) {
      if (fabs(lengths1[i] - lengths2[0]) < Point::eps) {
        bool flag = false;
        for (size_t j = 1; j < points_.size(); ++j) {
          size_t idx = (i + j) >= points_.size() ? i + j - points_.size() : i + j;
          if (fabs(angles1[idx] - angles2[j]) > Point::eps || fabs(lengths1[idx] - lengths2[j]) > Point::eps) {
            flag = true;
            break;
          }
        }
        if (!flag) {
          return true;
        }
      } else if (fabs(lengths1[i] - lengths2.back()) < Point::eps) {
        bool flag = false;
        for (size_t j = 1; j < points_.size(); ++j) {
          size_t idx = (i + j) >= points_.size() ? i + j - points_.size() : i + j;
          if (fabs(angles1[idx] - angles2[points_.size() - j]) > Point::eps || fabs(lengths1[idx] - lengths2[points_.size() - 1 - j]) > Point::eps) {
            flag = true;
            break;
          }
        }
        if (!flag) {
          return true;
        }
      }
    }
  }
  return false;
}
bool Polygon::isSimilarTo(const Shape& other) const {
  auto* poly = dynamic_cast<const Polygon*>(&other);
  if (poly == nullptr || points_.size() != poly->points_.size()) {
    return false;
  }
  std::vector<double> angles1(points_.size(), 0);
  Vector first_vec{points_.back(), points_[0]};
  Vector cur_vec{points_[0], points_[1]};
  for (size_t i = 1; i < points_.size() - 1; ++i) {
    angles1[i - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
    first_vec = cur_vec;
    cur_vec(points_[i], points_[i + 1]);
  }
  angles1[points_.size() - 2] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  first_vec = cur_vec;
  cur_vec(points_.back(), points_[0]);
  angles1[points_.size() - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());

  std::vector<double> angles2(points_.size(), 0);
  first_vec(poly->points_.back(), poly->points_[0]);
  cur_vec(poly->points_[0], poly->points_[1]);
  for (size_t i = 1; i < points_.size() - 1; ++i) {
    angles2[i - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
    first_vec = cur_vec;
    cur_vec(poly->points_[i], poly->points_[i + 1]);
  }
  angles2[points_.size() - 2] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());
  first_vec = cur_vec;
  cur_vec(poly->points_.back(), poly->points_[0]);
  angles2[points_.size() - 1] = first_vec.vector_mult(cur_vec) / (first_vec.len() * cur_vec.len());

  for (size_t i = 0; i < points_.size(); ++i) {
    if (fabs(angles1[i] - angles2[0]) < Point::eps) {
      bool flag = false;
      for (size_t j = 0; j < points_.size(); ++j) {
        size_t idx = (j + i >= points_.size()) ? j + i - points_.size() : j + i;
        if (fabs(angles1[idx] - angles2[j]) > Point::eps) {
          flag = true;
          break;
        }
      }
      if (!flag) {
        return true;
      }
    } else if (fabs(angles1[i] + angles2[0]) < Point::eps) {
      bool flag = false;
      for (size_t j = 1; j < points_.size(); ++j) {
        size_t idx = (i < j) ? i + points_.size() - j : i - j;
        if (fabs(angles1[idx] + angles2[j]) > Point::eps) {
          flag = true;
          break;
        }
      }
      if (!flag) {
        return true;
      }
    }
  }
  return false;
}
bool Polygon::containsPoint(const Point& point) const {
  Vector f{point, points_.back()};
  Vector s{point, points_[0]};
  double angle = acos(f.sc_mult(s) / (f.len() * s.len())) * ((f.vector_mult(s) >= 0) ? -1 : 1);
  f = s;
  for (size_t i = 1; i < points_.size(); ++i) {
    s(point, points_[i]);
    angle += acos(f.sc_mult(s) / (f.len() * s.len())) * ((f.vector_mult(s) >= 0) ? -1 : 1);
    f = s;
  }
  if (fabs(angle) < Point::eps) {
    return false;
  }
  return true;
}

////////////////////////////////////////////
///////////// RECTANGLE ////////////////////
////////////////////////////////////////////

Rectangle::Rectangle(const Point& first, const Point& second, double coef) : Polygon() {
  points_.push_back(first);
  Vector diag{first, second};
  double cos = 1 / sqrt(coef * coef + 1);
  double sin = sqrt(1 - cos * cos);
  double new_x = (second.x - first.x) * cos - (second.y - first.y) * sin + first.x;
  double new_y = (second.x - first.x) * sin + (second.y - first.y) * cos + first.y;
  Vector v{first, Point(new_x, new_y)};
  double sc = v.len() / diag.len();
  if (coef < 1) {
    sc *= coef;
  }
  points_.emplace_back(first.x + v.x * sc, first.y + v.y * sc);
  points_.push_back(second);
  points_.emplace_back(second.x - v.x * sc, second.y - v.y * sc);
  set_perimeter();
  set_area();
}
Rectangle::Rectangle(const Point& first, const Point& second, int coef) : Rectangle(first, second, static_cast<double>(coef)) {}
Rectangle::Rectangle(const std::vector<Point>& points) : Polygon(points) {}

template<class... Args>
Rectangle::Rectangle(Args... args) : Polygon() {
  static_assert(sizeof...(args) >= 3);
  (..., points_.push_back(args));
  set_perimeter();
  set_area();
}

Point Rectangle::center() {
  return {0.5 * (points_[0].x + points_[2].x), 0.5 * (points_[0].y + points_[2].y)};
}
std::pair<Line, Line> Rectangle::diagonals() {
  return {Line(points_[0], points_[2]), Line(points_[1], points_[3])};
}

////////////////////////////////////////////
////////////// ELLIPSE /////////////////////
////////////////////////////////////////////

Ellipse::Ellipse(const Point& p1, const Point& p2, double len) : focus_1_(p1), focus_2_(p2), length_(len) {
  double c = Vector{p1, p2}.len() / 2;
  double a = length_ / 2;
  double b = sqrt((length_ * length_ / 4) - c * c);
  area_ = M_PI * a * b;
  perimeter_ = 4 * a * std::comp_ellint_2(c / a);
}

double Ellipse::perimeter() const { return perimeter_; }
double Ellipse::area() const { return area_; }
void Ellipse::rotate(const Point& p, double angle) {
angle /= 180;
angle *= M_PI;

double new_x = (focus_1_.x - p.x) * cos(angle) - (focus_1_.y - p.y) * sin(angle) + p.x;
double new_y = (focus_1_.x - p.x) * sin(angle) + (focus_1_.y - p.y) * cos(angle) + p.y;
focus_1_.x = new_x;
focus_1_.y = new_y;

new_x = (focus_2_.x - p.x) * cos(angle) - (focus_2_.y - p.y) * sin(angle) + p.x;
new_y = (focus_2_.x - p.x) * sin(angle) + (focus_2_.y - p.y) * cos(angle) + p.y;
focus_2_.x = new_x;
focus_2_.y = new_y;
}
void Ellipse::reflect(const Point& point) {
focus_1_.x = 2 * point.x - focus_1_.x;
focus_1_.y = 2 * point.y - focus_1_.y;

focus_2_.x = 2 * point.x - focus_2_.x;
focus_2_.y = 2 * point.y - focus_2_.y;
}
void Ellipse::reflect(const Line& line) {
Point line1{0, line.b};
Vector s{1, line.a};
double len = s.len();

Vector f{line1, focus_1_};
double koef = 2 * f.vector_mult(s) / (len * sqrt(1 + line.a * line.a));
focus_1_.x += (-line.a) * koef;
focus_1_.y += koef;

f(line1, focus_2_);
koef = 2 * f.vector_mult(s) / (len * sqrt(1 + line.a * line.a));
focus_2_.x += (-line.a) * koef;
focus_2_.y += koef;
}
void Ellipse::scale(const Point& point, double koef) {
focus_1_.x = point.x + (focus_1_.x - point.x) * koef;
focus_1_.y = point.y + (focus_1_.y - point.y) * koef;

focus_2_.x = point.x + (focus_2_.x - point.x) * koef;
focus_2_.y = point.y + (focus_2_.y - point.y) * koef;

length_ *= koef;
area_ *= (koef * koef);
perimeter_ *= koef;
}
bool Ellipse::isCongruentTo(const Shape& other) const {
auto* ell = dynamic_cast<const Ellipse*>(&other);
if (ell == nullptr || fabs(ell->length_ - length_) > Point::eps) {
return false;
}
if (fabs(Vector{focus_1_, focus_2_}.len() - Vector{ell->focus_1_, ell->focus_2_}.len()) < Point::eps) {
return true;
}
return false;
}
bool Ellipse::isSimilarTo(const Shape& other) const {
auto* ell = dynamic_cast<const Ellipse*>(&other);
if (ell == nullptr) {
return false;
}
double k = Vector{focus_1_, focus_2_}.len() / Vector{ell->focus_1_, ell->focus_2_}.len();
if (fabs(length_ / ell->length_ - k) < Point::eps) {
return true;
}
return false;
}
bool Ellipse::containsPoint(const Point& point) const {
if (Vector{focus_1_, point}.len() + Vector{focus_2_, point}.len() <= length_) {
return true;
}
return false;
}
std::pair<Point, Point> Ellipse::focuses() { return {focus_1_, focus_2_}; }
double Ellipse::eccentricity() { return Vector{focus_1_, focus_2_}.len() / length_; }
std::pair<Line, Line> Ellipse::directrices() const {
  Vector axis{focus_1_, focus_2_};
  axis *= 1 / axis.len();
  double a = length_ / 2;
  double c = axis.len() / 2;
  double d = a * a - c;
  Point cent = center();
  Line l1{Point(cent.x + axis.x * d, cent.y + axis.y * d), -axis.x / axis.y};
  Line l2{Point(cent.x - axis.x * d, cent.y - axis.y * d), -axis.x / axis.y};
  return {l1, l2};
}
Point Ellipse::center() const { return {0.5 * (focus_1_.x + focus_2_.x), 0.5 * (focus_1_.y + focus_2_.y)}; }

bool Ellipse::operator==(const Shape& other) const {
  auto* ell = dynamic_cast<const Ellipse*>(&other);
  if (ell == nullptr) {
    return false;
  }
  return *this == *ell;
}
bool Ellipse::operator==(const Ellipse& other) const {
  if (other.length_ != length_) {
    return false;
  }
  if (other.focus_1_ == focus_1_ && other.focus_2_ == focus_2_) {
    return true;
  }
  if (other.focus_1_ == focus_2_ && other.focus_2_ == focus_1_) {
    return true;
  }
  return false;
}
bool Ellipse::operator!=(const Shape& other) const { return !(*this == other); }
bool Ellipse::operator!=(const Ellipse& other) const { return !(*this == other); }

////////////////////////////////////////////
/////////////// CIRCLE /////////////////////
////////////////////////////////////////////

Circle::Circle(const Point& point, double rad) : Ellipse(point, point, 2 * rad) {}
double Circle::radius() { return length_ / 2; }

////////////////////////////////////////////
/////////////// SQUARE /////////////////////
////////////////////////////////////////////

Square::Square(const Point& first, const Point& second) : Rectangle(first, second, 1.0) {}
Circle Square::circumscribedCircle() { return {center(), Vector(points_[0], points_[2]).len() / 2}; }
Circle Square::inscribedCircle() { return {center(), Vector(points_[0], points_[1]).len() / 2}; }

////////////////////////////////////////////
////////////// TRIANGLE ////////////////////
////////////////////////////////////////////

Triangle::Triangle(const std::vector<Point>& points) : Polygon(points) {}

template<class... Args>
Triangle::Triangle(Args... args) : Polygon() {
  static_assert(sizeof...(args) >= 3);
  (..., points_.push_back(args));
  set_perimeter();
  set_area();
}

Point Triangle::centerCircumscribedCircle() {
  Vector v12{points_[1], points_[0]};
  Vector v23{points_[2], points_[1]};
  Vector v31{points_[0], points_[2]};
  double z = v12.x * v31.y - v12.y * v31.x;
  double z_1 = points_[0].x * points_[0].x + points_[0].y * points_[0].y;
  double z_2 = points_[1].x * points_[1].x + points_[1].y * points_[1].y;
  double z_3 = points_[2].x * points_[2].x + points_[2].y * points_[2].y;
  double z_x = z_3 * v12.y + z_1 * v23.y + z_2 * v31.y;
  double z_y = z_3 * v12.x + z_1 * v23.x + z_2 * v31.x;
  double new_x = -z_x / (2 * z);
  double new_y = z_y / (2 * z);
  return {new_x, new_y};
}
Circle Triangle::circumscribedCircle() {
  Point center = centerCircumscribedCircle();
  return {center, Vector{center, points_[0]}.len()};
}
Circle Triangle::inscribedCircle() {
  double l_1 = Vector{points_[1], points_[2]}.len();
  double l_2 = Vector{points_[0], points_[2]}.len();
  double l_3 = Vector{points_[0], points_[1]}.len();
  double new_y = (l_1 * points_[0].y + l_2 * points_[1].y + l_3 * points_[2].y) / (l_1 + l_2 + l_3);
  double new_x = (l_1 * points_[0].x + l_2 * points_[1].x + l_3 * points_[2].x) / (l_1 + l_2 + l_3);
  double rad = -(points_[1].x * points_[0].y + points_[0].x * new_y - points_[0].y * new_x - points_[1].x * new_y
      - points_[1].y * points_[0].x + points_[1].y * new_x) /
      sqrt(pow(points_[0].x - points_[1].x, 2) + pow(points_[0].y - points_[1].y, 2));
  return {Point{new_x, new_y}, rad};
}
Point Triangle::centroid() {
  double new_x = (points_[0].x + points_[1].x + points_[2].x) / 3;
  double new_y = (points_[0].y + points_[1].y + points_[2].y) / 3;
  return {new_x, new_y};
}
Point Triangle::orthocenter() {
  double a1 = points_[2].x - points_[1].x, b1 = points_[2].y - points_[1].y;
  double c1 = points_[0].x * (points_[2].x - points_[1].x) + points_[0].y * (points_[2].y - points_[1].y);
  double a2 = points_[2].x - points_[0].x, b2 = points_[2].y - points_[0].y;
  double c2 = points_[1].x * (points_[2].x - points_[0].x) + points_[1].y * (points_[2].y - points_[0].y);
  double den = a1 * b2 - a2 * b1;
  double num_x = c1 * b2 - c2 * b1;
  double num_y = a1 * c2 - a2 * c1;
  return {num_x / den, num_y / den};
}
Line Triangle::EulerLine() {
  return {centroid(), orthocenter()};
}
Circle Triangle::ninePointsCircle() {
  Circle circumscribed = circumscribedCircle();
  Point center = (orthocenter() + circumscribed.center()) * 0.5;
  return {center, circumscribed.radius() / 2};
}