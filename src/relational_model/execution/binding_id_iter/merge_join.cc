#include "merge_join.h"

using namespace std;

MergeJoin::MergeJoin(unique_ptr<BindingIdIter> lhs, unique_ptr<BindingIdIter> rhs,
                     VarId join_var) :
    lhs(move(lhs)), rhs(move(rhs)), join_var(join_var) { }


void MergeJoin::begin(BindingId& input) {
    my_binding = make_unique<BindingId>(input.var_count());

    lhs->begin(input);
    current_left = lhs->next();

    rhs->begin(input);
    current_right = rhs->next();
}


void MergeJoin::reset(BindingId& input) {
    lhs->reset(input);
    current_left = lhs->next();

    rhs->reset(input);
    current_right = rhs->next();
}


BindingId* MergeJoin::next() {
    while (current_left != nullptr && current_right != nullptr) {
        if ( (*current_left)[join_var] == (*current_right)[join_var] ) {
            construct_binding();
            current_left = lhs->next();
            current_right = rhs->next();
            return my_binding.get();
        } else {
            if ( (*current_left)[join_var] < (*current_right)[join_var] ) {
                current_left = lhs->next();
            } else {
                current_right = rhs->next();
            }
        }
    }
    return nullptr;
}


void MergeJoin::construct_binding() {
    my_binding->add_all(*current_left);
    my_binding->add_all(*current_right);
}