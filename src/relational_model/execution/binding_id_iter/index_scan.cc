#include "index_scan.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "base/ids/var_id.h"
#include "relational_model/relational_model.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"

using namespace std;

IndexScan::IndexScan(BPlusTree& bpt, std::vector<std::unique_ptr<ScanRange>> ranges)
    : record_size(bpt.params->total_size), bpt(bpt), ranges(move(ranges)) { }


void IndexScan::begin(BindingId& input) {
    assert(ranges.size() == static_cast<size_t>(bpt.params->total_size)
        && "Inconsistent size of ranges and bpt");

    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;
    my_binding->add_all(*my_input);

    vector<uint64_t> min_ids(record_size);
    vector<uint64_t> max_ids(record_size);

    int i = 0;
    for (auto& range : ranges) {
        min_ids[i] = range->get_min(input);
        max_ids[i] = range->get_max(input);
        ++i;
    }

    it = bpt.get_range(
        Record(min_ids),
        Record(max_ids)
    );
}


BindingId* IndexScan::next() {
    if (it == nullptr)
        return nullptr;

    auto next = it->next();
    if (next != nullptr) {
        int i = 0;
        for (auto& range : ranges) {
            range->try_assign(*my_binding, next->ids[i]);
            ++i;
        }
        return my_binding.get();
    } else {
        return nullptr;
    }
}


void IndexScan::reset(BindingId& input) {
    my_input = &input;
    // TODO: if nulls were supported a my_binding->clean should be performed to set NULL_OBJECT_ID
    my_binding->add_all(*my_input);

    vector<uint64_t> min_ids(record_size);
    vector<uint64_t> max_ids(record_size);

    int i = 0;
    for (auto& range : ranges) {
        min_ids[i] = range->get_min(input);
        max_ids[i] = range->get_max(input);
        ++i;
    }

    it = bpt.get_range(
        Record(min_ids),
        Record(max_ids)
    );
}
