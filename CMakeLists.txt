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

    # Add subdirectories
    add_subdirectory(src)

    # Only add tests if BUILD_TESTS is ON
    if(BUILD_TESTS)
        add_subdirectory(tests)
    endif()

    # Install configuration
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)