project(tests)
add_subdirectory(googletest)
include_directories(../include ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(tests src/main.cpp src/database/test.cpp)
target_link_libraries(tests gtest gtest_main gmock gmock_main)