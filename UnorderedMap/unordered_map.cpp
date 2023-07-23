#pragma once
#include <cassert>
#include <iostream>
#include <tuple>
#include <vector>

template <class Key,
          class Value,
          class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key>,
          class Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
  private:
    struct Node;

  public:
    using NodeType = std::pair<const Key, Value>;
    using value_type = NodeType;
    using pointer = value_type*;
    using size_type = size_t;

  private:
    using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using NodeAllocTraits = typename std::allocator_traits<Alloc>::template rebind_traits<Node>;

    using NodePtrAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node*>;

    using NodeTypeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType>;
    using NodeTypeAllocTraits = typename std::allocator_traits<Alloc>::template rebind_traits<NodeType>;

    struct Node {
        pointer value;
        size_type hash;
        Node* next;
        Node()
            : value(nullptr), hash(0), next(nullptr) {}
        Node(pointer value, size_type hash, Node* next = nullptr)
            : value(value), hash(hash), next(next) {}
        Node(const Node&) = default;
        Node(Node&& node) noexcept
            : value(node.value), hash(node.hash), next(node.next) {
            node.value = nullptr;
            node.hash = 0;
            node.next = nullptr;
        }
        ~Node() = default;
    };
    Node* AllocateNewBase() {
        Node* node = NodeAllocTraits::allocate(node_alloc_, 1);
        try_construct_node(node);
        return node;
    }
    void ChangeBase(Node* new_base) noexcept {
        if (base_ != nullptr) {
            eliminate_node(base_);
        }
        base_ = new_base;
    }
    void ClearNode(Node* node) noexcept {
        if (node != nullptr) {
            if (node->value != nullptr) {
                eliminate_nodetype(node->value);
            }
            eliminate_node(node);
        }
    }

  public:
    template <bool IsConst>
    class CommonIterator {
        friend UnorderedMap;

      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = UnorderedMap::value_type;
        using reference = std::conditional_t<IsConst, const value_type&, value_type&>;
        using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
        using difference_type = int64_t;

        CommonIterator() = default;
        CommonIterator(const CommonIterator& it) = default;
        CommonIterator(CommonIterator&& it) noexcept = default;
        CommonIterator& operator=(const CommonIterator& it) & = default;

        reference operator*() const {
            return *(iter_->value);
        }

        pointer operator->() const {
            return (iter_->value);
        }

        CommonIterator& operator++() {
            iter_ = iter_->next;
            return *this;
        }

        CommonIterator operator++(int) & {
            CommonIterator copy = *this;
            iter_ = iter_->next;
            return copy;
        }

        operator CommonIterator<true>() noexcept {
            return CommonIterator<true>(iter_);
        }

        bool operator==(const CommonIterator& it) const {
            return iter_ == it.iter_;
        }

        bool operator!=(const CommonIterator& it) const {
            return iter_ != it.iter_;
        }

      private:
        CommonIterator(Node* node)
            : iter_(node) {}
        Node* iter_;
    };

    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept {
        return iterator(base_->next);
    }
    const_iterator begin() const noexcept {
        return const_iterator(base_->next);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(base_->next);
    }
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(nullptr);
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(nullptr);
    }
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(nullptr);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }
    const_iterator end() const noexcept {
        return const_iterator(nullptr);
    }
    const_iterator cend() const noexcept {
        return const_iterator(nullptr);
    }
    reverse_iterator rend() noexcept {
        return reverse_iterator(base_->next);
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(base_->next);
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(base_->next);
    }

    UnorderedMap()
        : UnorderedMap(default_hash_size_) {}
    explicit UnorderedMap(const Alloc& alloc)
        : UnorderedMap(default_hash_size_, Hash(), Equal(), alloc) {}
    explicit UnorderedMap(size_t hash_size,
                          const Hash& hash = Hash(),
                          const Equal& equal = Equal(),
                          const Alloc& alloc = Alloc())
        : hash_size_(hash_size), max_load_factor_(1.0), size_(0), base_(nullptr), hash_(hash), equal_(equal), alloc_(alloc), node_alloc_(alloc_) {
        base_ = AllocateNewBase();
        map_.resize(hash_size, nullptr);
    }

    UnorderedMap(const UnorderedMap& map)
        : hash_size_(map.hash_size_), max_load_factor_(map.max_load_factor_), size_(0), base_(nullptr), hash_(map.hash_), equal_(map.equal_), alloc_(NodeTypeAllocTraits::select_on_container_copy_construction(map.alloc_)), node_alloc_(alloc_) {
        base_ = AllocateNewBase();
        map_.resize(hash_size_, nullptr);
        for (const auto& elem : map) {
            emplace(elem);
        }
    }
    UnorderedMap(UnorderedMap&& map) noexcept(
        std::is_nothrow_move_constructible<std::vector<Node*, NodePtrAlloc>>::value&&
            std::is_nothrow_move_constructible<Hash>::value&&
                std::is_nothrow_move_constructible<Equal>::value&&
                    std::is_nothrow_move_constructible<Alloc>::value&&
                        std::is_nothrow_copy_constructible<Alloc>::value)
        : hash_size_(std::move(map.hash_size_)), max_load_factor_(std::move(map.max_load_factor_)), size_(map.size_), base_(map.base_), map_(std::move(map.map_)), hash_(std::move(map.hash_)), equal_(std::move(map.equal_)), alloc_(std::move(map.alloc_)), node_alloc_(alloc_) {
        map.base_ = nullptr;
    }
    ~UnorderedMap() {
        clear();
        assert(size_ == 0 && (base_ == nullptr) ? true : base_->next == nullptr);
        if (base_ != nullptr) {
            eliminate_node(base_);
            base_ = nullptr;
        }
    }

    UnorderedMap& operator=(const UnorderedMap& map) & {
        if (&map == this) {
            return *this;
        }
        clear();
        hash_size_ = map.hash_size_;
        max_load_factor_ = map.max_load_factor_;
        hash_ = map.hash_;
        equal_ = map.equal_;
        alloc_ = NodeTypeAllocTraits::select_on_container_copy_construction(map.alloc_);
        node_alloc_ = NodeAlloc(alloc_);
        map_.resize(hash_size_, nullptr);
        for (const auto& elem : map) {
            emplace(elem);
        }
        assert(map.size_ == size_);
        return *this;
    }
    UnorderedMap& operator=(UnorderedMap&& map) & noexcept(
        std::is_nothrow_move_constructible<std::vector<Node*, NodePtrAlloc>>::value&&
            std::is_nothrow_move_constructible<Hash>::value&&
                std::is_nothrow_move_constructible<Equal>::value&&
                    std::is_nothrow_move_constructible<Alloc>::value&&
                        std::is_nothrow_copy_constructible<Alloc>::value) {
        if (&map == this) {
            return *this;
        }
        clear();
        hash_size_ = map.hash_size_;
        max_load_factor_ = map.max_load_factor_;
        hash_ = std::move(map.hash_);
        equal_ = std::move(map.equal_);
        alloc_ = std::move(map.alloc_);
        node_alloc_ = NodeAlloc(alloc_);
        size_ = map.size_;
        map_ = std::move(map.map_);
        ChangeBase(map.base_);
        map.base_ = nullptr;
        map.size_ = 0;
        return *this;
    }

    Value& at(const Key& key) {
        auto iter = find(key);
        if (iter == end()) {
            std::string mes = "UnorderedMap at()";
            throw std::out_of_range(mes.c_str());
        }
        return (iter->second);
    }

    const Value& at(const Key& key) const {
        auto iter = find(key);
        if (iter == end()) {
            std::string mes = "UnorderedMap at()";
            throw std::out_of_range(mes.c_str());
        }
        return (iter->second);
    }

    Value& operator[](const Key& key) {
        auto inserter = emplace(key, std::move(Value()));
        return (inserter.first.iter_->value->second);
    }

    Value& operator[](Key&& key) {
        auto inserter = emplace(std::move(key), std::move(Value()));
        return (inserter.first.iter_->value->second);
    }

    std::pair<iterator, bool> insert(const NodeType& node) {
        return emplace(node);
    }

    std::pair<iterator, bool> insert(NodeType&& node) {
        return emplace(std::move(const_cast<Key&>(node.first)), std::move(node.second));  // NOLINT
    }

    template <typename InputIt>
    void insert(InputIt start, InputIt end) {
        while (start != end) {
            emplace(*start);
            ++start;
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        NodeType* node_type = construct_nodetype(std::forward<Args>(args)...);
        iterator iter(nullptr);
        size_type key_hash;
        size_type key_idx;
        try {
            auto [key_hash_get, key_idx_get] = get_key_hash_and_idx(node_type->first);
            key_hash = key_hash_get;
            key_idx = key_idx_get;
        } catch (...) {
            eliminate_nodetype(node_type);
            throw;
        }
        Node* start = map_[key_idx];
        if (start != nullptr) {
            auto iter_find = start->next;
            while (iter_find != nullptr && iter_find->hash % hash_size_ == key_idx) {
                try {
                    if (iter_find->hash == key_hash && equal_(iter_find->value->first, node_type->first)) {
                        iter.iter_ = iter_find;
                        break;
                    }
                } catch (...) {
                    eliminate_nodetype(node_type);
                    throw;
                }
                iter_find = iter_find->next;
            }
        }
        if (iter.iter_ != nullptr) {
            eliminate_nodetype(node_type);
            return {iter, false};
        }
        Node* node = NodeAllocTraits::allocate(node_alloc_, 1);
        try {
            NodeAllocTraits::construct(node_alloc_, node, node_type, key_hash);
        } catch (...) {
            eliminate_nodetype(node_type);
            NodeAllocTraits::deallocate(node_alloc_, node, 1);
            throw;
        }
        if (map_[key_idx] == nullptr) {
            if (size_ != 0) {
                map_[base_->next->hash % hash_size_] = node;
            }
            map_[key_idx] = base_;
        }
        add_node(map_[key_idx], node);
        ++size_;
        check_rehash();
        return {iterator(map_[key_idx]->next), true};
    }

    iterator find(const Key& key) {
        auto [key_hash, key_idx] = get_key_hash_and_idx(key);
        Node* start = map_[key_idx];
        if (start == nullptr) {
            return iterator(start);
        }
        auto iter_find = start->next;
        while (iter_find != nullptr && iter_find->hash % hash_size_ == key_idx) {
            if (iter_find->hash == key_hash && equal_(iter_find->value->first, key)) {
                return iterator(iter_find);
            }
            iter_find = iter_find->next;
        }
        return iterator(nullptr);
    }

    const_iterator find(const Key& key) const {
        auto [key_hash, key_idx] = get_key_hash_and_idx(key);
        Node* start = map_[key_idx];
        if (start == nullptr) {
            return const_iterator(nullptr);
        }
        auto iter = const_iterator(start->next);
        while (iter != nullptr&& iter->hash %= hash_size_ == key_idx) {
            if (iter->hash == key_hash && equal_(iter->value.first, key)) {
                return iter;
            }
            ++iter;
        }
        return const_iterator(nullptr);
    }

    void erase(iterator iter) {
        if (iter.iter_ == nullptr) {
            assert("erase empty iterator");
            return;
        }
        size_t idx = iter.iter_->hash % hash_size_;
        auto prev_to_hash = map_[idx];
        if (prev_to_hash == nullptr) {
            assert("erase empty hash");
            return;
        }
        while (prev_to_hash->next != iter.iter_) {
            prev_to_hash = prev_to_hash->next;
        }
        if (iter.iter_->next == nullptr) {
            /// last in forward list check
            size_type change_chain_ptr_idx = iter.iter_->hash % hash_size_;
            prev_to_hash->next = nullptr;
            if (prev_to_hash == base_ || prev_to_hash->hash % hash_size_ != change_chain_ptr_idx) {
                map_[change_chain_ptr_idx] = nullptr;
            }
        } else if (iter.iter_->next != nullptr && iter.iter_->next->hash % hash_size_ != iter.iter_->hash % hash_size_) {
            /// last in hash chunk check
            size_type change_chain_ptr_idx = iter.iter_->next->hash % hash_size_;
            map_[change_chain_ptr_idx] = prev_to_hash;
            prev_to_hash->next = iter.iter_->next;
            if (prev_to_hash->hash % hash_size_ != iter.iter_->hash % hash_size_) {
                map_[iter.iter_->hash % hash_size_] = nullptr;
            }
        } else {
            prev_to_hash->next = iter.iter_->next;
        }
        --size_;
        ClearNode(iter.iter_);
    }

    void erase(iterator start, iterator end) {
        while (start != end) {
            iterator it = start;
            ++start;
            erase(it);
        }
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }
    [[nodiscard]] double load_factor() const& {
        return static_cast<double>(size_) / hash_size_;
    }
    [[nodiscard]] double max_load_factor() const& {
        return max_load_factor_;
    }
    void max_load_factor(double nex_max_load_factor) & {
        max_load_factor_ = nex_max_load_factor;
    }

    void rehash(size_t new_hash_size = 0) {
        if (base_ == nullptr) {
            return;
        }
        if (new_hash_size > hash_size_) {
            hash_size_ = new_hash_size;
            size_ = 0;
            std::vector<Node*, NodePtrAlloc> new_map(new_hash_size, nullptr);
            Node* tail = base_->next;
            base_->next = nullptr;
            while (tail != nullptr) {
                Node* cur = tail;
                tail = tail->next;
                cur->next = nullptr;
                insert_node(cur, new_map);
            }
            map_ = std::move(new_map);
        }
    }

    void reserve(size_t reserve_hash_size) {
        if (reserve_hash_size > hash_size_) {
            rehash(reserve_hash_size);
        }
    }

    void clear() noexcept {
        if (base_ == nullptr) {
            return;
        }
        Node* node = base_->next;
        while (node != nullptr) {
            Node* it = node;
            node = node->next;
            eliminate_nodetype(it->value);
            eliminate_node(it);
        }
        base_->next = nullptr;
        size_ = 0;
        std::fill(map_.begin(), map_.end(), nullptr);
    }

  private:
    void try_construct_node(Node* node) {
        try {
            NodeAllocTraits::construct(node_alloc_, node);
        } catch (...) {
            NodeAllocTraits::deallocate(node_alloc_, node, 1);
            throw;
        }
    }
    void eliminate_node(Node* node) noexcept {
        if (node != nullptr) {
            NodeAllocTraits::destroy(node_alloc_, node);
            NodeAllocTraits::deallocate(node_alloc_, node, 1);
        }
    }
    void eliminate_nodetype(value_type* node_type) noexcept {
        if (node_type != nullptr) {
            NodeTypeAllocTraits::destroy(alloc_, node_type);
            NodeTypeAllocTraits::deallocate(alloc_, node_type, 1);
        }
    }
    void insert_node(Node* node, std::vector<Node*, NodePtrAlloc>& map) {
        size_type key_idx = node->hash % hash_size_;
        if (map[key_idx] == nullptr) {
            if (size_ != 0) {
                map[base_->next->hash % hash_size_] = node;
            }
            map[key_idx] = base_;
        }
        add_node(map[key_idx], node);
        ++size_;
    }
    void check_rehash() {
        if (load_factor() >= max_load_factor_) {
            rehash(2 * hash_size_);
        }
    }
    template <typename... Args>
    pointer construct_nodetype(Args&&... args) {
        NodeType* node = NodeTypeAllocTraits::allocate(alloc_, 1);
        try {
            NodeTypeAllocTraits::construct(alloc_, node, std::forward<Args>(args)...);
        } catch (...) {
            NodeTypeAllocTraits::deallocate(alloc_, node, 1);
            throw;
        }
        return node;
    }
    std::pair<size_type, size_type> get_key_hash_and_idx(const Key& key) {
        size_type key_hash = hash_(key);
        return {key_hash, key_hash % hash_size_};
    }
    void add_node(Node* to, Node* node) noexcept {
        if (to == nullptr || node == nullptr) {
            std::cerr << "add_node() : 'to' or 'node' is nullptr" << std::endl;
            return;
        }
        Node* next_to = to->next;
        to->next = node;
        node->next = next_to;
    }
    static constexpr size_t default_hash_size_ = 100;
    size_t hash_size_;
    double max_load_factor_;
    size_type size_;
    Node* base_;
    std::vector<Node*, NodePtrAlloc> map_;
    [[no_unique_address]] Hash hash_;
    [[no_unique_address]] Equal equal_;
    [[no_unique_address]] NodeTypeAlloc alloc_;
    [[no_unique_address]] NodeAlloc node_alloc_;
};