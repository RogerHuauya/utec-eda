# 'Google_test' is the subproject name
project(Google_tests)

# 'lib/googletest' is the folder with Google Test sources
add_subdirectory(lib/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Use FetchContent to get Eigen
include(FetchContent)
FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(Eigen)

# Add Eigen to the include directories
include_directories(${Eigen_SOURCE_DIR})


# 'Google_Tests_run' is the target name
add_executable(quadtree_test
        quadtree/test.cpp

        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        ../src/quadnode.cpp
        ../src/quadtree.cpp
        ../src/datatype.cpp
)

add_executable(bsptree_test
        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        ../src/quadnode.cpp
        ../src/quadtree.cpp
        bsptree/test.cpp
        ../src/plane.cpp
        ../src/bsptree.cpp
        ../src/datatype.cpp
)

add_executable(sstree_test
        ../src/particle.cpp
        ../src/point.cpp
        ../src/rect.cpp
        sstree/test.cpp
        ../src/sstree.cpp
        ../src/datatype.cpp
)

target_link_libraries(quadtree_test PRIVATE Eigen3::Eigen gtest gtest_main)
target_link_libraries(bsptree_test PRIVATE Eigen3::Eigen gtest gtest_main)
target_link_libraries(sstree_test PRIVATE Eigen3::Eigen gtest gtest_main)
