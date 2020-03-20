#ifndef RELATIONAL_MODEL__INDEX_NESTED_LOOP_JOIN_H_
#define RELATIONAL_MODEL__INDEX_NESTED_LOOP_JOIN_H_

#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"

#include <map>
#include <memory>
#include <vector>


class IndexNestedLoopJoin : public BindingIdIter {

private:
    std::unique_ptr<BindingIdIter> left;
    std::unique_ptr<BindingIdIter> right;

    BindingId* current_left;
    BindingId* current_right;

    std::vector<VarId> vars;
    std::unique_ptr<BindingId> my_binding;

    void construct_binding(BindingId& lhs, BindingId& rhs);

public:
    IndexNestedLoopJoin(std::unique_ptr<BindingIdIter> left, std::unique_ptr<BindingIdIter> right);
    ~IndexNestedLoopJoin() = default;
    void begin(BindingId& input);
    void reset(BindingId& input);
    BindingId* next();
};

#endif //RELATIONAL_MODEL__INDEX_NESTED_LOOP_JOIN_H_
