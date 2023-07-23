#include <iostream>
#include <vector>

class FenvickTree {
  using VecInt = std::vector<int32_t>;
  using VecVecInt = std::vector<VecInt>;
  using VecVecVecInt = std::vector<VecVecInt>;

  int32_t GetStartCube(int32_t x0, int32_t y0, int32_t z0) {
    if (x0 == 0 || y0 == 0 || z0 == 0) {
      return 0;
    }
    int32_t res = 0;
    for (int32_t x = x0; x > 0; x -= (x & -x)) {
      for (int32_t y = y0; y > 0; y -= (y & -y)) {
        for (int32_t z = z0; z > 0; z -= (z & -z)) {
          res += tree_[x][y][z];
        }
      }
    }
    return res;
  }

 public:
  explicit FenvickTree(int32_t size)
      : size_(size + 1),
        tree_(size + 1, VecVecInt(size + 1, VecInt(size + 1, 0))) {}

  void UpdStars(int32_t x0, int32_t y0, int32_t z0, int32_t delta) {
    for (int32_t x = x0; x < size_; x += (x & -x)) {
      for (int32_t y = y0; y < size_; y += (y & -y)) {
        for (int32_t z = z0; z < size_; z += (z & -z)) {
          tree_[x][y][z] += delta;
        }
      }
    }
  }

  int32_t GetStars(int32_t x1, int32_t x2, int32_t y1, int32_t y2, int32_t z1,
                   int32_t z2) {
    int32_t f_sum = GetStartCube(x2, y2, z2);
    f_sum -= GetStartCube(x2, y1, z2) + GetStartCube(x1, y2, z2) +
        GetStartCube(x2, y2, z1);
    f_sum += GetStartCube(x2, y1, z1) + GetStartCube(x1, y2, z1) +
        GetStartCube(x1, y1, z2);
    f_sum -= GetStartCube(x1, y1, z1);
    return f_sum;
  }

 private:
  uint8_t size_;
  VecVecVecInt tree_;
};

void UpdateRequest(FenvickTree& tree) {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t delta;
  std::cin >> x >> y >> z >> delta;
  tree.UpdStars(x + 1, y + 1, z + 1, delta);
}

int32_t GetRequest(FenvickTree& tree) {
  int32_t x1;
  int32_t y1;
  int32_t z1;
  int32_t x2;
  int32_t y2;
  int32_t z2;
  std::cin >> x1 >> y1 >> z1;
  std::cin >> x2 >> y2 >> z2;
  return tree.GetStars(x1, x2 + 1, y1, y2 + 1, z1, z2 + 1);
}

int main() {
  int32_t size;
  std::cin >> size;
  FenvickTree tree = FenvickTree(size);
  int32_t com;
  std::cin >> com;
  while (com != 3) {
    if (com == 1) {
      UpdateRequest(tree);
    } else {
      std::cout << GetRequest(tree) << "\n";
    }
    std::cin >> com;
  }
}