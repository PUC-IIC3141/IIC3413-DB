#ifndef STORAGE__BPT_MERGER_H_
#define STORAGE__BPT_MERGER_H_

#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/ordered_file/bpt_leaf_provider.h"
#include "storage/index/ordered_file/ordered_file.h"

class BptMerger : public BptLeafProvider {
public:
    BptMerger(OrderedFile&, BPlusTree&);
    ~BptMerger();

    void begin() override;
    bool has_more_tuples() override;
    uint_fast32_t next_tuples(uint64_t* output, uint_fast32_t max_tuples) override;

private:
    OrderedFile& ordered_file;
    BPlusTree& bpt;
    const uint_fast8_t record_size;

    std::unique_ptr<BPlusTree::Iter> bpt_iter;
    std::unique_ptr<Record> bpt_record;
    std::unique_ptr<Record> ordered_file_record;
};

#endif // STORAGE__BPT_MERGER_H_
