# 'Google_test' is the subproject name
project(Google_tests)

# 'lib/googletest' is the folder with Google Test sources
add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
add_executable(quadtree_test
        quadtree/test.cpp

        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        ../src/quadnode.cpp
        ../src/quadtree.cpp
)

add_executable(bsptree_test
        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        ../src/quadnode.cpp
        ../src/quadtree.cpp
        bsptree/test.cpp
)
target_link_libraries(quadtree_test gtest gtest_main)
target_link_libraries(bsptree_test gtest gtest_main)