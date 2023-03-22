#ifndef DEQUE_H_
#define DEQUE_H_

#include <array>
#include <cassert>
#include <iostream>
#include <vector>


template<typename T>
class Deque {
  struct Index {
    size_t block_;
    size_t index_;

    Index() : block_{0}, index_{0} {}
    Index(size_t block, size_t index) : block_{block}, index_{index} {}

    Index& operator++() {
      ++index_;
      if (index_ == BLOCK_SIZE) {
        ++block_;
        index_ = 0;
      }
      return *this;
    }
    void Extract(ptrdiff_t n) {
      if (n < 0) {
        *this += (-n);
      } else {
        *this -= n;
      }
    }
    void Add(ptrdiff_t n) {
      if (n < 0) {
        *this -= (-n);
      } else {
        *this += n;
      }
    }
    Index& operator--() {
      if (index_ == 0) {
        --block_;
        index_ = BLOCK_SIZE;
      }
      --index_;
      return *this;
    }
    Index& operator+=(size_t go) {
      block_ += go / BLOCK_SIZE;
      index_ += go % BLOCK_SIZE;
      if (index_ >= BLOCK_SIZE) {
        block_ += (index_ / BLOCK_SIZE);
        index_ %= BLOCK_SIZE;
      }
      return *this;
    }
    Index& operator-=(size_t go) {
      block_ -= go / BLOCK_SIZE;
      if (index_ < go % BLOCK_SIZE) {
        --block_;
        index_ += BLOCK_SIZE;
      }
      index_ -= go % BLOCK_SIZE;
      return *this;
    }
    Index operator+(size_t go) const {
      Index copy = *this;
      copy.block_ += go / BLOCK_SIZE;
      copy.index_ += go % BLOCK_SIZE;
      if (copy.index_ >= BLOCK_SIZE) {
        copy.block_ += copy.index_ / BLOCK_SIZE;
        copy.index_ %= BLOCK_SIZE;
      }
      return copy;
    }
    friend ptrdiff_t operator-(const Index& i_1, const Index& i_2) {
      auto i_1_general = static_cast<ptrdiff_t>(i_1.block_ * BLOCK_SIZE + i_1.index_);
      auto i_2_general = static_cast<ptrdiff_t>(i_2.block_ * BLOCK_SIZE + i_2.index_);
      return i_1_general - i_2_general;
    }
    friend auto operator<=>(const Index& i_1, const Index& i_2) {
      if (i_1.block_ != i_2.block_) {
        return i_1.block_ <=> i_2.block_;
      }
      return i_1.index_ <=> i_2.index_;
    }
    bool operator==(const Index& idx) const {
      return block_ == idx.block_ && index_ == idx.index_;
    }
    bool operator!=(const Index& idx) const {
      return block_ != idx.block_ || index_ != idx.index_;
    }
  };

  class Chunk {
   public:
    Chunk() : chunk_{nullptr}, create_{false} {}
    explicit Chunk(bool create) : chunk_{nullptr}, create_{create} {
      if (create) {
        chunk_ = reinterpret_cast<T*>(new char[BLOCK_SIZE * sizeof(T)]);
      }
    }
    ~Chunk() {
      if (create_) {
        delete[] reinterpret_cast<const char*>(chunk_);
        chunk_ = nullptr;
      }
    }
    void MakeChunk() {
      if (!create_) {
        create_ = true;
        chunk_ = reinterpret_cast<T*>(new char[BLOCK_SIZE * sizeof(T)]);
      }
    }
    void Swap(Chunk& chunk) {
      std::swap(chunk_, chunk.chunk_);
      std::swap(create_, chunk.create_);
    }
    void DelChunk() {
      if (create_) {
        create_ = false;
        delete[] reinterpret_cast<const char*>(chunk_);
      }
    }
    void SetNull() {
      create_ = false;
    }
    T* data() const {
      return chunk_;
    }
    T& operator[](size_t idx) const {
      return chunk_[idx];
    }
   private:
    T* chunk_;
    bool create_;
  };

  static constexpr size_t BLOCK_SIZE = 2;
  Index start_;
  Index end_;
  size_t deque_cap_;
  std::vector<Chunk> deque_;

 public:

  template<bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using chunks = std::conditional_t<IsConst, const Chunk*, Chunk*>;

    CommonIterator() = default;
    CommonIterator(const CommonIterator& other) = default;
    CommonIterator& operator=(const CommonIterator& other) = default;

    friend auto operator<=>(const CommonIterator& it_1, const CommonIterator& it_2) {
      return it_1.position_ <=> it_2.position_;
    }
    bool operator==(const CommonIterator& it) const {
      return position_ == it.position_;
    }
    bool operator!=(const CommonIterator& it) const {
      return position_ != it.position_;
    }

