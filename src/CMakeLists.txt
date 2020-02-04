cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCE_FILES
    RELATIVE
    ${CMAKE_CURRENT_LIST_DIR}
    "*.c"
)
list(REMOVE_ITEM source_files "main.c")

add_executable(hash-server "main.c")

if(SOURCE_FILES)
add_library(hash-server-lib STATIC ${SOURCE_FILES})
target_link_libraries(hash-server hash-server-lib)
endif()
