#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <iostream>
#include <optional>

class AVLTree {
  struct Node {
    Node(int64_t);
    void UpdateNode();
    static int64_t GetValue(Node*);
    static int64_t GetSum(Node*);
    char Balance();
    Node* LeftRotate();
    Node* RightRotate();

    int64_t value;
    int64_t sum;
    char height;
    Node* left;
    Node* right;
  };

 public:
  AVLTree();
  ~AVLTree();
  void Insert(int64_t);
  int64_t FindSum(size_t l, size_t r);

 private:
  int64_t SumBeforeInd(Node*, int64_t);
  Node* InsertNode(Node*, int64_t);
  void Clear(Node* node);

  Node* root_;
};

///////////////////////////Node/////////////////////////////////////
AVLTree::Node::Node(int64_t value)
    : value(value), sum(value), height(1), left(nullptr), right(nullptr) {}
void AVLTree::Node::UpdateNode() {
  if (left == nullptr) {
    height = (right == nullptr ? 0 : right->height) + 1;
    sum = value + (right == nullptr ? 0 : right->sum);
  } else if (right == nullptr) {
    height = left->height + 1;
    sum = value + left->sum;
  } else {
    height = std::max(right->height, left->height) + 1;
    sum = value + right->sum + left->sum;
  }
}
int64_t AVLTree::Node::GetValue(Node* node) {
  return (node == nullptr ? 0 : node->value);
}
int64_t AVLTree::Node::GetSum(Node* node) {
  return (node == nullptr ? 0 : node->sum);
}
char AVLTree::Node::Balance() {
  return (left != nullptr ? left->height : 0) -
         (right != nullptr ? right->height : 0);
};
AVLTree::Node* AVLTree::Node::LeftRotate() {
  Node* node = this->right;
  this->right = this->right->left;
  node->left = this;
  this->UpdateNode();
  node->UpdateNode();
  return node;
}
AVLTree::Node* AVLTree::Node::RightRotate() {
  Node* node = this->left;
  this->left = this->left->right;
  node->right = this;
  this->UpdateNode();
  node->UpdateNode();
  return node;
}

///////////////////////////AVLTree//////////////////////////////////
AVLTree::AVLTree() : root_(nullptr) {}
AVLTree::~AVLTree() { Clear(root_); }
void AVLTree::Insert(int64_t value) {
  root_ = InsertNode(root_, value);
  root_->UpdateNode();
}
int64_t AVLTree::FindSum(size_t l, size_t r) {
  int64_t s1 = SumBeforeInd(root_, r);
  int64_t s2 = SumBeforeInd(root_, l - 1);
  return s1 - s2;
}

int64_t AVLTree::SumBeforeInd(Node* node, int64_t ind) {
  if (node == nullptr) {
    return 0;
  }
  if (ind == Node::GetValue(node)) {
    return node->value + Node::GetSum(node->left);
  }
  if (ind < Node::GetValue(node)) {
    return SumBeforeInd(node->left, ind);
  }
  return Node::GetSum(node->left) + node->value +
      SumBeforeInd(node->right, ind);
}
AVLTree::Node* AVLTree::InsertNode(Node* node, int64_t value) {
  if (node == nullptr) {
    Node* x = new Node(value);
    return x;
  }
  if (value < node->value) {
    node->left = InsertNode(node->left, value);
    node->left->UpdateNode();
  } else if (value > node->value) {
    node->right = InsertNode(node->right, value);
    node->right->UpdateNode();
  } else {
    return node;
  }
  char balance = node->Balance();
  if (balance > 1) {
    if (value < node->left->value) {
      return node->RightRotate();
    }
    node->left = node->left->LeftRotate();
    return node->RightRotate();
  }
  if (balance < -1) {
    if (value > node->right->value) {
      return node->LeftRotate();
    }
    node->right = node->right->RightRotate();
    return node->LeftRotate();
  }
  return node;
}
void AVLTree::Clear(Node* node) {
  if (node == nullptr) {
    return;
  }
  Clear(node->left);
  Clear(node->right);
  delete node;
}

#endif //AVLTREE_H_
