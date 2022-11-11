cmake_minimum_required(VERSION 3.20)
project(inventory)

set(CMAKE_CXX_STANDARD 14)

add_executable(inventory src/main.cpp src/Item.cpp src/Items.cpp src/get_quote.cpp src/Result.cpp src/core.cpp)
