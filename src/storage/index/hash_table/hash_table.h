#ifndef STORAGE__HASH_TABLE_H_
#define STORAGE__HASH_TABLE_H_

#include <cstdint>
#include <string>
#include <map>

#include "storage/file_id.h"

struct Hash128 {
    Hash128(uint64_t hash_1, uint64_t  hash_2) :
      hash_1(hash_1), hash_2(hash_2) { }

    uint64_t hash_1;
    uint64_t hash_2;

    bool operator<(const Hash128 other) const {
        if (hash_1 < other.hash_1) {
            return true;
        } else {
            return hash_2 < other.hash_2;
        }
    }
};

class HashTable {
public:
    HashTable(const std::string& filename);
    ~HashTable();

    uint64_t get_id(uint64_t hash_1, uint64_t hash_2);
    void create_id(uint64_t hash_1, uint64_t hash_2, uint64_t id);

private:
    FileId file_id;
    std::map<Hash128, uint64_t> map;
};

#endif // STORAGE__HASH_TABLE_H_
