cmake_minimum_required(VERSION 3.20)
project(netcode-demo
    VERSION 1.0.0
    DESCRIPTION "Multiplayer networking demonstration"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Options
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_DOCS "Build documentation" OFF)

# CRITICAL: Enable testing at ROOT level - required for VS Test Explorer
enable_testing()

# vcpkg integration
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Find SFML via vcpkg (manifest mode handles toolchain automatically)
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system network)

# Make SFML available to subdirectories
set(SFML_LIBRARIES sfml-graphics sfml-window sfml-system sfml-network)
set(SFML_INCLUDE_DIRS ${SFML_INCLUDE_DIR})

# Include custom modules (only if the file exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")
    include(CompilerWarnings)
endif()

# Optional: Find Doxygen for documentation (only if BUILD_DOCS is enabled)
if(BUILD_DOCS)
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    
    if(DOXYGEN_FOUND)
        # Create docs directory
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/api)
        
        # Configure Doxygen file
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)
        
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        # Create custom target for documentation
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
        
        message(STATUS "Doxygen found. Use 'cmake --build . --target docs' to generate documentation")
    else()
        message(WARNING "Doxygen not found. Documentation target will not be available")
    endif()
endif()

# Add subdirectories
add_subdirectory(src)

# Only add tests if BUILD_TESTS is ON
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Print build information
message(STATUS "")
message(STATUS "Netcode Demo v${PROJECT_VERSION} - Build Configuration")
message(STATUS "=================================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build docs: ${BUILD_DOCS}")
message(STATUS "")