#include "Executable.h"

int main() {
  Treap treap = Treap();
  std::string request;
  while (std::cin >> request) {
    Executable::ExecuteOperation(treap, request);
  }
}