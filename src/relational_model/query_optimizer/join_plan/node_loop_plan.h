#ifndef RELATIONAL_MODEL__NODE_LOOP_PLAN_H_
#define RELATIONAL_MODEL__NODE_LOOP_PLAN_H_

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/object_id.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

class NodeLoopPlan : public JoinPlan {
public:
    NodeLoopPlan(const NodeLoopPlan& other);
    NodeLoopPlan(GraphId graph_id, VarId node_var_id, VarId edge_var_id);
    ~NodeLoopPlan() = default;

    double estimate_cost() override;
    double estimate_output_size() override;

    void set_input_vars(std::vector<VarId>& input_var_order) override;
    std::vector<VarId> get_var_order() override;

    std::unique_ptr<BindingIdIter> get_binding_id_iter() override;
    std::unique_ptr<JoinPlan> duplicate() override;

    void print(int indent) override;
private:
    GraphId graph_id;

    VarId node_var_id;
    VarId edge_var_id;

    bool node_assigned;
    bool edge_assigned;

    std::unique_ptr<ScanRange> get_node_range();
    std::unique_ptr<ScanRange> get_edge_range();
};

#endif // RELATIONAL_MODEL__NODE_LOOP_PLAN_H_
