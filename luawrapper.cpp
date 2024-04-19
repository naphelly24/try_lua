//
// Created by Chen, Hailin on 2024/4/5.
//

#include "luawrapper.h"
#include <spdlog/spdlog.h>

LuaWrapper::LuaWrapper() {
    rocksdb::Options options;
    options.create_if_missing = true;
    db_ = std::make_unique<Rocks>(options, "./rocksdb");

    L_ = std::unique_ptr<lua_State, LuaStateDeleter>(luaL_newstate());
    luaL_openlibs(L_.get());

    // Store a pointer to this LuaWrapper instance in the Lua state's registry
    lua_pushlightuserdata(L_.get(), this);
    lua_setfield(L_.get(), LUA_REGISTRYINDEX, "LuaWrapper");

    // Load pre-defined Lua file
    if (!checkResult(luaL_dofile(L_.get(), "defines.lua"))) {
        throw std::runtime_error("Failed to load defines.lua");
    }

    // Register the function to Lua
    lua_register(L_.get(), "GET", LuaWrapper::luaGet);
    lua_register(L_.get(), "PUT", LuaWrapper::luaPut);
    lua_register(L_.get(), "DEL", LuaWrapper::luaDel);
}

// region: private functions
bool LuaWrapper::checkResult(int r) {
    if (r == LUA_OK)
        return true;

    spdlog::error("error: {}", lua_tostring(L_.get(), -1));
    return false;
}
// endregion

int LuaWrapper::runFile(const std::string &filename) {
    int r = luaL_dofile(L_.get(), filename.c_str());
    checkResult(r);
    return r;
}

int LuaWrapper::callLuaFib(int n) {
    // Load the function, and push it onto the stack
    lua_getglobal(L_.get(), "fib");
    // Push the argument onto the stack
    lua_pushinteger(L_.get(), n);
    
    printStack(L_.get(), "Before pcall");
    
    // Call the function with 1 argument, and expect 1 result
    // first 1: number of arguments
    // second 1: number of results
    // 0: error handler
    int r = lua_pcall(L_.get(), 1, 1, 0);
    if (!checkResult(r)) {
        return -1;
    }
    
    printStack(L_.get(), "After pcall");
    
    int result = lua_tointeger(L_.get(), -1);
    lua_pop(L_.get(), 1);
    return result;
}

void LuaWrapper::printStack(lua_State *L, const std::string& prefix) {
    int top = lua_gettop(L);
    spdlog::debug("{}, lua stack size: {}", prefix, top);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch (t) {
            case LUA_TSTRING:
                spdlog::debug ("{}: {}", i, lua_tostring(L, i));
                break;
            case LUA_TBOOLEAN:
                spdlog::debug("{}: {}", i, lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                spdlog::debug("{}: {}", i, lua_tonumber(L, i));
                break;
            default:
                spdlog::debug("{}: {}", i, lua_typename(L, t));
                break;
        }
    }
}


int LuaWrapper::luaPut(lua_State *L) {
    // 2 in stack
    printStack(L);

    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto *wrapper = static_cast<LuaWrapper *>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // | -1 | 3 | LuaWrapper instance |
    // | -2 | 2 | value |
    // | -3 | 1 | key |
    if (lua_gettop(L) != 3) {
        return luaL_error(L, "Expected 3 argument");
    }

    // 3 in stack
    printStack(L);

    // Get key and value args
    const char *key = lua_tostring(L, 1);
    const char *value = lua_tostring(L, 2);

    // Call the C++ function
    wrapper->db_->put(key, value);

    // pop the LuaWrapper instance
    lua_pop(L, 1);

    // 2 in stack, lua engine will take care of the method parameters in the stack, a.k.a: key, value
    printStack(L);

    // Return the number of results
    return 0;
}

int LuaWrapper::luaGet(lua_State *L) {
    // 1 in stack: key
    printStack(L);
    
    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto *wrapper = static_cast<LuaWrapper *>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // | -1 | 2 | LuaWrapper instance |
    // | -2 | 1 | key |
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Expected 2 argument");
    }

    // 2 in stack: key and LuaWrapper instance
    printStack(L);

    // Get key arg
    const char *key = lua_tostring(L, -2);

    // Call the C++ function
    std::string value = wrapper->db_->get(key);


    // pop the LuaWrapper instance
    lua_pop(L, 1);

    // Push result onto the Lua stack
    lua_pushstring(L, value.c_str());

    // 2 in stack: key and value (method result)
    printStack(L);

    // Return the number of results
    return 1;
}


int LuaWrapper::luaDel(lua_State *L) {
    // 1 in stack: key
    printStack(L);
    
    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto *wrapper = static_cast<LuaWrapper *>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // | -1 | 2 | LuaWrapper instance |
    // | -2 | 1 | key |
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Expected 2 argument");
    }

    // 2 in stack: key and LuaWrapper instance
    printStack(L);

    // Get key arg
    const char *key = lua_tostring(L, -2);

    // Call the C++ function
    wrapper->db_->del(key);

    // pop the LuaWrapper instance
    lua_pop(L, 1);

    // 1 in stack: key
    printStack(L);

    // Return the number of results
    return 0;
}
