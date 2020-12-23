cmake_minimum_required(VERSION 3.17)
project(league_shopper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_executable(league_shopper main.cpp calc.cpp cli.cpp game.cpp items.cpp)
