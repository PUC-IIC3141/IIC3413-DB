#ifndef RELATIONAL_MODEL__EDGE_PROPERTY_PLAN_H_
#define RELATIONAL_MODEL__EDGE_PROPERTY_PLAN_H_

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/object_id.h"
#include "relational_model/query_optimizer/join_plan/join_plan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/scan_range.h"

class EdgePropertyPlan : public JoinPlan {
public:
    EdgePropertyPlan(const EdgePropertyPlan& other);
    EdgePropertyPlan(GraphId graph_id, VarId edge_var_id, VarId key_var_id, VarId value_var_id,
                 ObjectId key_id, ObjectId value_id);
    ~EdgePropertyPlan() = default;

    double estimate_cost() override;
    double estimate_output_size() override;

    void set_input_vars(std::vector<VarId>& input_var_order) override;
    std::vector<VarId> get_var_order() override;

    std::unique_ptr<BindingIdIter> get_binding_id_iter() override;
    std::unique_ptr<JoinPlan> duplicate() override;

    void print(int indent) override;
private:
    GraphId graph_id;

    VarId edge_var_id;
    VarId key_var_id;
    VarId value_var_id;

    ObjectId key_id;
    ObjectId value_id;

    bool edge_assigned;
    bool key_assigned;
    bool value_assigned;

    std::unique_ptr<ScanRange> get_edge_range();
    std::unique_ptr<ScanRange> get_key_range();
    std::unique_ptr<ScanRange> get_value_range();
};

#endif // RELATIONAL_MODEL__EDGE_PROPERTY_PLAN_H_
