#ifndef SPLAYTREE_H_
#define SPLAYTREE_H_

#include <iostream>
#include <optional>
#include <string>
#include <vector>

class SplayTree {
  struct Node {
    Node(std::string&, size_t);
    std::string key;
    size_t index;
    Node* parent;
    Node* left;
    Node* right;
  };

 public:
  SplayTree();
  ~SplayTree();

  std::optional<size_t> Find(std::string&);
  void Insert(std::string&, size_t);

 private:
  void InsertNode(Node*, std::string&, size_t);
  Node* FindNode(Node*, std::string&);
  void Splay(Node*);
  static void Zig(Node*);
  static void ZigZig(Node*);
  static void ZigZag(Node*);
  void Clear(Node* node);
  Node* root_;
};

SplayTree::Node::Node(std::string& key, size_t i) {
this->key = key;
this->index = i;
parent = nullptr;
left = nullptr;
right = nullptr;
}

SplayTree::SplayTree() : root_(nullptr) {}
SplayTree::~SplayTree() { Clear(root_); }

std::optional<size_t> SplayTree::Find(std::string& key) {
  Node* res = FindNode(root_, key);
  if (res != nullptr) {
    Splay(res);
    return res->index;
  }
  return std::nullopt;
}

void SplayTree::Insert(std::string& key, size_t i) { InsertNode(root_, key, i); }

void SplayTree::InsertNode(Node* node, std::string& key, size_t i) {
  if (node == nullptr) {
    root_ = new Node(key, i);
    return;
  }
  if (node->key > key) {
    if (node->left == nullptr) {
      node->left = new Node(key, i);
      node->left->parent = node;
      Splay(node->left);
      return;
    }
    InsertNode(node->left, key, i);
  } else if (node->key < key) {
    if (node->right == nullptr) {
      node->right = new Node(key, i);
      node->right->parent = node;
      Splay(node->right);
      return;
    }
    InsertNode(node->right, key, i);
  } else {
    Splay(node);
    return;
  }
}

void SplayTree::Splay(Node* node) {
  if (node == nullptr) {
    return;
  }
  while (node->parent != nullptr) {
    Node* p = node->parent;
    Node* g = p->parent;
    if (g == nullptr) {
      Zig(node);
    } else if (g->left == p && p->left == node) {
      ZigZig(node);
    } else if (g->right == p && p->right == node) {
      ZigZig(node);
    } else {
      ZigZag(node);
    }
  }
  root_ = node;
}

SplayTree::Node* SplayTree::FindNode(Node* node, std::string& key) {
  if (node->key == key) {
    Splay(node);
    return root_;
  }
  if (key < node->key) {
    if (node->left == nullptr) {
      Splay(node);
      return nullptr;
    }
    return FindNode(node->left, key);
  }
  if (node->right == nullptr) {
    Splay(node);
    return nullptr;
  }
  return FindNode(node->right, key);
}

void SplayTree::Zig(Node* x) {
  Node* p = x->parent;
  if (p->left == x) {
    Node* a = x->right;
    x->parent = nullptr;
    x->right = p;
    p->parent = x;
    p->left = a;
    if (a != nullptr) {
      a->parent = p;
    }
    return;
  }
  Node* a = x->left;
  x->parent = nullptr;
  x->left = p;
  p->parent = x;
  p->right = a;
  if (a != nullptr) {
    a->parent = p;
  }
}

void SplayTree::ZigZig(Node* x) {
  Node* p = x->parent;
  Node* g = p->parent;
  if (p->left == x) {
    Node* b = x->right;
    Node* c = p->right;
    x->parent = g->parent;
    x->right = p;
    p->parent = x;
    p->left = b;
    p->right = g;
    g->parent = p;
    g->left = c;
    if (x->parent != nullptr) {
      if (x->parent->left == g) {
        x->parent->left = x;
      } else {
        x->parent->right = x;
      }
    }
    if (b != nullptr) {
      b->parent = p;
    }
    if (c != nullptr) {
      c->parent = g;
    }
  } else {
    Node* b = p->left;
    Node* c = x->left;
    x->parent = g->parent;
    x->left = p;
    p->parent = x;
    p->right = c;
    p->left = g;
    g->parent = p;
    g->right = b;
    if (x->parent != nullptr) {
      if (x->parent->left == g) {
        x->parent->left = x;
      } else {
        x->parent->right = x;
      }
    }
    if (b != nullptr) {
      b->parent = g;
    }
    if (c != nullptr) {
      c->parent = p;
    }
  }
}

void SplayTree::ZigZag(Node* x) {
  Node* p = x->parent;
  Node* g = p->parent;
  if (p->right == x) {
    Node* b = x->left;
    Node* c = x->right;
    x->parent = g->parent;
    x->left = p;
    x->right = g;
    p->parent = x;
    p->right = b;
    g->parent = x;
    g->left = c;
    if (x->parent != nullptr) {
      if (x->parent->left == g) {
        x->parent->left = x;
      } else {
        x->parent->right = x;
      }
    }
    if (b != nullptr) {
      b->parent = p;
    }
    if (c != nullptr) {
      c->parent = g;
    }
  } else {
    Node* b = x->left;
    Node* c = x->right;
    x->parent = g->parent;
    x->left = g;
    x->right = p;
    p->parent = x;
    p->left = c;
    g->parent = x;
    g->right = b;
    if (x->parent != nullptr) {
      if (x->parent->left == g) {
        x->parent->left = x;
      } else {
        x->parent->right = x;
      }
    }
    if (b != nullptr) {
      b->parent = g;
    }
    if (c != nullptr) {
      c->parent = p;
    }
  }
}

void SplayTree::Clear(Node* node) {
  if (node == nullptr) {
    return;
  }
  Clear(node->left);
  Clear(node->right);
  delete node;
}

#endif //SPLAYTREE_H_