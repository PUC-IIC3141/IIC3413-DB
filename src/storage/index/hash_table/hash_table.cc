#include "hash_table.h"

#include <iostream>

#include "base/ids/object_id.h"
#include "storage/file_manager.h"

using namespace std;

HashTable::HashTable(const std::string& filename) :
    file_id(file_manager.get_file_id(filename))
{
    auto& file = file_manager.get_file(file_id);
    file.seekg (0, file.end);
    // If the file is empty, read the values
    if (file.tellg() != 0) {
        // read tuples
        uint32_t length;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));
        cout << "HashTable size: " << length << "\n";

        for (uint32_t i = 0; i < length; i++) {
            uint64_t hash_1, hash_2, id;
            file.read(reinterpret_cast<char*>(&hash_1), sizeof(hash_1));
            file.read(reinterpret_cast<char*>(&hash_2), sizeof(hash_2));
            file.read(reinterpret_cast<char*>(&id), sizeof(id));

            map.insert({ Hash128(hash_1, hash_2), id });
        }
    }
}


HashTable::~HashTable() {
    auto& file = file_manager.get_file(file_id);
    file.seekg (0, file.beg);

    uint32_t length = map.size();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length));

    for (auto&& [hash_128, id] : map) {
        file.write(reinterpret_cast<const char*>(&hash_128.hash_1), sizeof(length));
        file.write(reinterpret_cast<const char*>(&hash_128.hash_2), sizeof(length));
        file.write(reinterpret_cast<const char*>(&id), sizeof(length));
    }
}


uint64_t HashTable::get_id(uint64_t hash_1, uint64_t hash_2) {
    auto search = map.find(Hash128(hash_1, hash_2));
    if (search == map.end()) {
        return ObjectId::OBJECT_ID_NOT_FOUND;
    } else {
        return search->second;
    }
}


void HashTable::create_id(uint64_t hash_1, uint64_t hash_2, uint64_t id) {
    map.insert({ Hash128(hash_1, hash_2), id });
}
