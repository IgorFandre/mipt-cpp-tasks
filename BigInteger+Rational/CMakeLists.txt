cmake_minimum_required(VERSION 3.21)
project(Tests_BigInt_Rational)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(./BigInt_Rational)
add_executable(test test.cpp)

target_link_libraries(test PUBLIC biginteger rational)