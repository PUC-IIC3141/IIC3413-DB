/*
 * BPlusTree allows to search a record in logarithmic time
 *
 * example usage:
 * // search all records between (0,0) and (20, 1000)
 * // (including equal records)
 * auto iter = bplus_tree.get_range(Record(0, 0), Record(20, 1000));
 * auto record = iter->next();
 * while (record != nullptr) {
 *     // record is a not-null unique_ptr<Record>
 *     // do some work with record
 *     record = iter->next();
 * }
 * */

#ifndef STORAGE__B_PLUS_TREE_H_
#define STORAGE__B_PLUS_TREE_H_

#include "storage/index/bplus_tree/bplus_tree_dir.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"

#include <string>
#include <memory>

class Record;
class OrderedFile;
class BPlusTreeDir;
class BPlusTreeParams;

class BPlusTree {
private:
    bool is_empty;
    std::unique_ptr<BPlusTreeDir> root;
    void create_new(const Record& key, const Record& value);

public:
    std::unique_ptr<BPlusTreeParams> params;

    BPlusTree(std::unique_ptr<BPlusTreeParams> params);
    ~BPlusTree() = default;

    void bulk_import(OrderedFile&);

    void insert(const Record& record);
    void insert(const Record& key, const Record& value);

    bool check() const;

    std::unique_ptr<Record> get(const Record& record);

    class Iter {
        public:
            Iter(const BPlusTreeParams& params, int leaf_page_number, int current_pos, const Record& max);
            ~Iter() = default;
            std::unique_ptr<Record> next();

        private:
            int current_pos;
            const BPlusTreeParams& params;
            const Record max;
            std::unique_ptr<BPlusTreeLeaf> current_leaf;
    };

    std::unique_ptr<BPlusTree::Iter> get_range(const Record& min, const Record& max);
};

#endif // STORAGE__B_PLUS_TREE_H_
