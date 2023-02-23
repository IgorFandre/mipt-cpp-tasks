#include "AVLTree.h"

int main() {
  int num_of_requests;
  std::cin >> num_of_requests;
  AVLTree tree = AVLTree();
  std::pair<bool, int64_t> mod = {false, 0};
  while (num_of_requests > 0) {
    char sign;
    std::cin >> sign;
    if (sign == '?') {
      mod.first = true;
      int64_t l;
      int64_t r;
      std::cin >> l >> r;
      mod.second = tree.FindSum(l, r);
      std::cout << mod.second << "\n";
    } else {
      int64_t num;
      std::cin >> num;
      if (mod.first) {
        tree.Insert((num + mod.second) % 1000000000);
      } else {
        tree.Insert(num);
      }
      mod.first = false;
    }
    num_of_requests--;
  }
}