cmake_minimum_required(VERSION 3.27)
project(try_lua)

set(CMAKE_CXX_STANDARD 17)


# use cmake default search procedure. 
# This usually includes standard system paths and paths specified by the user.
find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h PATH "")
message(STATUS "ROCKSDB_INCLUDE_DIR: ${ROCKSDB_INCLUDE_DIR}")
include_directories(${ROCKSDB_INCLUDE_DIR})

find_library(ROCKSDB_LIB rocksdb)
message(STATUS "finding ${ROCKSDB_LIB}")

add_executable(try_lua main.cpp
        rocks.h
        rocks.cpp)
target_link_libraries(try_lua ${ROCKSDB_LIB} )
