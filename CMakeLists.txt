cmake_minimum_required(VERSION 3.10)

project(hash-server LANGUAGES C)

# Build dependencies
#
#add_subdirectory(external/...)

if (BUILD_TESTS)
    add_definitions(-DUNIT_TEST=1)
endif()

add_subdirectory("src")
set_target_properties(hash-server
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

if(BUILD_TESTS)
    enable_testing()

    add_subdirectory("external/Unity")
    add_subdirectory("test")
endif()

