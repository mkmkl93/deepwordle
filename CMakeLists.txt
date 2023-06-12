cmake_minimum_required(VERSION 3.18)
project(wordle)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(test src/dead_letters_tests.cpp src/dead_letters.cpp src/response.cpp)

add_executable(wordle
        src/solveInFive.cpp src/words_handler.cpp src/dead_letters.cpp src/response.cpp src/utils.h src/utils.cpp src/static_solver.cpp src/static_solver.h src/base_solver.cpp src/base_solver.h src/ctpl_stl.h src/semi_static_solver.h src/semi_static_solver.cpp)