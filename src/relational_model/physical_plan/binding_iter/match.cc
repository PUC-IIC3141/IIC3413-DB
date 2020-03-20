#include "match.h"

#include "storage/index/object_file/object_file.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/binding/binding_match.h"
#include "relational_model/physical_plan/binding_id_iter/index_nested_loop_join.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

#include <iostream>

using namespace std;

Match::Match(vector<unique_ptr<QueryOptimizerElement>> elements, std::map<std::string, VarId> var_pos)
    : elements(move(elements)), var_pos(move(var_pos)) {

    root = get_join_plan();
}


void Match::begin() {
    binding_id = make_unique<BindingId>(var_pos.size());
    root->begin(*binding_id);
}


unique_ptr<Binding> Match::next() {

    auto binding_id_ptr = root->next();
    if (binding_id_ptr != nullptr) {
        auto binding_id_copy = make_unique<BindingId>(*binding_id_ptr);
        return make_unique<BindingMatch>(var_pos, move(binding_id_copy));
    }
    else {
        return nullptr;
    }
}


unique_ptr<BindingIdIter> Match::get_join_plan() {

    unique_ptr<BindingIdIter> current_root = nullptr;

    auto elements_size = elements.size();
    for (size_t i = 0; i < elements_size; i++) {
        int best_index = 0;
        int best_heuristic = elements[0]->get_heuristic();

        for (size_t j = 1; j < elements_size; j++) {
            auto current_heuristic = elements[j]->get_heuristic();
            if (current_heuristic > best_heuristic) {
                best_heuristic = current_heuristic;
                best_index = j;
            }
        }
        auto assigned_vars = elements[best_index]->assign();

        for (auto& element : elements) {
            for (auto& var : assigned_vars) {
                element->try_assign_var(var);
            }
        }
        if (current_root == nullptr) {
            current_root = elements[best_index]->get_scan();
        }
        else {
            current_root = make_unique<IndexNestedLoopJoin>(move(current_root), elements[best_index]->get_scan());
        }
    }

    return current_root;
}