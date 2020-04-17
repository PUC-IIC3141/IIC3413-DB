#ifndef STORAGE__B_PLUS_TREE_PARAMS_H_
#define STORAGE__B_PLUS_TREE_PARAMS_H_

#include <string>
#include "storage/page.h"

class BufferManager;

class BPlusTreeParams {
public:
    BPlusTreeParams(const std::string& path, int key_size);
    BPlusTreeParams(const std::string& path, int key_size, int value_size);
    ~BPlusTreeParams() = default;

    FileId const dir_file_id;
    FileId const leaf_file_id;

    int const key_size;
    int const value_size;
    int const total_size; // total_size = key_size + value_size

    int dir_max_records;
    int leaf_max_records;
};

#endif // STORAGE__B_PLUS_TREE_PARAMS_H_
