#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_PROPERTY_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_PROPERTY_H_

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/var_id.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

#include <string>

class QueryOptimizerProperty : public QueryOptimizerElement {

private:
    GraphId graph_id;

    VarId element_var_id;
    VarId key_var_id;
    VarId value_var_id;

    ObjectType element_type;
    ObjectId key_object_id;
    ObjectId value_object_id;

    bool assigned = false;

    bool element_assigned;
    bool key_assigned;
    bool value_assigned;

public:
    QueryOptimizerProperty(GraphId graph_id, VarId element_var_id, VarId key_var_id,
        VarId value_var_id, ObjectType element_type, ObjectId key_object_id, ObjectId value_object_id);
    ~QueryOptimizerProperty() = default;

    int get_heuristic();
    void try_assign_var(VarId var_id);
    std::vector<VarId> assign();
    std::unique_ptr<GraphScan> get_scan();
};

#endif //RELATIONAL_MODEL__QUERY_OPTIMIZER_PROPERTY_H_
