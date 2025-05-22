# Only build tests if requested
if(NOT BUILD_TESTS)
    return()
endif()

# Function to create test executable
function(add_test_executable name)
    # Collect test sources
    file(GLOB_RECURSE TEST_SOURCES "${name}/*.cpp")
    
    if(TEST_SOURCES)
        # Create test executable
        add_executable(test-${name} ${TEST_SOURCES})
        
        # Link libraries
        target_link_libraries(test-${name}
            PRIVATE
                netcode::common
                Catch2::Catch2WithMain
        )
        
        # Include directories
        target_include_directories(test-${name}
            PRIVATE
                ${CMAKE_SOURCE_DIR}/src/${name}
        )
        
        # Set properties
        set_target_properties(test-${name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
        
        # Register with CTest
        include(CTest)
        include(Catch)
        catch_discover_tests(test-${name})
    endif()
endfunction()

# Add test executables
add_test_executable(common)
add_test_executable(client)
add_test_executable(server)