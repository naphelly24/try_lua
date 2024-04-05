cmake_minimum_required(VERSION 3.27)
project(try_lua)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)




# use cmake default search procedure. 
# This usually includes standard system paths and paths specified by the user.
find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h PATH "")
message(STATUS "ROCKSDB_INCLUDE_DIR: ${ROCKSDB_INCLUDE_DIR}")

find_path(LUA_INCLUDE_DIR lua/lua.h PATH "")
message(STATUS "LUA_INCLUDE_DIR: ${LUA_INCLUDE_DIR}")

include_directories(${ROCKSDB_INCLUDE_DIR} ${LUA_INCLUDE_DIR})


set(SPDLOG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog/include)
include_directories(${SPDLOG_DIR})

find_library(ROCKSDB_LIB rocksdb)
message(STATUS "finding ${ROCKSDB_LIB}")

find_library(LUA_LIB lua)
message(STATUS "finding ${LUA_LIB}")

add_executable(try_lua main.cpp
        rocks.cpp
        luawrapper.cpp)
target_link_libraries(try_lua ${ROCKSDB_LIB} ${LUA_LIB})

add_custom_command(TARGET try_lua POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/scripts.lua $<TARGET_FILE_DIR:try_lua>
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/defines.lua $<TARGET_FILE_DIR:try_lua>)

