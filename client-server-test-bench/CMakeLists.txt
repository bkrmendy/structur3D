cmake_minimum_required(VERSION 3.17)

add_executable(toy_server server.cpp)
add_executable(toy_client client.cpp)

include_directories(toy_server ../thirdparty/boost)
include_directories(toy_client ../thirdparty/boost)