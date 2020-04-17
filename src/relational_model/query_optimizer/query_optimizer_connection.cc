#include "query_optimizer_connection.h"

#include "relational_model/binding/binding_id.h"
#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"

using namespace std;

QueryOptimizerConnection::QueryOptimizerConnection(GraphId graph_id, VarId from_var_id, VarId to_var_id,
        VarId edge_var_id) :
    graph_id(graph_id),
    from_var_id(from_var_id),
    to_var_id(to_var_id),
    edge_var_id(edge_var_id)
{
    from_assigned = false;
    to_assigned = false;
    edge_assigned = false;
}


int QueryOptimizerConnection::get_heuristic() {
    if (assigned) return -1;

    else if (from_assigned && to_assigned && edge_assigned) return 99; // Connection(_,_,_)
    else if (from_assigned && to_assigned)                  return  9; // Connection(_,?,_)
    else if (from_assigned)                                 return  7; // Connection(_,?,?)
    else if (to_assigned)                                   return  6; // Connection(?,?,_)
    else                                                    return  1; // Connection(?,?,?)
}


void QueryOptimizerConnection::try_assign_var(VarId var_id) {
    if (assigned) {
        return;
    }
    if (from_var_id == var_id) {
        from_assigned = true;
    }
    if (to_var_id == var_id) { // not else if because from_var_id may be equal to to_var_id
        to_assigned = true;
    }
    else if (edge_var_id == var_id) {
        edge_assigned = true;
    }
}


std::vector<VarId> QueryOptimizerConnection::assign() {
    assigned = true;

    vector<VarId> res;

    if (!from_assigned)
        res.push_back(from_var_id);

    if (!to_assigned)
        res.push_back(to_var_id);

    if (!edge_assigned)
        res.push_back(edge_var_id);

    return res;
}


unique_ptr<BindingIdIter> QueryOptimizerConnection::get_scan() {
    vector<pair<ObjectId, int>> terms;
    vector<pair<VarId, int>> vars;

    if (from_assigned) {
        if (edge_assigned) {
            vars.push_back(make_pair(edge_var_id, 0));
            vars.push_back(make_pair(from_var_id, 1));
            vars.push_back(make_pair(to_var_id,   2));
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).edge_from_to,
                                          move(terms), move(vars));
        }
        else {
            vars.push_back(make_pair(from_var_id, 0));
            vars.push_back(make_pair(to_var_id,   1));
            vars.push_back(make_pair(edge_var_id, 2));
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).from_to_edge,
                                          move(terms), move(vars));
        }
    }
    else if (to_assigned) { // from_assigned == false
        vars.push_back(make_pair(to_var_id,   0));
        vars.push_back(make_pair(edge_var_id, 1));
        vars.push_back(make_pair(from_var_id, 2));
        return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).to_edge_from,
                                      move(terms), move(vars));
    }
    else {
        vars.push_back(make_pair(edge_var_id, 0));
        vars.push_back(make_pair(from_var_id, 1));
        vars.push_back(make_pair(to_var_id,   2));
        return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).edge_from_to,
                                      move(terms), move(vars));
    }
}
