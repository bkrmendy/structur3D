project(tests)

add_subdirectory(thirdparty/googletest)
include_directories(../include ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(thirdparty/rapidcheck)
add_subdirectory(thirdparty/rapidcheck/extras/gtest)

add_executable(tests src/main.cpp src/database/units.cpp src/data/setOperationTypeTests.cpp src/vm/interaction_through_document.cpp src/document/documentTests.cpp src/data/graphTests.cpp src/database/properties.cpp src/wire/unit_client.cpp src/wire/unit_server.cpp src/serialization/property_serialization.cpp src/serialization/unit_serialization.cpp src/database/unit_columndb.cpp)
target_link_libraries(tests structur3d_base gtest gtest_main gmock gmock_main rapidcheck rapidcheck_gtest)