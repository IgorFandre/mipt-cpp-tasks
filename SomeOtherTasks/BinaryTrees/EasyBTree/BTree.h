#ifndef BTREE_H_
#define BTREE_H_

#include <deque>
#include <iostream>
#include <optional>

class BTree {
  struct Node {
    Node();
    Node(int64_t);
    Node* parent;
    std::deque<Node*> child;
    std::deque<int64_t> values;
  };

 public:
  BTree(size_t);
  ~BTree();
  void Insert(int64_t);
  std::optional<int64_t> Next(int64_t);

 private:
  std::optional<int64_t> Next(Node*, int64_t);
  void Insert(Node*, std::optional<size_t>, Node*, int64_t);
  Node* Split(Node*, size_t);
  void Clear(Node*);

  Node* root_;
  size_t rang_;
};

///////////////////////////Node/////////////////////////////////////
BTree::Node::Node() : parent(nullptr) {}
BTree::Node::Node(int64_t value) : parent(nullptr) {
  values.push_back(value);
  child.push_back(nullptr);
  child.push_back(nullptr);
}

///////////////////////////BTree////////////////////////////////////
BTree::BTree(size_t rang) : root_(nullptr), rang_(rang) {}
BTree::~BTree() { Clear(root_); }
void BTree::Insert(int64_t value) { Insert(nullptr, std::nullopt, root_, value); }
std::optional<int64_t> BTree::Next(int64_t value) { return Next(root_, value); }

std::optional<int64_t> BTree::Next(Node* node, int64_t value) {
  if (node == nullptr) {
    return std::nullopt;
  }
  size_t i;
  for (i = 0; i < node->values.size() && node->values[i] <= value; ++i) {
    if (node->values[i] == value) {
      return value;
    }
  }
  if (i == node->values.size()) {
    if (node->child.size() <= i || node->child[i] == nullptr) {
      return std::nullopt;
    }
    return Next(node->child[i], value);
  }
  if (node->child[i] != nullptr) {
    auto res = Next(node->child[i], value);
    return res.has_value() ? res.value() : node->values[i];
  }
  return node->values[i];
}
void BTree::Insert(Node* parent, std::optional<size_t> par_i, Node* node,
            int64_t value) {
  if (node == nullptr) {
    node = new Node(value);
    if (node->parent == nullptr) {
      root_ = node;
      return;
    }
    node->parent = parent;
    parent->child[par_i.value()] = node;
    return;
  }
  if (node->values.size() >= 2 * rang_ - 1) {
    node = Split(node, rang_ - 1);
  }
  auto it_vals = node->values.begin();
  auto it_ch = node->child.begin();
  size_t i = 0;
  while (*it_vals < value && it_vals != node->values.end()) {
    ++it_vals;
    ++it_ch;
    ++i;
  }
  if (*it_vals == value) {
    return;
  }
  if (*it_ch == nullptr) {
    node->values.insert(it_vals, value);
    node->child.insert(it_ch, nullptr);
    return;
  }
  return Insert(node, i, *it_ch, value);
}

BTree::Node* BTree::Split(Node* node, size_t i_root) {
  if (node == nullptr) {
    return nullptr;
  }
  Node* new_node = new Node();
  for (size_t i = 0; i < i_root; ++i) {
    new_node->values.push_back(node->values.front());
    node->values.pop_front();
    new_node->child.push_back(node->child.front());
    if (node->child.front() != nullptr) {
      node->child.front()->parent = new_node;
    }
    node->child.pop_front();
  }
  new_node->child.push_back(node->child.front());
  if (node->child.front() != nullptr) {
    node->child.front()->parent = new_node;
  }
  node->child.pop_front();

  int64_t local_root = node->values.front();
  node->values.pop_front();
  if (node->parent != nullptr) {
    auto it_val = node->parent->values.begin();
    auto it_ch = node->parent->child.begin();
    while (*it_ch != node) {
      ++it_val;
      ++it_ch;
    }
    node->parent->values.insert(it_val, local_root);
    node->parent->child.insert(it_ch, new_node);
    new_node->parent = node->parent;
    return node->parent;
  }
  root_ = new Node(local_root);
  root_->child[0] = new_node;
  new_node->parent = root_;
  root_->child[1] = node;
  node->parent = root_;
  return root_;
}

void BTree::Clear(Node* node) {
  if (node == nullptr) {
    return;
  }
  for (size_t i = 0; i < node->child.size(); ++i) {
    Clear(node->child[i]);
  }
  delete node;
}

#endif //BTREE_H_
