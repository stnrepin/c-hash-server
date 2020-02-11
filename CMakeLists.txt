cmake_minimum_required(VERSION 3.10)

project(hash-server LANGUAGES C)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

# Build dependencies
#
add_subdirectory("external/openssl-gost")
# Gost CMake doen't add include directory like, e.g., Unity.
target_include_directories(gost PUBLIC "external/openssl-gost")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

if (BUILD_TESTS)
    add_definitions(-DUNIT_TEST=1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99 -Werror -Wall")

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
