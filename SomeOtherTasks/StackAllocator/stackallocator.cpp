#pragma once

#include <cstdint>
#include <iostream>
#include <memory>

template <size_t N>
class StackStorage {
  public:
    using size_type = size_t;

    StackStorage()
        : stack_(new char[N]), st_size_(N) {}
    ~StackStorage() {
        delete[] stack_;
    }

    StackStorage(const StackStorage<N>&) = delete;
    StackStorage& operator=(const StackStorage<N>&) = delete;

    template <typename T>
    T* GetAlligned(size_type count, size_type al = alignof(T)) {
        void* address = stack_ + N - st_size_;
        if (std::align(al, count * sizeof(T), address, st_size_)) {
            st_size_ -= count * sizeof(T);
            return reinterpret_cast<T*>(address);
        }
        throw std::bad_alloc();
    }

  private:
    char* stack_;
    size_type st_size_;
};

template <typename T, size_t N>
class StackAllocator {
  public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;

    StackAllocator(StackStorage<N>& stack_storage)
        : storage_(&stack_storage) {}
    StackAllocator(const StackAllocator& alloc)
        : storage_(alloc.GetStorage()) {}

    template <class U>
    StackAllocator(const StackAllocator<U, N>& alloc)
        : storage_(alloc.GetStorage()) {}

    ~StackAllocator() = default;

    template <size_t U>
    bool operator==(const StackAllocator<T, U>& alloc) {
        return (U == N) && (&storage_ == &alloc.storage_);
    }
    template <size_t U>
    bool operator!=(const StackAllocator<T, U>& alloc) {
        return (U != N) || (&storage_ != &alloc.storage_);
    }

    pointer allocate(size_type size) {
        if (size == 0) {
            ++size;
        }
        return storage_->template GetAlligned<T>(size);
    }

    void deallocate(pointer obj, size_type n) {
        std::ignore = obj;
        std::ignore = n;
    }

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };

    struct propagate_on_container_copy_assignment : std::false_type {
    };

    StackAllocator select_on_container_copy_construction() {
        return StackAllocator<T, N>{storage_};
    }

    StackStorage<N>* GetStorage() const {
        return storage_;
    }

  private:
    StackStorage<N>* storage_;
};