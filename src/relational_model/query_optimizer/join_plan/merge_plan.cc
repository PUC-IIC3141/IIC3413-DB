#include "merge_plan.h"

#include <limits>

#include "relational_model/execution/binding_id_iter/merge_join.h"

using namespace std;

MergePlan::MergePlan(unique_ptr<JoinPlan> lhs, unique_ptr<JoinPlan> rhs) :
    lhs(move(lhs)), rhs(move(rhs)) { }


MergePlan::MergePlan(const MergePlan& other) :
    lhs(other.lhs->duplicate()), rhs(other.rhs->duplicate()) { }


unique_ptr<JoinPlan> MergePlan::duplicate() {
    return make_unique<MergePlan>(*this);
}


void MergePlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "MergeJoin(\n";
    lhs->print(indent + 2);
    cout << ",\n";
    rhs->print(indent + 2);
    cout << "\n";
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << ")";
}


double MergePlan::estimate_cost() {
    return estimate_cost(*lhs, *rhs);
}


double MergePlan::estimate_cost(JoinPlan& lhs, JoinPlan& rhs) {
    bool merge_possible = true;

    // check merge is possible
    std::vector<VarId> join_vars;
    auto left_vars = lhs.get_var_order();
    auto right_vars = rhs.get_var_order();

    // merge must be on first variable
    if (left_vars[0] != right_vars[0]) {
        merge_possible = false;
    }

    // only first variable is on both left and right
    auto left_size = left_vars.size();
    for (size_t i = 1; i < left_size; ++i) {
        auto right_size = right_vars.size();
        for (size_t j = 1; j < right_size; ++j) {
            if (left_vars[i] == right_vars[j]) {
                merge_possible = false;
            }
        }
    }

    // checkear que tienen las variables de join al principio y en el mismo orden
    for (size_t i = 0; i < join_vars.size(); ++i) {
        if (right_vars[i] != left_vars[i]) {
            merge_possible = false;
            break;
        }
    }

    if (merge_possible) {
        return lhs.estimate_cost() + rhs.estimate_cost();
    } else {
        return numeric_limits<double>::max();
    }
}


double MergePlan::estimate_output_size() {
    // TODO: better estimation needed
    return lhs->estimate_output_size() * rhs->estimate_output_size();
}


vector<VarId> MergePlan::get_var_order() {
    auto result = lhs->get_var_order();

    for (auto& right_var : rhs->get_var_order()) {
        bool already_present = false;
        for (auto res_var : result) {
            if (res_var == right_var) {
                already_present = true;
                break;
            }
        }
        if (!already_present) {
            result.push_back(right_var);
        }
    }
    return result;
}


void MergePlan::set_input_vars(std::vector<VarId>& input_var_order) {
    lhs->set_input_vars(input_var_order);
    rhs->set_input_vars(input_var_order);
}


unique_ptr<BindingIdIter> MergePlan::get_binding_id_iter() {
    auto join_var = lhs->get_var_order()[0];
    return make_unique<MergeJoin>(lhs->get_binding_id_iter(), rhs->get_binding_id_iter(), join_var);
}