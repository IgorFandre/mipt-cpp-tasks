#include <iostream>
#include <optional>
#include <vector>

void Merge(std::vector<std::pair<size_t, size_t>>& pairs, size_t i_1, size_t i_2, size_t i_3) {
  size_t a = i_1;
  size_t b = i_2;
  std::vector<std::pair<size_t, size_t>> s_pairs(i_3 - i_1);
  size_t index = 0;
  while (a < i_2 && b < i_3) {
    if ((pairs[a].first < pairs[b].first) ||
        (pairs[a].first == pairs[b].first && pairs[a].second > pairs[b].second))
    {
      s_pairs[index] = pairs[a];
      ++a;
    } else {
      s_pairs[index] = pairs[b];
      ++b;
    }
    ++index;
  }
  while (a < i_2) {
    s_pairs[index] = pairs[a];
    ++a;
    ++index;
  }
  while (b < i_3) {
    s_pairs[index] = pairs[b];
    ++b;
    ++index;
  }
  for (size_t i = 0; i < i_3 - i_1; ++i) {
    pairs[i_1 + i] = s_pairs[i];
  }
}

void MergeSort(std::vector<std::pair<size_t, size_t>>& pairs, size_t start, size_t finish) {
  if (finish - start == 1) {
    return;
  }
  int middle = (finish + start) / 2;
  MergeSort(pairs, start, middle);
  MergeSort(pairs, middle, finish);
  Merge(pairs, start, middle, finish);
}


int main() {
  size_t size;
  std::cin >> size;
  std::vector<std::pair<size_t, size_t>> pairs(size);
  for (size_t i = 0; i < size; ++i) {
    std::cin >> pairs[i].first >> pairs[i].second;
  }
  MergeSort(pairs, 0, size);
  size_t result = 0;
  for (size_t i = 0; i < size - 1; ++i) {
    for (size_t j = i + 1; j < size; ++j) {
      if (pairs[i].second > pairs[j].second) {
        ++result;
      } else if (pairs[i].second == pairs[j].second && pairs[i].first < pairs[j].first) {
        ++result;
      }
    }
  }
  std::cout << result << "\n";
}