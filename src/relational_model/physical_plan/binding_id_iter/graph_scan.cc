#include "graph_scan.h"

#include "base/ids/var_id.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_leaf.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/relational_model.h"

#include <iostream>
#include <vector>

using namespace std;

GraphScan::GraphScan(GraphId graph_id, BPlusTree& bpt, vector<pair<ObjectId, int>> terms,
                     vector<pair<VarId, int>> vars)
    : record_size(bpt.params->total_size), bpt(bpt), terms(move(terms)), vars(move(vars))
{
    graph_mask = graph_id << RelationalModel::GRAPH_OFFSET;
}


void GraphScan::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());
    my_input = &input;

    vector<uint64_t> min_ids(record_size);
    vector<uint64_t> max_ids(record_size);

    for (int i = 0; i < record_size; i++) {
        min_ids[i] = graph_mask;
        max_ids[i] = 0x0000'FF'FFFFFFFFFFUL | graph_mask;
    }

    for (auto& term : terms) {
        min_ids[term.second] = term.first | graph_mask;
        max_ids[term.second] = term.first | graph_mask;
    }

    for (auto& var : vars) {
        auto obj_id = input[var.first];
        if (obj_id.is_null()) {
            break;
        }
        else {
            min_ids[var.second] = obj_id | graph_mask;
            max_ids[var.second] = obj_id | graph_mask;
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
