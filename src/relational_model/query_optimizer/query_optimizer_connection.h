#ifndef RELATIONAL_MODEL__QUERY_OPTIMIZER_CONNECTION_H_
#define RELATIONAL_MODEL__QUERY_OPTIMIZER_CONNECTION_H_

#include "base/ids/graph_id.h"
#include "base/ids/var_id.h"
#include "relational_model/query_optimizer/query_optimizer_element.h"

#include <string>

class QueryOptimizerConnection : public QueryOptimizerElement {

private:
    GraphId graph_id;
    VarId from_var_id;
    VarId to_var_id;
    VarId edge_var_id;

    bool assigned = false;

    bool from_assigned;
    bool to_assigned;
    bool edge_assigned;

public:
    QueryOptimizerConnection(GraphId graph_id, VarId from_var_id, VarId to_var_id, VarId edge_var_id);
    ~QueryOptimizerConnection() = default;
    int get_heuristic();
    void try_assign_var(VarId var_id);
    std::vector<VarId> assign();
    std::unique_ptr<BindingIdIter> get_scan();
};

#endif //RELATIONAL_MODEL__QUERY_OPTIMIZER_CONNECTION_H_
