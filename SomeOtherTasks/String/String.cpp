#include "String.h"

////////////////////////////////String initialization///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

String::String(const char* substr, size_t size) : size_(size),
                                          cap_(size),
                                          string_(new char[size + 1]) {
  memcpy(string_, substr, size_);
  string_[size_] = '\0';
}
String::String() : size_(0), cap_(0), string_(new char[1]) {
  string_[0] = '\0';
}
String::String(size_t n, char ch) : size_(n), cap_(n), string_(new char[n + 1]) {
  memset(string_, ch, n);
  string_[n] = '\0';
}
String::String(const char* cstr) : size_(std::strlen(cstr)),
                           cap_(size_),
                           string_(new char[size_ + 1]) {
  memcpy(string_, cstr, size_ + 1);
}
String::String(const String& str) : size_(str.size_),
                            cap_(str.cap_),
                            string_(new char[str.cap_ + 1]) {
  memcpy(string_, str.string_, size_ + 1);
}
String::~String() {
  delete[] string_;
}

////////////////////////////////String operators////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

String& String::operator=(String str) {
  swap(str);
  return *this;
}

char& String::operator[](size_t index) { return string_[index]; }
const char& String::operator[](size_t index) const { return string_[index]; }

String& String::operator+=(const String& str) {
  if (cap_ < size_ + str.size_) {
    increase_capacity(std::max(size_ + str.size_, 2 * cap_));
  }
  memcpy(string_ + size_, str.string_, str.size_ + 1);
  size_ += str.size_;
  return *this;
}
String& String::operator+=(char ch) {
  this->push_back(ch);
  return *this;
}
String operator+(String str, char ch) {
  str.push_back(ch);
  return str;
}
String operator+(char ch, String str) {
  str.size_ += 1;
  str.cap_ = str.size_;
  char* new_str = new char[str.size_ + 1];
  new_str[0] = ch;
  memcpy(new_str + 1, str.string_, str.size_);
  delete[] str.string_;
  str.string_ = new_str;
  return str;
}
String operator+(String str1, const String& str2) {
  str1 += str2;
  return str1;
}
bool operator==(const String& str1, const String& str2) {
  if (str1.size_ != str2.size_) {
    return false;
  }
  for (size_t i = 0; i < str1.size_; ++i) {
    if (str1.string_[i] != str2.string_[i]) {
      return false;
    }
  }
  return true;
}

bool operator==(const String& str, const char* cstr) {
  if (str.size_ != std::strlen(cstr)) {
    return false;
  }
  for (size_t i = 0; i < str.size_; ++i) {
    if (cstr[i] != str.string_[i]) {
      return false;
    }
  }
  return true;
}
bool operator==(const char* cstr, const String& str) {
  return str == cstr;
}
bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}
bool operator<(const String& str1, const String& str2) {
  size_t last_i = std::min(str1.size_, str2.size_);
  for (size_t i = 0; i < last_i; ++i) {
    if (str1.string_[i] >= str2.string_[i]) {
      return false;
    }
  }
  if (str1.size_ < str2.size_) {
    return true;
  }
  return false;
}
bool operator<=(const String& str1, const String& str2) {
  return !(str2 < str1);
}
bool operator>(const String& str1, const String& str2) {
  return str2 < str1;
}
bool operator>=(const String& str1, const String& str2) {
  return !(str1 < str2);
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  out << str.string_;
  return out;
}
std::istream& operator>>(std::istream& in, String& str) {
  str.clear();
  char symbol;
  while (in.get(symbol)) {
    if (!std::isspace(symbol)) {
      str.push_back(symbol);
    } else {
      break;
    }
  }
  return in;
}

////////////////////////////////String methods//////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void String::increase_capacity(size_t new_cap = 0) {
  cap_ = (new_cap > 0) ? new_cap : 2 * cap_;
  char* new_str = new char[cap_ + 1];
  memcpy(new_str, string_, size_ + 1);
  delete[] string_;
  string_ = new_str;
}

String String::substr(size_t start, size_t count) const {
  return {string_ + start, count};
}

char& String::front() { return string_[0]; }
const char& String::front() const { return string_[0]; }
char& String::back() { return string_[size_ - 1]; }
const char& String::back() const { return string_[size_ - 1]; }
char* String::data() { return string_; }
const char* String::data() const { return string_; }
size_t String::size() const { return size_; }
size_t String::capacity() const { return cap_; }

void String::swap(String& str) {
  std::swap(str.string_, string_);
  std::swap(str.size_, size_);
  std::swap(str.cap_, cap_);
}
size_t String::find(const String& substr) const {
  if (substr.size_ > size_) {
    return size_;
  }
  for (size_t i = 0; i <= size_ - substr.size_; ++i) {
    if (string_[i] == substr.string_[0]) {
      bool found = true;
      for (size_t j = 1; j < substr.size_; ++j) {
        if (string_[j + i] != substr.string_[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        return i;
      }
    }
  }
  return size_;
}
size_t String::rfind(const String& substr) const {
  if (substr.size_ > size_) {
    return size_;
  }
  for (size_t k = 0; k <= size_ - substr.size_; ++k) {
    size_t i = size_ - substr.size_ - k;
    if (string_[i] == substr.string_[0]) {
      bool found = true;
      for (size_t j = 1; j < substr.size_; ++j) {
        if (string_[j + i] != substr.string_[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        return i;
      }
    }
  }
  return size_;
}
size_t String::length() const {
  return size_;
}
void String::push_back(char letter) {
  if (size_ == cap_) {
    increase_capacity(std::max(cap_ + 2, 2 * cap_));
  }
  string_[size_++] = letter;
  string_[size_] = '\0';
}
void String::pop_back() {
  if (size_ != 0)
    string_[--size_] = '\0';
}
void String::shrink_to_fit() {
  if (cap_ == size_)
    return;
  char* new_str = new char[size_ + 1];
  memcpy(new_str, string_, size_ + 1);
  delete[] string_;
  string_ = new_str;
  cap_ = size_;
}
bool String::empty() const{
  return size_ == 0;
}
void String::clear() {
  string_[0] = '\0';
  size_ = 0;
}
std::vector<String> String::split(const String& delim) {
  std::vector<String> res;
  if (delim.size_ > size_) {
    return res;
  }
  size_t start = 0;
  for (size_t k = 0; k <= size_ - delim.size_; ++k) {
    size_t idx = size_ - delim.size_ - k;
    if (string_[idx] == delim.string_[0]) {
      bool found = true;
      for (size_t j = 1; j < delim.size_; ++j) {
        if (string_[j + idx] != delim.string_[j]) {
          found = false;
          break;
        }
      }
      if (found) {
        res.push_back(String(string_ + start, k - start));
        start = k + delim.size_;
      }
    }
  }
  if (start < size_) {
    res.push_back(String(string_ + start, size_ - start));
  }
  return res;
}
String String::join(const std::vector<String>& strings) const {
  if (strings.empty()) {
    return {};
  }
  String res = strings[0];
  for (size_t i = 1; i < strings.size(); ++i) {
    res += *this;
    res += strings[i];
  }
  return res;
}