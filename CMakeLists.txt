cmake_minimum_required(VERSION 3.30)
project(efektywnoscAlgGrafowych)

set(CMAKE_CXX_STANDARD 20)

add_executable(efektywnoscAlgGrafowych cmake-build-debug/main.cpp
        cmake-build-debug/ListGraph.h
        cmake-build-debug/MSTSolver.h
        cmake-build-debug/Structures.h
        cmake-build-debug/MatrixGraph.h
)
