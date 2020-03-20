cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "g++")

set(EXECUTABLE_OUTPUT_PATH bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -O3 -g3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Og -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -O3 -g0")

# set the project name and version
project(GraphDB VERSION 0.1)

include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SRCS src/*.cc)

set(sources ${SRCS})
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/main/*)
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*)
add_library(graphdb STATIC ${sources})

set(BUILD_TARGETS
    create_db
    grammar
    query
    check_bpts
)

set(TEST_TARGETS
    bplus_tree
)

foreach(target ${BUILD_TARGETS})
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/main/${target}.cc)
    target_link_libraries(${target} graphdb ssl crypto stdc++fs)
endforeach(target)

foreach(target ${TEST_TARGETS})
    add_executable(${target} ${CMAKE_SOURCE_DIR}/src/tests/${target}.cc)
    target_link_libraries(${target} graphdb ssl crypto stdc++fs)
    # add_test(NAME ${target} COMMAND ${target})
endforeach(target)