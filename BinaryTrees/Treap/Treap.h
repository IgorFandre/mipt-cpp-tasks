#ifndef TREAP_H_
#define TREAP_H_

#include <cstdlib>
#include <iostream>
#include <optional>

class Treap {
  struct Node {
    explicit Node(int);
    explicit Node(std::pair<int, int>);
    void Update();
    static int64_t GetCount(Node* node);

    int64_t count;
    int x;
    int y;
    Node* left;
    Node* right;
  };
 public:
  Treap();
  ~Treap();
  void Erase(int);
  void Insert(int);
  bool Find(int);
  std::optional<int> LowerBound(int);
  std::optional<int> UpperBound(int);
  std::optional<int> GetKth(int64_t);

 private:
  std::optional<int> GetKth(Node*, int64_t);
  std::optional<int> UpperBound(Node*, int);
  std::optional<int> LowerBound(Node*, int);
  Node* Erase(Node*, int);
  std::pair<Node*, Node*> Find(Node* parent, Node* node, int value);
  void Clear(Node*);
  Node* Merge(Node*, Node*);
  std::pair<Node*, Node*> Split(Node*, int);

  Node* root_;
};

///////////////////////////Node/////////////////////////////////////
Treap::Node::Node(int x)
    : count(1), x(x), y(rand()), left(nullptr), right(nullptr) {
}
Treap::Node::Node(std::pair<int, int> pair)
    : count(1), x(pair.first), y(pair.second), left(nullptr), right(nullptr) {
}
void Treap::Node::Update() {
  count = 1 + GetCount(left) + GetCount(right);
}
int64_t Treap::Node::GetCount(Node* node) {
  return (node == nullptr ? 0 : node->count);
}

///////////////////////////Treap////////////////////////////////////
Treap::Treap() : root_(nullptr){};
Treap::~Treap() { Clear(root_); }
void Treap::Erase(int value) { root_ = Erase(root_, value); }
void Treap::Insert(int key) {
  if (!Find(key)) {
    auto treaps = Split(root_, key);
    root_ = Merge(Merge(treaps.first, new Node(key)), treaps.second);
  }
}
bool Treap::Find(int key) { return Find(nullptr, root_, key).second != nullptr; }
std::optional<int> Treap::LowerBound(int value) { return LowerBound(root_, value); }
std::optional<int> Treap::UpperBound(int value) { return UpperBound(root_, value); }
std::optional<int> Treap::GetKth(int64_t k) { return GetKth(root_, k + 1); }

std::optional<int> Treap::GetKth(Node* node, int64_t k) {
  if (node == nullptr || Node::GetCount(node) < k || k < 0) {
    return std::nullopt;
  }
  if (k <= Node::GetCount(node->left)) {
    return GetKth(node->left, k);
  }
  if (k > Node::GetCount(node->left) + 1) {
    return GetKth(node->right, k - Node::GetCount(node->left) - 1);
  }
  return node->x;
}
std::optional<int> Treap::UpperBound(Node* node, int target) {
  if (node == nullptr) {
    return std::nullopt;
  }
  if (node->x >= target) {
    return UpperBound(node->left, target);
  }
  auto res = UpperBound(node->right, target);
  return res.has_value() ? res.value() : node->x;
}
std::optional<int> Treap::LowerBound(Node* node, int target) {
  if (node == nullptr) {
    return std::nullopt;
  }
  if (node->x <= target) {
    return LowerBound(node->right, target);
  }
  auto res = LowerBound(node->left, target);
  return res.has_value() ? res.value() : node->x;
}
Treap::Node* Treap::Erase(Node* node, int value) {
  if (node == nullptr) {
    return nullptr;
  }
  if (value < node->x) {
    node->left = Erase(node->left, value);
    node->Update();
    return node;
  }
  if (value > node->x) {
    node->right = Erase(node->right, value);
    node->Update();
    return node;
  }
  Node* l = node->left;
  Node* r = node->right;
  delete node;
  return Merge(l, r);
}
std::pair<Treap::Node*, Treap::Node*> Treap::Find(Node* parent, Node* node, int value) {
  if (node == nullptr) {
    return {parent, node};
  }
  if (node->x == value) {
    return {parent, node};
  }
  if (value < node->x) {
    return Find(node, node->left, value);
  }
  return Find(node, node->right, value);
}
void Treap::Clear(Node* node) {
  if (node == nullptr) {
    return;
  }
  Clear(node->right);
  Clear(node->left);
  delete node;
}
Treap::Node* Treap::Merge(Node* first, Node* second) {
  if (first == nullptr) {
    return second;
  }
  if (second == nullptr) {
    return first;
  }
  if (first->y > second->y) {
    first->right = Merge(first->right, second);
    first->Update();
    return first;
  }
  second->left = Merge(first, second->left);
  second->Update();
  return second;
}
std::pair<Treap::Node*, Treap::Node*> Treap::Split(Node* node, int value) {
  if (node == nullptr) {
    return {nullptr, nullptr};
  }
  if (value <= node->x) {
    std::pair<Node*, Node*> treaps = Split(node->left, value);
    node->left = treaps.second;
    node->Update();
    return {treaps.first, node};
  }
  std::pair<Node*, Node*> treaps = Split(node->right, value);
  node->right = treaps.first;
  node->Update();
  return {node, treaps.second};
}

#endif //TREAP_H_
