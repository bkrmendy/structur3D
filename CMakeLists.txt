cmake_minimum_required(VERSION 3.17)
project(structur3d_base)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE five_src src/five/*.cpp)

file(GLOB sqlpp11_connector_sqlite_src
        src/sqlpp11-connector-sqlite/*
        src/sqlpp11-connector-sqlite/detail/*)

add_library(structur3d_base SHARED
        ${five_src}
        ${sqlpp11_connector_sqlite_src}

        src/version.c

        src/db/NodeType.cpp
        src/db/DatabaseImpl.cpp

        src/document/DocumentImpl.cpp

        src/vm/ViewmodelImpl.cpp

        src/mesh/Mesh.cpp

        src/data/Graph.cpp src/base/SetOperationType.cpp)

find_package(PNG)
set(LIBS ${PNG_LIBRARIES})

target_include_directories(structur3d_base PUBLIC
        ${PNG_INCLUDE_DIR}
        thirdparty
        include
        /usr/local/include
        /usr/local/include/boost
        /usr/local/include/eigen3)

link_directories(/usr/local/lib/)
target_link_libraries(structur3d_base LINK_PUBLIC sqlite3 ${LIBS})

option(BUILD_TESTS "Build test suite" ON)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)