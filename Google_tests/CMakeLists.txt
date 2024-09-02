# 'Google_test' is the subproject name
project(Google_tests)

# 'lib/googletest' is the folder with Google Test sources
add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
add_executable(Google_Tests_run
        quadtree/test.cpp

        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        ../src/quadnode.cpp
        ../src/quadtree.cpp
)
target_link_libraries(Google_Tests_run gtest gtest_main)