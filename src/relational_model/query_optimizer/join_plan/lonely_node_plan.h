#ifndef RELATIONAL_MODEL__LONELY_NODE_PLAN_H_
#define RELATIONAL_MODEL__LONELY_NODE_PLAN_H_

#include "base/ids/graph_id.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"

class LonelyNodePlan : public JoinPlan {
public:
    LonelyNodePlan(const LonelyNodePlan& other);
    LonelyNodePlan(GraphId graph_id, VarId node_var_id);
    ~LonelyNodePlan() = default;

    double estimate_cost() override;
    double estimate_output_size() override;

    std::vector<VarId> get_var_order() override;
    void set_input_vars(std::vector<VarId>& input_var_order) override;

    std::unique_ptr<BindingIdIter> get_binding_id_iter() override;
    std::unique_ptr<JoinPlan> duplicate() override;

    void print(int indent) override;
private:
    GraphId graph_id;
    VarId node_var_id;

    bool element_assigned;
};

#endif // RELATIONAL_MODEL__LONELY_NODE_PLAN_H_
