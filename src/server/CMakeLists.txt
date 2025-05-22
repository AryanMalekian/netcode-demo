# Collect server source files
file(GLOB_RECURSE SERVER_SOURCES
    "*.cpp"
    "*.hpp"
)

# Create server executable
add_executable(netcode-server ${SERVER_SOURCES})

# Link libraries
target_link_libraries(netcode-server
    PRIVATE
        netcode::common
        ws2_32
)

# Include directories
target_include_directories(netcode-server
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler features
target_compile_features(netcode-server
    PRIVATE
        cxx_std_17
)

# Apply warning settings
set_project_warnings(netcode-server)

# Set target properties
set_target_properties(netcode-server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX "d"
)

# Install
install(TARGETS netcode-server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)