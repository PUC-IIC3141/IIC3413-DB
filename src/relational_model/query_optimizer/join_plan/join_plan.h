#ifndef RELATIONAL_MODEL__JOIN_PLAN_H_
#define RELATIONAL_MODEL__JOIN_PLAN_H_

#include <memory>
#include <vector>

#include "base/ids/var_id.h"
#include "relational_model/binding/binding_id_iter.h"

// Abstract Class
class JoinPlan {
public:
    virtual ~JoinPlan() { };

    virtual double estimate_cost() = 0;
    virtual double estimate_output_size() = 0;

    bool cartesian_product_needed(JoinPlan& other) {
        for (auto& my_var : get_var_order()) {
            for (auto& other_var : other.get_var_order()) {
                if (my_var == other_var) {
                    return false;
                }
            }
        }
        return true;
    }
    virtual void set_input_vars(std::vector<VarId>& input_var_order) = 0;
    virtual std::vector<VarId> get_var_order() = 0;
    virtual std::unique_ptr<BindingIdIter> get_binding_id_iter() = 0;
    virtual std::unique_ptr<JoinPlan> duplicate() = 0;

    virtual void print(int indent) = 0;
};

#endif // RELATIONAL_MODEL__JOIN_PLAN_H_
