cmake_minimum_required(VERSION 3.17)
project(structur3d_base)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE five_src src/five/*.cpp)

file(GLOB sqlpp11_connector_sqlite_src
        src/sqlpp11-connector-sqlite/*
        src/sqlpp11-connector-sqlite/detail/*)

file(GLOB_RECURSE bitsery_src src/bitsery/*.h)

add_library(structur3d_base SHARED
        ${five_src}
        ${sqlpp11_connector_sqlite_src}
        ${bitsery_src}

        src/version.c

        src/db/NodeType.cpp
        src/db/DatabaseImpl.cpp
        src/db/db_utils.cpp
        src/db/attribute_priority.cpp

        src/document/DocumentImpl.cpp

        src/vm/ViewmodelImpl.cpp

        src/data/Timestamp.cpp
        src/data/Graph.cpp
        src/data/Radius.cpp
        src/data/Coord.cpp
        src/data/Name.cpp

        src/base/SetOperationType.cpp
        src/base/Radius.cpp

        src/wire/ClientImpl.cpp

        src/mesh/MeshFactory.cpp

        src/utils/random_string.cpp

        src/utils/approximately_equal.cpp
        src/interactor/ModularInteractor.cpp

        src/wire/ServerController.cpp
        )

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

enable_testing()
add_subdirectory(test)