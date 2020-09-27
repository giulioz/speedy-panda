cmake_minimum_required(VERSION 3.10)
project(speedy-panda)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall --pedantic -O3 -g -finline-functions -march=native")
#set(CMAKE_CXX_FLAGS "-Wall --pedantic -g -finline-functions -march=native")

add_executable(speedy-panda src/main.cc src/panda.cc)
