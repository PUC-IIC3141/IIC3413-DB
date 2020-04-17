#include "query_optimizer_label.h"

#include "relational_model/binding/binding_id.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"

using namespace std;

QueryOptimizerLabel::QueryOptimizerLabel
    (GraphId graph_id, VarId element_var_id, VarId label_var_id, ObjectType element_type, ObjectId label_object_id) :
    graph_id(graph_id),
    element_var_id(element_var_id),
    label_var_id(label_var_id),
    element_type(element_type),
    label_object_id(label_object_id)
{
    element_assigned = false;
    label_assigned = !label_object_id.is_null();
}


int QueryOptimizerLabel::get_heuristic() {
    if (assigned) return -1;

    else if (element_assigned && label_assigned) return 99; // Label(_,_)
    else if (element_assigned)                   return 10; // Label(_,?)
    else if (label_assigned)                     return  5; // Label(?,_)
    else                                         return  2; // Label(?,?)
}


std::vector<VarId> QueryOptimizerLabel::assign() {
    assigned = true;
    vector<VarId> res;

    if (!element_assigned)
        res.push_back(element_var_id);

    if (!label_assigned)
        res.push_back(label_var_id);

    return res;
}


void QueryOptimizerLabel::try_assign_var(VarId var_id) {
    if (assigned) {
        return;
    }
    if (element_var_id == var_id) {
        element_assigned = true;
    }
    else if (label_var_id == var_id) {
        label_assigned = true;
    }
}


unique_ptr<BindingIdIter> QueryOptimizerLabel::get_scan() {
    vector<pair<ObjectId, int>> terms;
    vector<pair<VarId, int>> vars;

    if (label_assigned) { // Label(?,_) or Label(_,_)
        if (label_object_id.is_null()) {
            vars.push_back(make_pair(label_var_id, 0));
        }
        else {
            terms.push_back(make_pair(label_object_id, 0));
        }
        vars.push_back(make_pair(element_var_id, 1));
        if (element_type == ObjectType::node) {
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).label2node,
                                          move(terms), move(vars));
        }
        else { // if (element_type == ObjectType::edge)
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).label2edge,
                                          move(terms), move(vars));
        }
    }
    else { // Label(?,?) or Label(_,?)
        vars.push_back(make_pair(element_var_id, 0));
        vars.push_back(make_pair(label_var_id, 1));
        if (element_type == ObjectType::node) {
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).node2label,
                                          move(terms), move(vars));
        }
        else { // if (element_type == ObjectType::edge)
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).edge2label,
                                          move(terms), move(vars));
        }
    }
}
