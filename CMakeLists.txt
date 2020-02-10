cmake_minimum_required(VERSION 3.10)

project(hash-server LANGUAGES C)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

# Build dependencies
#
add_subdirectory(external/openssl-gost)

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

find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories(${OPENSSL_INCLUDE_DIR})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

if(BUILD_TESTS)
    enable_testing()

    add_subdirectory("external/Unity")
    add_subdirectory("test")
endif()

