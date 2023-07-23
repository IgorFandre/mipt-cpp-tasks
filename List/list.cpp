#pragma once

#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
    struct BaseNode;
    struct ListNode;
    using NodeAllocType = typename std::allocator_traits<Allocator>::template rebind_alloc<ListNode>;
    using NodeAllocTraits = typename std::allocator_traits<Allocator>::template rebind_traits<ListNode>;

  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

  private:
    struct BaseNode {
        BaseNode* next;
        BaseNode* prev;
        BaseNode() {
            next = this;
            prev = this;
        }
        virtual ~BaseNode() = default;
    };

    struct ListNode : public BaseNode {
        value_type value;
        ListNode()
            : BaseNode(), value() {}
        explicit ListNode(const T& value)
            : BaseNode(), value(value) {
        }
        ~ListNode() override = default;
    };

  public:
    template <bool IsConst>
    class CommonIterator {
        friend List;

      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = List::value_type;
        using reference = std::conditional_t<IsConst, List::const_reference, List::reference>;
        using pointer = std::conditional_t<IsConst, List::const_pointer, List::pointer>;
        using difference_type = int64_t;
        using node_pointer = std::conditional_t<IsConst, const BaseNode*, BaseNode*>;
        using parent_node_pointer = std::conditional_t<IsConst, const ListNode*, ListNode*>;

        CommonIterator() = default;
        CommonIterator(const CommonIterator& it) = default;
        CommonIterator& operator=(const CommonIterator& it) = default;

        reference operator*() const {
            auto node = dynamic_cast<parent_node_pointer>(node_);
            return node->value;
        }

        pointer operator->() const {
            auto node = dynamic_cast<parent_node_pointer>(node_);
            return &(node->value);
        }

        CommonIterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        CommonIterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        CommonIterator operator++(int) & {
            CommonIterator copy = *this;
            node_ = node_->next;
            return copy;
        }

        CommonIterator operator--(int) & {
            CommonIterator copy = *this;
            node_ = node_->prev;
            return copy;
        }

        bool operator==(const CommonIterator& it) const {
            return node_ == it.node_;
        }
        bool operator!=(const CommonIterator& it) const {
            return node_ != it.node_;
        }

        operator CommonIterator<true>() const {
            return CommonIterator<true>{node_};
        };

      private:
        explicit CommonIterator(node_pointer node = nullptr)
            : node_(node) {}
        node_pointer node_;
    };

    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(base_.next);
    }
    const_iterator begin() const {
        return const_iterator(base_.next);
    }
    const_iterator cbegin() const {
        return const_iterator(base_.next);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(end());
    }

    iterator end() {
        return iterator(&base_);
    }
    const_iterator end() const {
        return const_iterator(&base_);
    }
    const_iterator cend() const {
        return const_iterator(&base_);
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin());
    }

    List()
        : alloc_(NodeAllocType()), size_(0), base_() {}
    List(const Allocator& alloc)
        : alloc_(alloc), size_(0), base_() {}
    List(size_type size, const Allocator& alloc = Allocator())
        : List(alloc) {
        try {
            for (size_type i = 0; i < size; ++i) {
                emplace(end());
            }
        } catch (...) {
            size_type got_size = size_;
            for (size_type i = 0; i < got_size; ++i) {
                pop_back();
            }
            throw;
        }
        size_ = size;
    }
    List(const List& list)
        : alloc_(NodeAllocTraits::select_on_container_copy_construction(list.alloc_)),
          size_(0),
          base_() {
        copy_nodes(list);
    }
    template <typename OtherAlloc>
    explicit List(const List<T, OtherAlloc>& list)
        : alloc_(NodeAllocType()), size_(0), base_() {
        copy_nodes(list);
    }

    ~List() {
        clear_nodes();
    }

    List& operator=(const List& list) {
        List to_swap(list);
        swap(to_swap);
        if constexpr (NodeAllocTraits::propagate_on_container_copy_assignment::value) {
            alloc_ = list.alloc_;
        }
        return *this;
    }

    void push_back(const_reference elem) {
        emplace(end(), elem);
    }
    void push_front(const_reference elem) {
        emplace(begin(), elem);
    }

    void pop_back() noexcept {
        erase(iterator(base_.prev));
    }
    void pop_front() noexcept {
        erase(iterator(base_.next));
    }

    template <typename... Args>
    void emplace(const_iterator it, Args&&... args) {
        auto* inserter = const_cast<BaseNode*>(it.node_->prev);  // NOLINT
        ListNode* new_node = NodeAllocTraits::allocate(alloc_, 1);
        try {
            NodeAllocTraits::construct(alloc_, new_node, std::forward<Args>(args)...);
            base_after(inserter, new_node);
        } catch (...) {
            NodeAllocTraits::deallocate(alloc_, dynamic_cast<ListNode*>(new_node), 1);
            throw;
        }
        ++size_;
    }

    void insert(const_iterator it, const_reference elem) {
        emplace(it, elem);
    }

    void erase(const_iterator it) noexcept {
        --size_;
        auto* to_clear = const_cast<BaseNode*>(it.node_);  // NOLINT
        unbase(to_clear, to_clear);
        NodeAllocTraits::destroy(alloc_, to_clear);
        NodeAllocTraits::deallocate(alloc_, dynamic_cast<ListNode*>(to_clear), 1);
    }

    void swap(List& list) {
        if constexpr (NodeAllocTraits::propagate_on_container_swap::value) {
            std::swap(alloc_, list.alloc_);
        }
        std::swap(base_, list.base_);
        std::swap(base_.next->prev, list.base_.next->prev);
        std::swap(base_.prev->next, list.base_.prev->next);
        std::swap(size_, list.size_);
    }

    [[nodiscard]] bool empty() const noexcept {
        return size_ == 0;
    }
    [[nodiscard]] size_type size() const noexcept {
        return size_;
    }
    [[nodiscard]] Allocator get_allocator() const noexcept {
        return alloc_;
    }

  private:
    void insert_after(BaseNode* node, const_reference elem) {
        ListNode* new_node = NodeAllocTraits::allocate(alloc_, 1);
        try {
            NodeAllocTraits::construct(alloc_, new_node, elem);
            base_after(node, new_node);
        } catch (...) {
            NodeAllocTraits::deallocate(alloc_, dynamic_cast<ListNode*>(new_node), 1);
            throw;
        }
    }
    static void unbase(BaseNode* start, BaseNode* end) {
        start->prev->next = end->next;
        end->next->prev = start->prev;
    }
    void base(BaseNode* start, BaseNode* end) {
        base_.prev->next = start;
        start->prev = base_.prev;
        base_.prev = end;
        end->next = &base_;
    }
    void base_after(BaseNode* before, BaseNode* new_after) {
        new_after->next = before->next;
        new_after->next->prev = new_after;
        new_after->prev = before;
        before->next = new_after;
    }
    template <typename OtherAlloc>
    void copy_nodes(const List<T, OtherAlloc>& list) {
        try {
            for (const auto& node : list) {
                push_back(node);
            }
        } catch (...) {
            size_type size = size_;
            for (size_type i = 0; i < size; ++i) {
                pop_back();
            }
            throw;
        }
    }
    void clear_nodes() {
        BaseNode* deleter = base_.next;
        while (size_ != 0) {
            deleter = deleter->next;
            NodeAllocTraits::destroy(alloc_, deleter->prev);
            NodeAllocTraits::deallocate(alloc_, dynamic_cast<ListNode*>(deleter->prev), 1);
            --size_;
        }
        base_.next = base_.prev = &base_;
    }

    NodeAllocType alloc_;
    size_type size_;
    BaseNode base_;
};

template <typename T, typename Allocator = std::allocator<T>>
std::ostream& operator<<(std::ostream& out, const List<T, Allocator>& list) {
    out << "[ ";
    auto it_1 = list.begin();
    auto it_2 = list.begin();
    if (list.empty()) {
        out << "]" << std::endl;
        return out;
    }
    ++it_2;
    while (true) {
        if (it_2 == list.end()) {
            out << *it_1 << " ]" << std::endl;
            return out;
        }
        out << *it_1 << ", ";
        ++it_1;
        ++it_2;
    }
}