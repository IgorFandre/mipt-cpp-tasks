#include "BTree.h"

int main() {
  int num_of_requests;
  std::cin >> num_of_requests;
  BTree tree = BTree(20);
  std::pair<bool, int64_t> mod = {false, 0};
  while (num_of_requests > 0) {
    char sign;
    int64_t num;
    std::cin >> sign >> num;
    if (sign == '?') {
      mod.first = true;
      auto res = tree.Next(num);
      mod.second = res.has_value() ? res.value() : -1;
      std::cout << mod.second << "\n";
    } else {
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
