#pragma once

#include <cassert>
#include <iostream>

namespace ControllBlocks {

//////////////////////////////////////////////////////////////
///// ControllBlocks start
//////////////////////////////////////////////////////////////

struct BaseControllBlock {
    using size_type = size_t;

    size_type shared_cnt = 0;
    size_type weak_cnt = 0;

    BaseControllBlock() = default;
    BaseControllBlock(size_t shared_cnt, size_t weak_cnt)
        : shared_cnt(shared_cnt), weak_cnt(weak_cnt) {}
    virtual ~BaseControllBlock() = default;

    void add_shared() noexcept {
        ++shared_cnt;
    }

    void add_weak() noexcept {
        ++weak_cnt;
    }

    virtual void useDeleter() = 0;
    virtual void deallocThis() = 0;
};

template <typename T, typename Deleter, typename Alloc>
struct ControllBlockRegular : public BaseControllBlock {
    using size_type = BaseControllBlock::size_type;
    using value_type = T;

    value_type* value = nullptr;
    Deleter deleter;
    Alloc alloc;

    ControllBlockRegular(size_t shared_cnt, size_t weak_cnt, value_type* ptr,
                         const Deleter& del, const Alloc& all)
        : BaseControllBlock(shared_cnt, weak_cnt),
          value(ptr),
          deleter(del),
          alloc(all) {}
    ~ControllBlockRegular() override {
        deleter(value);
    }

    void useDeleter() override {
        deleter(value);
        value = nullptr;
    }
    void deallocThis() override {
        using CBRegularAllocTraits = typename std::allocator_traits<
            Alloc>::template rebind_traits<ControllBlockRegular>;
        typename CBRegularAllocTraits::allocator_type block_alloc(alloc);
        CBRegularAllocTraits::deallocate(block_alloc, this, 1);
    }
};

template <typename T, typename Alloc>
struct ControllBlockMakeShared : public BaseControllBlock {
    template <typename A>
    friend class SharedPtr;

    using size_type = BaseControllBlock::size_type;
    using value_type = T;

    value_type value;
    Alloc alloc;

    template <typename... Args>
    ControllBlockMakeShared(size_t shared_cnt, size_t weak_cnt,
                            const Alloc& all, Args&&... args)
        : BaseControllBlock(shared_cnt, weak_cnt),
          value(std::forward<Args>(args)...),
          alloc(all) {}
    ~ControllBlockMakeShared() override = default;

    void useDeleter() override {
        using ValueAllocTraits =
            typename std::allocator_traits<Alloc>::template rebind_traits<T>;
        typename ValueAllocTraits::allocator_type val_alloc(alloc);
        ValueAllocTraits::destroy(val_alloc, std::addressof(value));
    }
    void deallocThis() override {
        using CBMakeSharedAllocTraits = typename std::allocator_traits<
            Alloc>::template rebind_traits<ControllBlockMakeShared>;
        typename CBMakeSharedAllocTraits::allocator_type block_alloc(alloc);
        CBMakeSharedAllocTraits::deallocate(block_alloc, this, 1);
    }
};

//////////////////////////////////////////////////////////////
///// ControllBlocks finish
//////////////////////////////////////////////////////////////

}  // namespace ControllBlocks

