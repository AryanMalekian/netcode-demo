cmake_minimum_required(VERSION 3.14)

# Enable testing - MUST be in root CMakeLists.txt as well
enable_testing()

# Find Catch2
find_package(Catch2 3 CONFIG REQUIRED)

# Include CTest
include(CTest)

# Include Catch2's CMake functions (for Catch2 v3+)
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
)

# Set include directories
target_include_directories(netcode_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(netcode_tests PRIVATE 
    Catch2::Catch2WithMain 
    ws2_32
)

# Set runtime output directory to help VS find the tests
set_target_properties(netcode_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# CRITICAL FIX: Use explicit test discovery with proper working directory
catch_discover_tests(netcode_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES
        LABELS "unit"
    # Fix for VS Test Explorer - specify exact working directory
    TEST_PREFIX "netcode::"
    # Ensure tests are discovered after build
    DISCOVERY_TIMEOUT 60
)

# Alternative manual test registration (fallback if catch_discover_tests fails)
# This ensures at least the main test executable shows up
add_test(NAME netcode_all_tests 
         COMMAND netcode_tests
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# Set test properties for better VS integration
set_tests_properties(netcode_all_tests PROPERTIES 
    LABELS "unit;all"
    TIMEOUT 300
)