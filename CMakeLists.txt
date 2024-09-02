cmake_minimum_required(VERSION 3.29)
project(eda)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(eda
    include/particle.h
    include/point.h
    include/quadnode.h
    include/rect.h
    src/particle.cpp
    src/point.cpp
    src/rect.cpp src/test.cpp
        src/quadnode.cpp
        include/quadtree.cpp
        include/quadtree.h
        src/quadtree.cpp)
