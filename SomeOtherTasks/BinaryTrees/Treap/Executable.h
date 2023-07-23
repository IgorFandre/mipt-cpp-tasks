#ifndef TREAP_EXECUTABLE_H_
#define TREAP_EXECUTABLE_H_

#include "Treap.h"

class Executable {
 public:
  static void ExecuteOperation(Treap& treap, std::string& oper) {
    if (oper == "insert") {
      Insert(treap);
    }
    if (oper == "delete") {
      Delete(treap);
    }
    if (oper == "exists") {
      if (Exists(treap)) {
        std::cout << "true\n";
      } else {
        std::cout << "false\n";
      }
    }
    if (oper == "next") {
      auto res = Next(treap);
      if (res.has_value()) {
        std::cout << res.value() << "\n";
      } else {
        std::cout << "none\n";
      }
    }
    if (oper == "prev") {
      auto res = Prev(treap);
      if (res.has_value()) {
        std::cout << res.value() << "\n";
      } else {
        std::cout << "none\n";
      }
    }
    if (oper == "kth") {
      auto res = Kth(treap);
      if (res.has_value()) {
        std::cout << res.value() << "\n";
      } else {
        std::cout << "none\n";
      }
    }
  }

 private:
  static void Insert(Treap& treap) {
    int val;
    std::cin >> val;
    treap.Insert(val);
  }
  static void Delete(Treap& treap) {
    int val;
    std::cin >> val;
    treap.Erase(val);
  }
  static bool Exists(Treap& treap) {
    int val;
    std::cin >> val;
    return treap.Find(val);
  }
  static std::optional<int> Next(Treap& treap) {
    int val;
    std::cin >> val;
    return treap.LowerBound(val);
  }
  static std::optional<int> Prev(Treap& treap) {
    int val;
    std::cin >> val;
    return treap.UpperBound(val);
  }
  static std::optional<int> Kth(Treap& treap) {
    int64_t k;
    std::cin >> k;
    return treap.GetKth(k);
  }
};

#endif //TREAP_EXECUTABLE_H_
