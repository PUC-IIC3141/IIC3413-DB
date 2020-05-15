#include "total_scan.h"

#include "base/ids/var_id.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/relational_model.h"

using namespace std;

TotalScan::TotalScan(BPlusTree& bpt, vector<pair<VarId, int>> vars)
    : record_size(bpt.params->total_size), bpt(bpt), vars(move(vars)) { }


void TotalScan::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;

    vector<uint64_t> min_ids(record_size);
    vector<uint64_t> max_ids(record_size);

    for (int i = 0; i < record_size; i++) {
        min_ids[i] = 0;
        max_ids[i] = 0xFFFF'FF'FFFFFFFFFFUL;
    }
    it = bpt.get_range(
        Record(min_ids),
        Record(max_ids)
    );
}


BindingId* TotalScan::next() {
    if (it == nullptr)
        return nullptr;

    auto next = it->next();
    if (next != nullptr) {
        my_binding->add_all(*my_input);
        for (auto& var : vars) {
            ObjectId element_id = ObjectId(next->ids[var.second]);
            my_binding->add(var.first, element_id);
        }
        return my_binding.get();
    }
    else return nullptr;
}


void TotalScan::reset(BindingId& input) {
    begin(input);
}
