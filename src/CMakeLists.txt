cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SRC_FILES
    RELATIVE
    ${CMAKE_CURRENT_LIST_DIR}
    "*.c"
)
list(REMOVE_ITEM SRC_FILES "main.c")

add_executable(hash-server "main.c")
target_link_libraries(hash-server OpenSSL::Crypto)
target_link_libraries(hash-server gost)

if(SRC_FILES)
add_library(hash-server_lib STATIC ${SRC_FILES})
target_include_directories(hash-server_lib
    PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}"
)
target_link_libraries(hash-server hash-server_lib)
endif()
