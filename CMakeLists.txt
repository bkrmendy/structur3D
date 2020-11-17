cmake_minimum_required(VERSION 3.17)
project(structur3d_base)

set(CMAKE_CXX_STANDARD 17)

add_library(structur3d_base SHARED src/db/NodeType.cpp src/db/SQLite3Database.cpp src/document/DocumentImpl.cpp src/vm/ViewmodelImpl.cpp)
target_include_directories(structur3d_base PUBLIC thirdparty include)