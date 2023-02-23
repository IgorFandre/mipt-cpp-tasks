#include <cmath>
#include <iostream>
#include <vector>

class SparseTable {
  struct Value {
    int32_t val;
    size_t idx;
    bool operator==(const Value& value) const {
      return (val == value.val) && (idx == value.idx);
    }
    Value GetMin(const Value& value) {
      if (val < value.val) {
        return *this;
      }
      if (val == value.val) {
        return (idx < value.idx) ? *this : value;
      }
      return value;
    }
  };
  void FillLogs() {
    logs_.resize(size_ + 1, 0);
    for (size_t i = 1; i <= size_; ++i) {
      logs_[i] = static_cast<int32_t>(log2(i));
    }
  }
  void ResizeTable() {
    table_.resize(max_pow_);
    table_[0].resize(1);
    size_t prev_minus = 1;
    for (size_t i = 1; i < max_pow_; ++i) {
      table_[i].resize(size_ - prev_minus);
      prev_minus += static_cast<int32_t>(pow(2, static_cast<double>(i)));
    }
  }
  void FillTable(const std::vector<int32_t>& array) {
    for (size_t i = 0; i < size_ - 1; ++i) {
      if (array[i] < array[i + 1]) {
        table_[1][i] = {{array[i], i}, {array[i + 1], i + 1}};
      } else {
        table_[1][i] = {{array[i + 1], i + 1}, {array[i], i}};
      }
    }
    size_t prev_minus = 1;
    for (size_t i = 1; i + 1 < max_pow_; ++i) {
      for (size_t j = 0; j + (1 << i) < size_ - prev_minus; ++j) {
        if (table_[i][j].first.val < table_[i][j + (1 << i)].first.val) {
          table_[i + 1][j] = {
              table_[i][j].first,
              table_[i][j].second.GetMin(table_[i][j + (1 << i)].first)};
        } else {
          table_[i + 1][j] = {
              table_[i][j + (1 << i)].first,
              table_[i][j].first.GetMin(table_[i][j + (1 << i)].second)};
        }
      }
      prev_minus += static_cast<int32_t>(pow(2, static_cast<double>(i)));
    }
  }

 public:
  SparseTable(const std::vector<int32_t>& array) {
    size_ = array.size();
    max_pow_ = static_cast<size_t>(ceil(log2(size_ + 1)));
    FillLogs();
    ResizeTable();
    FillTable(array);
  }

  int32_t FindSecTh(size_t l, size_t r) {  // [l, r)
    size_t pow = logs_[r - l];
    if (logs_[r - l - 1] != logs_[r - l]) {
      return table_[pow][l].second.val;
    }
    if (table_[pow][l].first == table_[pow][r - (1 << pow)].first) {
      return std::min(table_[pow][l].second.val,
                      table_[pow][r - (1 << pow)].second.val);
    }
    if (table_[pow][l].first.val == table_[pow][r - (1 << pow)].first.val) {
      return table_[pow][l].first.val;
    }
    if (table_[pow][l].first.val < table_[pow][r - (1 << pow)].first.val) {
      return std::min(table_[pow][l].second.val,
                      table_[pow][r - (1 << pow)].first.val);
    }
    return std::min(table_[pow][l].first.val,
                    table_[pow][r - (1 << pow)].second.val);
  }

 private:
  size_t size_;
  size_t max_pow_;
  std::vector<int32_t> logs_;
  std::vector<std::vector<std::pair<Value, Value>>> table_;
};

int main() {
  size_t size;
  int32_t requests;
  std::cin >> size >> requests;
  std::vector<int32_t> array(size);
  for (size_t i = 0; i < size; ++i) {
    std::cin >> array[i];
  }
  SparseTable table = SparseTable(array);
  while (requests > 0) {
    --requests;
    size_t l;
    size_t r;
    std::cin >> l >> r;
    std::cout << table.FindSecTh(l - 1, r) << "\n";
  }
}