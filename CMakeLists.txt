cmake_minimum_required(VERSION 3.24)
project(wordle)

set(CMAKE_CXX_STANDARD 20)

add_executable(test dead_letters_tests.cpp dead_letters.cpp response.cpp)

add_executable(wordle
        solveInFive.cpp words_handler.cpp dead_letters.cpp response.cpp)