    CommonIterator& operator++() {
      ++position_;
      return *this;
    }
    CommonIterator& operator--() {
      --position_;
      return *this;
    }
    CommonIterator operator++(int) {
      auto copy = *this;
      ++position_;
      return copy;
    }

    CommonIterator operator--(int) {
      auto copy = *this;
      --position_;
      return copy;
    }
    CommonIterator& operator+=(ptrdiff_t n) {
      position_.Add(n);
      return *this;
    }
    CommonIterator operator+(ptrdiff_t n) const {
      auto copy{*this};
      copy.position_.Add(n);
      return copy;
    }
    friend CommonIterator operator+(ptrdiff_t n, CommonIterator it) {
      it.Add(n);
      return it;
    }
    CommonIterator& operator-=(ptrdiff_t n) {
      position_.Extract(n);
      return *this;
    }
    CommonIterator operator-(ptrdiff_t n) const {
      auto copy{*this};
      copy.position_.Extract(n);
      return copy;
    }
    ptrdiff_t operator-(const CommonIterator& other) const {
      return position_ - other.position_;
    }
    reference operator*() const {
      return source_[position_.block_][position_.index_];
    }
    pointer operator->() const {
      return source_[position_.block_].data() + position_.index_;
    }
    reference operator[](size_t index) const {
      Index idx = position_ + index;
      return source_[idx.block_][idx.index_];
    }
    operator CommonIterator<true>() const {
      return CommonIterator<true>(source_, position_);
    }
    friend std::ostream& operator<<(std::ostream& os, CommonIterator<IsConst> c) {
      return (os << c.position_.block_ << "\t" << c.position_.index_);
    }
    std::pair<size_t, size_t> getter() {
      return {position_.block_, position_.index_};
    };
   private:
    CommonIterator(chunks deque, const Index& pos) : position_{pos}, source_{deque} {};
    friend Deque;
    Index position_;
    chunks source_;
  };
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;

  Deque()
      : start_(), end_(), deque_cap_{1}, deque_(1) {}
  explicit Deque(size_t n)
      : start_(), end_(), deque_cap_{n / BLOCK_SIZE + static_cast<int>(n % BLOCK_SIZE > 0)}, deque_(deque_cap_) {
    end_ += n;
    for (size_t i = start_.block_; i < end_.block_ + static_cast<int>(end_.index_ > 0); ++i) {
      deque_[i].MakeChunk();
    }
    Index cur = start_;
    try {
      for (; cur < end_; ++cur) {
        new(deque_[cur.block_].data() + cur.index_) T();
      }
    } catch (...) {
      for (Index i = start_; i < cur; ++i) {
        (deque_[i.block_].data() + i.index_)->~T();
      }
    }
  }
  Deque(size_t n, const T& elem)
      : start_(), end_(), deque_cap_(n / BLOCK_SIZE + static_cast<int>(n % BLOCK_SIZE > 0)), deque_(deque_cap_) {
    for (size_t i = start_.block_; i < deque_cap_; ++i) {
      deque_[start_.block_ + i].MakeChunk();
    }
    size_t cnt = 0;
    try {
      for (; cnt < n; ++cnt, ++end_) {
        new (deque_[end_.block_].data() + end_.index_) T(elem);
      }
    } catch (...) {
      for (size_t i = 0; i < cnt; ++i) {
        --end_;
        (deque_[end_.block_].data() + end_.index_)->~T();
      }
      for (size_t i = 0; i < deque_cap_; ++i) {
        deque_[start_.block_ + i].DelChunk();
      }
      deque_cap_ = 0;
      throw;
    }
  }
  Deque(const Deque& deque) : start_(deque.start_), end_(deque.end_), deque_cap_{deque.deque_cap_}, deque_(deque.deque_.size()) {
    Index idx{start_};
    try {
      for (; idx != end_; ++idx) {
        if (idx.index_ == 0) {
          deque_[idx.block_].MakeChunk();
        }
        new(deque_[idx.block_].data() + idx.index_) T(deque.deque_[idx.block_][idx.index_]);
      }
    } catch (...) {
      for (Index i = start_; i < idx; ++i) {
        (deque_[i.block_].data() + i.index_)->~T();
        if (idx.index_ == 0) {
          deque_[i.block_].DelChunk();
        }
      }
      end_ = start_;
      deque_cap_ = 0;
      throw;
    }
  }

  void swap(Deque& other) {
    std::swap(deque_cap_, other.deque_cap_);
    std::swap(deque_, other.deque_);
    std::swap(start_, other.start_);
    std::swap(end_, other.end_);
  }

  Deque& operator=(Deque other) {
    swap(other);
    return *this;
  }

  size_t size() const {
    ptrdiff_t diff = end_ - start_;
    return static_cast<size_t>(diff);
  }

  T& operator[](size_t idx) {
    Index res{start_};
    res += idx;
    return deque_[res.block_][res.index_];
  }
  const T& operator[](size_t idx) const {
    Index res{start_};
    res += idx;
    return deque_[res.block_][res.index_];
  }
  T& at(size_t idx) {
    Index res{start_};
    res += idx;
    if (res < start_ || end_ <= res) {
      throw std::out_of_range("Out of range deque!");
    }
    return deque_[res.block_][res.index_];
  }
  const T& at(size_t idx) const {
    Index res{start_};
    res += idx;
    if (res < start_ || end_ <= res) {
      throw std::out_of_range("Out of range deque!");
    }
    return deque_[res.block_][res.index_];
  }

  iterator begin() noexcept {
    return iterator(deque_.data(), start_);
  }
  iterator end() noexcept {
    return iterator(deque_.data(), end_);
  }
  const_iterator begin() const noexcept {
    return const_iterator(deque_.data(), start_);
  }
  const_iterator end() const noexcept {
    return const_iterator(deque_.data(), end_);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(deque_.data(), start_);
  }
  const_iterator cend() const noexcept {
    return const_iterator(deque_.data(), end_);
  }

  std::reverse_iterator<iterator> rbegin() noexcept {
    return std::reverse_iterator(end());
  }
  std::reverse_iterator<iterator> rend() noexcept {
    return std::reverse_iterator(begin());
  }
  std::reverse_iterator<const_iterator> rbegin() const noexcept {
    return std::reverse_iterator(end());
  }
  std::reverse_iterator<const_iterator> rend() const noexcept {
    return std::reverse_iterator(begin());
  }
  std::reverse_iterator<const_iterator> crbegin() const noexcept {
    return std::reverse_iterator(cend());
  }
  std::reverse_iterator<const_iterator> crend() const noexcept {
    return std::reverse_iterator(cbegin());
  }

  void push_back(const T& elem) {
    if (end_.block_ >= deque_cap_) {
      increase_cap();
    }
    if (end_.index_ == 0) {
      deque_[end_.block_].MakeChunk();
    }
    new (deque_[end_.block_].data() + end_.index_) T(elem);
    ++end_;
  }

  void push_front(const T& elem) {
    if (start_.block_ == 0 && start_.index_ == 0) {
      increase_cap();
    }
    if (start_.index_ == 0) {
      deque_[start_.block_ - 1].MakeChunk();
    }
    Index prev = start_;
    --prev;
    new (deque_[prev.block_].data() + prev.index_) T(elem);
    --start_;
  }

  void pop_back() {
    if (start_ == end_) {
      return;
    }
    --end_;
    (deque_[end_.block_].data() + end_.index_)->~T();
    if (end_.index_ == 0) {
      deque_[end_.block_].DelChunk();
    }
  }

  void pop_front() {
    if (start_ == end_) {
      return;
    }
    (deque_[start_.block_].data() + start_.index_)->~T();
    ++start_;
    if (start_.index_ == 0) {
      deque_[start_.block_ - 1].DelChunk();
    }
  }

  void erase(iterator it) {
    assert(it.source_ == deque_.data());
    Index cur = it.position_;
    Index next = it.position_ + 1;
    while (next != end_) {
      (deque_[cur.block_].data() + cur.index_)->~T();
      new (deque_[cur.block_].data() + cur.index_) T(deque_[next.block_][next.index_]);
      ++next;
      ++cur;
    }
    --end_;
    if (end_.index_ == 0) {
      deque_[end_.block_].DelChunk();
    }
  }

  void insert(iterator it, const T& elem) {
    Index save = it.position_;
    if (end_.block_ >= deque_cap_) {
      ptrdiff_t dif = save - start_;
      increase_cap();
      save = start_;
      save += dif;
    }
    if (end_.index_ == 0) {
      deque_[end_.block_].MakeChunk();
    }
    Index cur = end_;
    --cur;
    Index next = end_;
    while (next != save) {
      (deque_[next.block_].data() + next.index_)->~T();
      new (deque_[next.block_].data() + next.index_) T(deque_[cur.block_][cur.index_]);
      --cur;
      --next;
    }
    new (deque_[save.block_].data() + save.index_) T(elem);
    ++end_;
  }

 private:
  void increase_cap() {
    size_t cur_block_size = end_.block_ - start_.block_ + 1 - static_cast<size_t>(end_.index_ == 0);
    size_t new_cap = std::max(3 * (cur_block_size + 1), deque_cap_);
    std::vector<Chunk> new_deque(new_cap);
    for (size_t i = 0; i < cur_block_size; ++i) {
      new_deque[cur_block_size + 1 + i].Swap(deque_[i + start_.block_]);
      deque_[i + start_.block_].SetNull();
    }
    deque_ = new_deque;
    for (size_t i = 0; i < new_deque.size(); ++i) {
      new_deque[i].SetNull();
    }
    deque_cap_ = new_cap;
    size_t size = end_ - start_;
    start_.block_ = cur_block_size + 1;
    end_ = start_ + size;
  }
};

#endif //DEQUE_H_