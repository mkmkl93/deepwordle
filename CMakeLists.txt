cmake_minimum_required(VERSION 3.18)
project(wordle)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(test code/dead_letters_tests.cpp code/dead_letters.cpp code/response.cpp)

add_executable(wordle
        code/solveInFive.cpp code/words_handler.cpp code/dead_letters.cpp code/response.cpp code/utils.cpp code/deep_solver.cpp code/xen_solver.cpp code/xen_solver.h code/greedy_solver.cpp code/greedy_solver.h code/static_solver.cpp code/static_solver.h code/base_solver.cpp code/base_solver.h code/ctpl_stl.h)