#ifndef STRING_H_CLASS
#define STRING_H_CLASS

#include <cstring>
#include <iostream>
#include <vector>

class String {
  size_t size_;
  size_t cap_;
  char* string_;

  friend bool operator==(const String&, const char*);
  friend bool operator==(const String&, const String&);
  friend bool operator<(const String&, const String&);
  friend std::ostream& operator<<(std::ostream&, const String&);
  friend std::istream& operator>>(std::istream&, String&);
  friend String operator+(char, String);

  void increase_capacity(size_t);
  String(const char*, size_t);

 public:
  String();
  String(size_t, char);
  String(const char*);
  String(const String&);
  ~String();
  String substr(size_t start, size_t count) const;

  String& operator=(String);
  char& operator[](size_t);
  const char& operator[](size_t) const;

  char& front();
  const char& front() const;
  char& back();
  const char& back() const;
  char* data();
  const char* data() const;
  size_t size() const;
  size_t capacity() const;

  String& operator+=(const String&);
  String& operator+=(char);

  void swap(String&);
  size_t find(const String&) const;
  size_t rfind(const String&) const;
  size_t length() const;
  void push_back(char);
  void pop_back();
  void shrink_to_fit();
  bool empty() const;
  void clear();
  std::vector<String> split(const String& delim = " ");
  String join(const std::vector<String>& strings) const;
};

String operator+(String, char);
String operator+(char ch, String);
String operator+(String, const String&);

bool operator==(const String&, const String&);
bool operator<=(const String&, const String&);
bool operator>(const String&, const String&);
bool operator>=(const String&, const String&);

#endif

