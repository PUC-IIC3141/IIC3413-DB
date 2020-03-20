#include "graph_scan.h"

#include "base/ids/var_id.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "relational_model/binding/binding_id.h"

#include <iostream>
#include <vector>

using namespace std;

GraphScan::GraphScan(BPlusTree& bpt, vector<pair<ObjectId, int>> terms, vector<pair<VarId, int>> vars)
    : record_size(bpt.params->total_size), bpt(bpt), terms(move(terms)), vars(move(vars)) { }


void GraphScan::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;

    vector<uint64_t> min_ids(record_size);
    vector<uint64_t> max_ids(record_size);

    for (int i = 0; i < record_size; i++) {
        min_ids[i] = 0;
        max_ids[i] = UINT64_MAX;
    }

    for (auto& term : terms) {
        min_ids[term.second] = term.first;
        max_ids[term.second] = term.first;
    }

    for (auto& var : vars) {
        auto obj_id = input[var.first];
        if (obj_id.is_null()) {
            break;
        }
        else {
            min_ids[var.second] = obj_id;
            max_ids[var.second] = obj_id;
        }
    }

    it = bpt.get_range(
        Record(min_ids),
        Record(max_ids)
    );
}


BindingId* GraphScan::next() {
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


void GraphScan::reset(BindingId& input) {
    begin(input);
}
