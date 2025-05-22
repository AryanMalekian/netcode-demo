# Collect common source files
file(GLOB_RECURSE COMMON_SOURCES
    "*.cpp"
    "*.hpp"
)

# Create common library
add_library(netcode-common STATIC ${COMMON_SOURCES})
add_library(netcode::common ALIAS netcode-common)

# Include directories
target_include_directories(netcode-common
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(netcode-common
    PUBLIC
        sfml-system
        sfml-network
    PRIVATE
        # Private dependencies here
)

# Compiler features
target_compile_features(netcode-common
    PUBLIC
        cxx_std_17
)

# Apply warning settings
set_project_warnings(netcode-common)

# Set target properties
set_target_properties(netcode-common PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "netcode-common"
    DEBUG_POSTFIX "d"
)