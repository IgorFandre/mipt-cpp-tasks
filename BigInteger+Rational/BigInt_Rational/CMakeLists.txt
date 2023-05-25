cmake_minimum_required(VERSION 3.21)
project(BigInt_Rational)

set(CMAKE_CXX_STANDARD 17)

add_library(biginteger SHARED
        biginteger.cpp
)

add_library(rational SHARED
        biginteger.cpp
        rational.cpp
)
