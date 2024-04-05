#include <iostream>
#include "luawrapper.h"
#include <unistd.h>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>


void loopDbOpsInC() {
    rocksdb::Options options;
    options.create_if_missing = true;
    Rocks db(options, "./rocksdb");

    std::ios::sync_with_stdio(false);

    for (int i = 0; i < 100000; ++i) {
        std::string k = "k1";
        std::string v = "v1";

        spdlog::debug("put k/v to db, key: k1, value: v1");
        db.put(k, v);
        spdlog::debug("-------");

        spdlog::debug("get value from db, key: k1");
        std::string a = db.get(k);
        spdlog::debug("value in db for key k1 is: {}", a);
        spdlog::debug("-------");

        spdlog::debug("delete key from db, key: k1");
        db.del(k);
        spdlog::debug("-------");

        spdlog::debug("get value from db, key: k1");
        a = db.get(k);
        spdlog::debug("value in db for key k1 is: {}", a);
        spdlog::debug("=======");
    }
}

void loopDbOpsInLua() {
    LuaWrapper luaWrapper;

    for (int i = 0; i < 100000; ++i) {
        luaWrapper.runFile("scripts.lua");
    }
}

void perf() {
    // Measure the time taken by loopGetInC
    auto start = std::chrono::high_resolution_clock::now();
    loopDbOpsInC();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    spdlog::info("Time taken by loopGetInC: {} milliseconds", diff1.count());
    spdlog::info("-------");

    // Measure the time taken by loopGetInLua
    start = std::chrono::high_resolution_clock::now();
    loopDbOpsInLua();
    end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    spdlog::info("Time taken by loopGetInLua: {} milliseconds", diff2.count());
    spdlog::info("-------");

    spdlog::info("loopGetInC is {} times faster than loopGetInLua", diff2.count() / diff1.count());
}

void showCurWorkingDir() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        spdlog::info("Current working directory: {}", cwd);
    } else {
        spdlog::error("getcwd() error");
    }
}

void runFile() {
    showCurWorkingDir();

    LuaWrapper luaWrapper;
    // run lua script, which will call C++ function
    luaWrapper.runFile("scripts.lua");
}

void callLuaInC() {
    showCurWorkingDir();

    LuaWrapper luaWrapper;
    // call lua function
    int res = luaWrapper.callLuaFib(10);
    spdlog::info("fib(10) = {}", res);
}

/**
 * async logger with rotating file sink and stdout sink
 */
void initLog() {
    spdlog::init_thread_pool(8192, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("try_lua.log", 1024 * 1024 * 10, 3);
    std::vector<spdlog::sink_ptr> sinks{stdout_sink, rotating_sink};
    auto logger = std::make_shared<spdlog::async_logger>("mylogger", sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::set_level(spdlog::level::debug);
}

int main() {
    initLog();

//    runFile();
    callLuaInC();
//    perf();
    return 0;
}
