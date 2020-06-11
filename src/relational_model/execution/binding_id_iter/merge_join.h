#ifndef RELATIONAL_MODEL__MERGE_JOIN_H_
#define RELATIONAL_MODEL__MERGE_JOIN_H_

#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"

#include <memory>

// This merge is valid only under these conditions:
// - lhs and rhs have one variable in common: `join_var`
// - lhs and rhs will be ordered by `join_var`
// - lhs and rhs won't give any duplicates of `join_var`

class MergeJoin : public BindingIdIter {
public:
    MergeJoin(std::unique_ptr<BindingIdIter> lhs,
              std::unique_ptr<BindingIdIter> rhs,
              VarId join_var);
    ~MergeJoin() = default;

    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();

private:
    std::unique_ptr<BindingIdIter> lhs;
    std::unique_ptr<BindingIdIter> rhs;
    VarId join_var;

    BindingId* current_left = nullptr;
    BindingId* current_right = nullptr;

    std::unique_ptr<BindingId> my_binding;

    void construct_binding();
};

#endif // RELATIONAL_MODEL__MERGE_JOIN_H_
