//
// Created by Chen, Hailin on 2024/4/4.
//
#include "rocks.h"

Rocks::Rocks(const rocksdb::Options &options, const std::string &path) {
    rocksdb::Status status = rocksdb::DB::Open(options, path, &db_);
    std::cout << "create and open rocksdb, status: " << status.code() << std::endl;
    if (!status.ok()) {
        throw std::runtime_error("Failed to open rocksdb");
    }
}

Rocks::~Rocks() {
    delete db_;
    // set to nullptr, if you accidentally delete it again, 
    // nothing will happen because deleting a null pointer is a no-op in C++.
    db_ = nullptr;
}

void Rocks::put(const std::string &key, const std::string &value) {
    db_->Put(rocksdb::WriteOptions(), key, value);
}

std::string Rocks::get(const std::string &key) {
    std::string value;
    db_->Get(rocksdb::ReadOptions(), key, &value);
    return value;
}

void Rocks::del(const std::string &key) {
    db_->Delete(rocksdb::WriteOptions(), key);
}


