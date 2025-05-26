cmake_minimum_required(VERSION 3.14)

# Enable testing
enable_testing()

# Find Catch2
find_package(Catch2 3 CONFIG REQUIRED)

# Include CTest and Catch2's CMake functions
include(CTest)
include(Catch)

# Find all test source files
file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/client/*_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common/*_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/server/*_tests.cpp
)

# Find common source files
file(GLOB COMMON_SRC
    ${CMAKE_SOURCE_DIR}/src/common/*.cpp
)

# Create the test executable
add_executable(netcode_tests
    ${TEST_SOURCES}
    ${COMMON_SRC}
    "client/client_tests.cpp"
    "server/server_tests.cpp"
)

# Set include directories
target_include_directories(netcode_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries (Catch2 always, ws2_32 only on Windows)
target_link_libraries(netcode_tests PRIVATE Catch2::Catch2WithMain)

if(WIN32)
    target_link_libraries(netcode_tests PRIVATE ws2_32)
endif()

# Set runtime output directory to help VS find the tests
set_target_properties(netcode_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# Use Catch2 test discovery
catch_discover_tests(netcode_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES
        LABELS "unit"
    TEST_PREFIX "netcode::"
    DISCOVERY_TIMEOUT 60
)
