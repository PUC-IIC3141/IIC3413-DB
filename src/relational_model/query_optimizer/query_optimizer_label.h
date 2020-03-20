#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_LABEL_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_LABEL_H_

#include <string>
#include <vector>

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/var_id.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

class RelationalGraph;

class QueryOptimizerLabel : public QueryOptimizerElement {
private:
    GraphId graph_id;

    VarId element_var_id;
    VarId label_var_id;

    ObjectType element_type;
    ObjectId label_object_id;

    bool assigned = false;

    bool element_assigned;
    bool label_assigned;

public:
    QueryOptimizerLabel(GraphId graph_id, VarId element_var_id, VarId label_var_id,
        ObjectType element_type, ObjectId label_object_id);
    ~QueryOptimizerLabel() = default;
    int get_heuristic();
    void try_assign_var(VarId var_id);
    std::vector<VarId> assign();
    std::unique_ptr<GraphScan> get_scan();
};

#endif //RELATIONAL_MODEL__QUERY_OPTIMIZER_LABEL_H_
