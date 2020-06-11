#ifndef STORAGE__B_PLUS_TREE_SPLIT_H_
#define STORAGE__B_PLUS_TREE_SPLIT_H_

#include "storage/index/record.h"

struct BPlusTreeSplit {
    BPlusTreeSplit(Record record, int encoded_page_number)
        : record(record), encoded_page_number(encoded_page_number) { }
    Record record;
    // positive number: pointer to leaf, negative number: pointer to dir
    int encoded_page_number;
};

#endif // STORAGE__B_PLUS_TREE_SPLIT_H_
