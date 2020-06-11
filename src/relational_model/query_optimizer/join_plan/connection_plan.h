#ifndef RELATIONAL_MODEL__CONNECTION_PLAN_H_
#define RELATIONAL_MODEL__CONNECTION_PLAN_H_

#include "base/ids/graph_id.h"
#include "base/graph/graph_object.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

class ConnectionPlan : public JoinPlan {
public:
    ConnectionPlan(const ConnectionPlan& other);
    ConnectionPlan(GraphId graph_id, VarId node_from_var_id, VarId node_to_var_id, VarId edge_var_id);
    ~ConnectionPlan() = default;

    double estimate_cost() override;
    double estimate_output_size() override;

    void set_input_vars(std::vector<VarId>& input_var_order) override;
    std::vector<VarId> get_var_order() override;
    std::unique_ptr<BindingIdIter> get_binding_id_iter() override;
    std::unique_ptr<JoinPlan> duplicate() override;

    void print(int indent) override;
private:
    GraphId graph_id;

    VarId node_from_var_id;
    VarId node_to_var_id;
    VarId edge_var_id;

    bool node_from_assigned;
    bool node_to_assigned;
    bool edge_assigned;

    std::unique_ptr<ScanRange> get_node_from_range();
    std::unique_ptr<ScanRange> get_node_to_range();
    std::unique_ptr<ScanRange> get_edge_range();
};

#endif // RELATIONAL_MODEL__CONNECTION_PLAN_H_
