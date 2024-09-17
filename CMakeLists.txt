cmake_minimum_required(VERSION 3.29)
project(eda)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(eda
        include/particle.h
        include/point.h
        include/quadnode.h
        include/rect.h
        include/quadtree.h
        include/datatype.h

        src/particle.cpp
        src/point.cpp
        src/rect.cpp
        src/quadnode.cpp
        src/quadtree.cpp
        src/test.cpp
        Google_tests/quadtree/test.cpp
        include/line.h
        include/plane.h
        include/bsptree.h
        src/plane.cpp
        src/bsptree.cpp
        src/datatype.cpp
        src/datatype.cpp
)

add_executable(bsptree_prof_test
        src/particle.cpp
        src/point.cpp
        src/rect.cpp
        src/quadnode.cpp
        src/quadtree.cpp
        src/plane.cpp
        src/bsptree.cpp
        src/datatype.cpp
        test/main.cpp
)

add_subdirectory(Google_tests)