# Collect client source files
file(GLOB_RECURSE CLIENT_SOURCES
    "*.cpp"
    "*.hpp"
)

# Create client executable
add_executable(netcode-client ${CLIENT_SOURCES})

# Ensure SFML is available
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

# Link libraries
target_link_libraries(netcode-client
    PRIVATE
        netcode::common
        sfml-graphics
        sfml-window
        sfml-audio
        sfml-system
)

# Include directories
target_include_directories(netcode-client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler features
target_compile_features(netcode-client
    PRIVATE
        cxx_std_17
)



# Set target properties
set_target_properties(netcode-client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX "d"
)

# Plattform-spesifikt: **IKKE** WIN32_EXECUTABLE!
# Fjern eller ikke ha med denne blokka:
# if(WIN32)
#     set_target_properties(netcode-client PROPERTIES
#         WIN32_EXECUTABLE TRUE
#     )
# endif()

# Install
install(TARGETS netcode-client
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
