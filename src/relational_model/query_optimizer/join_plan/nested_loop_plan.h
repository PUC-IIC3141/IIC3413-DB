#ifndef RELATIONAL_MODEL__NESTED_LOOP_PLAN_H_
#define RELATIONAL_MODEL__NESTED_LOOP_PLAN_H_

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"

class NestedLoopPlan : public JoinPlan {
public:
    NestedLoopPlan(const NestedLoopPlan& other);
    NestedLoopPlan(std::unique_ptr<JoinPlan> lhs, std::unique_ptr<JoinPlan> rhs);
    ~NestedLoopPlan() = default;

    double estimate_cost() override;
    double estimate_output_size() override;

    std::vector<VarId> get_var_order() override;
    void set_input_vars(std::vector<VarId>& input_var_order) override;

    std::unique_ptr<BindingIdIter> get_binding_id_iter() override;
    std::unique_ptr<JoinPlan> duplicate() override;

    static double estimate_cost(JoinPlan& lhs, JoinPlan& rhs);

    void print(int indent) override;
private:
    std::unique_ptr<JoinPlan> lhs;
    std::unique_ptr<JoinPlan> rhs;
};

#endif // RELATIONAL_MODEL__NESTED_LOOP_PLAN_H_
