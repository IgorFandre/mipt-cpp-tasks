#include "SplayTree.h"

int main() {
  SplayTree keys = SplayTree();
  SplayTree names = SplayTree();
  size_t size;
  std::cin >> size;
  std::vector<std::pair<std::string, std::string>> info(size, {"", ""});
  for (size_t i = 0; i < size; ++i) {
    std::string name;
    std::string key;
    std::cin >> key >> name;
    keys.Insert(key, i);
    names.Insert(name, i);
    info[i] = {name, key};
  }
  std::cin >> size;
  for (size_t i = 0; i < size; ++i) {
    std::string str;
    std::cin >> str;
    auto res = names.Find(str);
    if (!res.has_value()) {
      res = keys.Find(str);
      std::cout << info[res.value()].first << "\n";
    } else {
      std::cout << info[res.value()].second << "\n";
    }
  }
}
