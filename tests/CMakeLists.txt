cmake_minimum_required(VERSION 3.14)
project(NetcodeTests)

enable_testing()

find_package(Catch2 CONFIG REQUIRED)
include(CTest)
include(Catch)

file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/client/*_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/*_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/server/*_tests.cpp
)

file(GLOB COMMON_SRC
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
)

add_executable(netcode_tests
    ${TEST_SOURCES}
    ${COMMON_SRC}
)

target_include_directories(netcode_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(netcode_tests PRIVATE Catch2::Catch2WithMain ws2_32)

catch_discover_tests(netcode_tests)
