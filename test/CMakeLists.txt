project(tests)
add_subdirectory(googletest)
include_directories(../include ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(tests src/main.cpp src/database/test.cpp src/data/tests.cpp)
target_link_libraries(tests structur3d_base gtest gtest_main gmock gmock_main)