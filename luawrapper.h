//
// Created by Chen, Hailin on 2024/4/5.
//

#ifndef TRY_LUA_LUAWRAPPER_H
#define TRY_LUA_LUAWRAPPER_H

#include "lua/lua.hpp"
#include "rocks.h"
#include <iostream>
#include <functional>

// Custom deleter for lua_State
struct LuaStateDeleter {
    void operator()(lua_State* L) const {
        lua_close(L);
    }
};

class LuaWrapper {
public:
    LuaWrapper();
    ~LuaWrapper() = default;

    int runFile(const std::string &filename);
    int callLuaFib(int n);

    // region: bridges between Lua and C++
    static int luaGet(lua_State *L);
    static int luaPut(lua_State *L);
    static int luaDel(lua_State *L);
    // endregion
private:
    bool checkResult(int r);
    
    std::unique_ptr<Rocks> db_;
    std::unique_ptr<lua_State, LuaStateDeleter> L_;
};

#endif //TRY_LUA_LUAWRAPPER_H
