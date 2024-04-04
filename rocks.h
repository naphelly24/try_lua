//
// Created by Chen, Hailin on 2024/4/4.
//

#ifndef TRY_LUA_ROCKS_H
#define TRY_LUA_ROCKS_H

#include <iostream>
#include "rocksdb/db.h"

class Rocks {
public:
    Rocks(const rocksdb::Options &options, const std::string &path);

    ~Rocks();

    void put(const std::string &key, const std::string &value);

    std::string get(const std::string &key);

    void del(const std::string &key);

private:
    rocksdb::DB* db_ = nullptr;
    
};

#endif //TRY_LUA_ROCKS_H
