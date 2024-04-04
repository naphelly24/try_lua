#include <iostream>
#include "rocks.h"


int main() {
    rocksdb::Options options;
    options.create_if_missing = true;
    Rocks db(options, "./rocksdb");

    std::string k = "key1";
    std::string v = "value1";
    db.put(k, v);
    std::string getV = db.get(k);
    std::cout << "after put, key: " << k <<  ", value: " << getV << std::endl;
    
    db.del(k);

    getV = db.get(k);
    std::cout << "after delete, key: " << k <<  ", value: " << getV << std::endl;
    

    return 0;
}