template <typename T>
struct EnableSharedFromThis;

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
    template <typename A>
    friend class SharedPtr;
    template <typename A>
    friend class WeakPtr;
    template <typename A>
    friend struct EnableSharedFromThis;

    template <class A, class Alloc, class... Args>
    friend SharedPtr<A> allocateShared(const Alloc& all, Args&&... args);

  public:
    using size_type = size_t;
    using value_type = T;
    using reference = value_type&;
    using block = ControllBlocks::BaseControllBlock;

    SharedPtr() : value_(nullptr), block_(nullptr) {}
    explicit SharedPtr(std::nullptr_t) : value_(nullptr), block_(nullptr) {}

    template <typename A = T>
    SharedPtr(A* ptr)
        : SharedPtr(ptr, std::default_delete<T>(), std::allocator<T>()) {}

    template <typename A = T, typename Deleter = std::default_delete<T>>
    SharedPtr(A* ptr, const Deleter& del)
        : SharedPtr(ptr, del, std::allocator<T>()) {}

    template <typename A = T, typename Deleter = std::default_delete<T>,
              typename Alloc = std::allocator<T>>
    SharedPtr(A* ptr, const Deleter& del, const Alloc& all)
        : value_(ptr), block_(nullptr) {
        if (value_ == nullptr) {
            return;
        }

        using CBRegular =
            ControllBlocks::ControllBlockRegular<T, Deleter, Alloc>;
        using CBRegularAlloc = typename std::allocator_traits<
            Alloc>::template rebind_alloc<CBRegular>;
        using CBRegularAllocTraits = typename std::allocator_traits<
            Alloc>::template rebind_traits<CBRegular>;

        CBRegularAlloc block_alloc(all);
        CBRegular* block = CBRegularAllocTraits::allocate(block_alloc, 1);
        try {
            size_type shared_cnt = ptr == nullptr ? 0 : 1;
            //CBRegularAllocTraits::construct(block_alloc, block, 1, 0, ptr, del, all);
            new (block) CBRegular(shared_cnt, 0, ptr, del, all);
        } catch (...) {
            CBRegularAllocTraits::deallocate(block_alloc, block, 1);
            throw;
        }
        block_ = block;
        if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
            ptr->weak_this_ = *this;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept
        : value_(other.value_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->add_shared();
        }
    }

    template <class A>
    SharedPtr(const SharedPtr<A>& other) noexcept
        : value_(other.value_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->add_shared();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept
        : value_(other.value_), block_(other.block_) {
        other.value_ = nullptr;
        other.block_ = nullptr;
    }

    template <class A>
    SharedPtr(SharedPtr<A>&& other) noexcept
        : value_(other.value_), block_(other.block_) {
        other.value_ = nullptr;
        other.block_ = nullptr;
    }

    template <class A>
    explicit SharedPtr(const WeakPtr<A>& other)
        : SharedPtr(std::move(other.lock())) {
        if (value_ == nullptr) {
            throw std::bad_weak_ptr();
        }
    }

    ~SharedPtr() {
        decrement();
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (&other == this) {
            return *this;
        }
        SharedPtr(other).swap(*this);
        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    void reset() {
        SharedPtr().swap(*this);
    }
    template <typename A = T>
    void reset(A* ptr) {
        SharedPtr(ptr).swap(*this);
    }
    template <typename A = T, typename Deleter = std::default_delete<T>>
    void reset(A* ptr, const Deleter& del) {
        SharedPtr(ptr, del).swap(*this);
    }
    template <typename A = T, typename Deleter = std::default_delete<T>,
              typename Alloc = std::allocator<T>>
    void reset(A* ptr, const Deleter& del, const Alloc& all) {
        SharedPtr(ptr, del, all).swap(*this);
    }

    [[nodiscard]] size_t use_count() const noexcept {
        assert(block_ != nullptr);
        return block_->shared_cnt;
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(value_, other.value_);
        std::swap(block_, other.block_);
    }

    value_type* operator->() const noexcept {
        return value_;
    }

    reference operator*() const noexcept {
        return *(value_);
    }

    [[nodiscard]] value_type* get() const noexcept {
        return value_;
    }

  private:
    template <typename Alloc>
    SharedPtr(ControllBlocks::ControllBlockMakeShared<T, Alloc>* block)
        : value_(std::addressof(block->value)), block_(block) {}
    SharedPtr(const WeakPtr<T>& other)
        : value_(other.value_), block_(other.block_) {}
    void decrement() noexcept {
        if (block_ == nullptr) {
            return;
        }
        assert(block_->shared_cnt != 0);
        --block_->shared_cnt;
        if (block_->shared_cnt == 0 && block_->weak_cnt == 0) {
            block_->useDeleter();
            block_->deallocThis();
            block_ = nullptr;
            value_ = nullptr;
        } else if (block_->shared_cnt == 0) {
            block_->useDeleter();
            value_ = nullptr;
        }
    }
    value_type* value_;
    block* block_;
};

template <class T, class Alloc, class... Args>
SharedPtr<T> allocateShared(const Alloc& all, Args&&... args) {
    using CBMakeShared = ControllBlocks::ControllBlockMakeShared<T, Alloc>;
    using CBMakeSharedAlloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<CBMakeShared>;
    using CBMakeSharedAllocTraits = typename std::allocator_traits<
        Alloc>::template rebind_traits<CBMakeShared>;

    CBMakeSharedAlloc block_alloc(all);
    CBMakeShared* block = CBMakeSharedAllocTraits::allocate(block_alloc, 1);
    try {
        CBMakeSharedAllocTraits::construct(block_alloc, block, 1, 0, all,
                                           std::forward<Args>(args)...);
    } catch (...) {
        CBMakeSharedAllocTraits::deallocate(block_alloc, block, 1);
    }
    return SharedPtr(block);
}

template <class T, class... Args>
SharedPtr<T> makeShared(Args&&... args) {
    return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template <typename T>
class WeakPtr {
    template <typename A>
    friend class SharedPtr;

    template <typename A>
    friend class WeakPtr;

  public:
    using size_type = typename SharedPtr<T>::size_type;
    using value_type = typename SharedPtr<T>::value_type;
    using reference = typename SharedPtr<T>::reference;
    using block = typename SharedPtr<T>::block;

    WeakPtr() : value_(nullptr), block_(nullptr) {}

    template <typename A>
    WeakPtr(const SharedPtr<A>& other)
        : value_(other.value_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->add_weak();
        }
    }

    WeakPtr(const WeakPtr& other) noexcept
        : value_(other.value_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->add_weak();
        }
    }

    template <typename A>
    WeakPtr(const WeakPtr<A>& other) noexcept
        : value_(other.value_), block_(other.block_) {
        if (block_ != nullptr) {
            block_->add_weak();
        }
    }

    WeakPtr(WeakPtr&& other) noexcept
        : value_(other.value_), block_(other.block_) {
        other.value_ = nullptr;
        other.block_ = nullptr;
    }

    ~WeakPtr() {
        decrement();
    }

    template <typename A>
    WeakPtr& operator=(const WeakPtr<A>& other) {
        decrement();
        value_ = other.value_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->add_weak();
        }
        return *this;
    }

    template <typename A>
    WeakPtr& operator=(WeakPtr<A>&& other) {
        decrement();
        value_ = other.value_;
        other.value_ = nullptr;
        block_ = other.block_;
        other.block_ = nullptr;
        return *this;
    }

    template <typename A>
    WeakPtr& operator=(const SharedPtr<A>& other) {
        decrement();
        value_ = other.value_;
        block_ = other.block_;
        if (block_ != nullptr) {
            block_->add_weak();
        }
        return *this;
    }

    [[nodiscard]] size_t use_count() const noexcept {
        assert(block_ != nullptr);
        return block_->shared_cnt;
    }

    [[nodiscard]] bool expired() const {
        return block_ == nullptr || block_->shared_cnt == 0;
    }

    [[nodiscard]] SharedPtr<T> lock() const {
        if (block_ == nullptr) {
            return SharedPtr<T>();
        }
        block_->add_shared();
        return SharedPtr<T>(*this);
    }

  private:
    void decrement() noexcept {
        if (block_ == nullptr) {
            return;
        }
        assert(block_->weak_cnt > 0);
        --block_->weak_cnt;
        if (block_->weak_cnt == 0 && block_->shared_cnt == 0) {
            block_->deallocThis();
        }
    }
    value_type* value_;
    block* block_;
};


/*
template <typename T>
struct EnableSharedFromThis {
    template <typename A>
    friend class SharedPtr;

  public:
    SharedPtr<T> shared_from_this() {
        if (weak_this_.expired()) {
            throw std::bad_weak_ptr();
        }
        return SharedPtr<T>(weak_this_);
    }

    SharedPtr<const T> shared_from_this() const {
        if (weak_this_.expired()) {
            throw std::bad_weak_ptr();
        }
        return SharedPtr<const T>(weak_this_);
    }

    WeakPtr<T> weak_from_this() noexcept {
        return weak_this_;
    }

    WeakPtr<const T> weak_from_this() const noexcept {
        return weak_this_;
    }

  private:
    mutable WeakPtr<T> weak_this_;
};
*/