#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_LONELY_NODE_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_LONELY_NODE_H_

#include <string>
#include <vector>
#include <memory>

#include "base/graph/graph_object.h"
#include "base/ids/graph_id.h"
#include "base/ids/var_id.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

class QueryOptimizerLonelyNode : public QueryOptimizerElement {
private:
    GraphId graph_id;
    VarId element_var_id;

    bool assigned = false;
    bool element_assigned;

public:
    QueryOptimizerLonelyNode(GraphId graph_id, VarId element_var_id);
    ~QueryOptimizerLonelyNode() = default;
    int get_heuristic();
    void try_assign_var(VarId var_id);
    std::vector<VarId> assign();
    std::unique_ptr<BindingIdIter> get_scan();
};

#endif // RELATIONAL_MODEL__QUERY_OPTIMIZER_LONELY_NODE_H_
