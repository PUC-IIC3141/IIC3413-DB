#ifndef STORAGE__INDEX__B_PLUS_TREE_DIR_H_
#define STORAGE__INDEX__B_PLUS_TREE_DIR_H_

#include "storage/page.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "storage/index/bplus_tree/bplus_tree_split.h"

#include <memory>

class BPlusTreeDir {
friend class BPlusTree;
public:
    BPlusTreeDir(const BPlusTreeParams& params, Page& page);
    ~BPlusTreeDir();

    std::unique_ptr<BPlusTreeSplit> bulk_insert(BPlusTreeLeaf& leaf);

    // returns not null when it needs to split
    std::unique_ptr<BPlusTreeSplit> insert(const Record& key, const Record& value);

    std::unique_ptr<Record> get(const Record& key);
    SearchLeafResult search_leaf(const Record& min);

    bool check() const;

    bool is_leaf()  { return false; }
    int get_key_count() { return *key_count; }

private:
    const BPlusTreeParams& params;
    Page& page;
    int* key_count;
    uint64_t* keys;
    int* children;

    int search_child_index(int from, int to, const Record& record);
    void shift_right_keys(int from, int to);
    void shift_right_children(int from, int to);
    void update_key(int index, const Record& record);
    void update_child(int index, int dir);
    void split(const Record& record);
};

#endif // STORAGE__INDEX__B_PLUS_TREE_DIR_H_
