cmake_minimum_required(VERSION 3.31)
project(gene01)

set(CMAKE_CXX_STANDARD 23)

add_executable(gene01
        main.cpp
        de.cpp
        pso.cpp
        ga.cpp
        eda.cpp
        aco.cpp)

target_compile_options(gene01 PUBLIC -march=native)