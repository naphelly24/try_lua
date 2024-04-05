//
// Created by Chen, Hailin on 2024/4/5.
//

#include "luawrapper.h"

// region: private functions
bool LuaWrapper::checkResult(int r) {
    if (r == LUA_OK)
        return true;

    std::cout << "error: " << lua_tostring(L_.get(), -1) << std::endl;
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
    // Call the function with 1 argument, and expect 1 result
    int r = lua_pcall(L_.get(), 1, 1, 0);
    if (!checkResult(r)) {
        return -1;
    }
    int result = lua_tointeger(L_.get(), -1);
    lua_pop(L_.get(), 1);
    return result;
}

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

int LuaWrapper::luaPut(lua_State *L) {
    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto* wrapper = static_cast<LuaWrapper*>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // Index -1 (or 3) is the LuaWrapper instance
    // Index -2 (or 2) is the key
    // Index -3 (or 1) is the value
    if (lua_gettop(L) != 3) {
        return luaL_error(L, "Expected 3 argument");
    }
    
    // Get key and value args
    const char* key = lua_tostring(L, 1);
    const char* value = lua_tostring(L, 2);
    
    // Call the C++ function
    wrapper->db_->put(key, value);
    
    // Return the number of results
    return 0;
}

int LuaWrapper::luaGet(lua_State* L) {
    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto* wrapper = static_cast<LuaWrapper*>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // Index -1 (or 2) is the LuaWrapper instance
    // Index -2 (or 1) is the key
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Expected 2 argument");
    }

    // Get key arg
    const char* key = lua_tostring(L, -2);

    // Call the C++ function
    std::string value = wrapper->db_->get(key);

    // Push result onto the Lua stack
    lua_pushstring(L, value.c_str());

    // Return the number of results
    return 1;
}

int LuaWrapper::luaDel(lua_State *L) {
    // Retrieve the LuaWrapper instance from the Lua state's registry
    lua_getfield(L, LUA_REGISTRYINDEX, "LuaWrapper");
    auto* wrapper = static_cast<LuaWrapper*>(lua_touserdata(L, -1));

    // Check that we have the correct number of arguments
    // Index -1 (or 2) is the LuaWrapper instance
    // Index -2 (or 1) is the key
    if (lua_gettop(L) != 2) {
        return luaL_error(L, "Expected 2 argument");
    }

    // Get key arg
    const char* key = lua_tostring(L, -2);

    // Call the C++ function
    wrapper->db_->del(key);

    // Return the number of results
    return 0;
}
