#include "query_optimizer_property.h"

#include "relational_model/binding/binding_id.h"
#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"

using namespace std;

QueryOptimizerProperty::QueryOptimizerProperty(GraphId graph_id, VarId element_var_id, VarId key_var_id,
    VarId value_var_id, ObjectType element_type, ObjectId key_object_id, ObjectId value_object_id) :
    graph_id(graph_id),
    element_var_id(element_var_id),
    key_var_id(key_var_id),
    value_var_id(value_var_id),
    element_type(element_type),
    key_object_id(key_object_id),
    value_object_id(value_object_id)
{
    element_assigned = false;
    key_assigned = !key_object_id.is_null();
    value_assigned = !value_object_id.is_null();
}


int QueryOptimizerProperty::get_heuristic() {
    if (assigned) return -1;

    else if (element_assigned && key_assigned && value_assigned) return 99; // Property(_,_,_)
    else if (element_assigned && key_assigned)                   return 11; // Property(_,_,?)
    else if (key_assigned && value_assigned)                     return  8; // Property(?,_,_)
    else if (element_assigned && key_assigned)                   return  4; // Property(?,_,?)
    else                                                         return  1; // Property(?,?,?)
}


void QueryOptimizerProperty::try_assign_var(VarId var_id) {
    if (assigned) {
        return;
    }
    if (element_var_id == var_id) {
        element_assigned = true;
    }
    else if (key_var_id == var_id) {
        key_assigned = true;
    }
    else if (value_var_id == var_id) {
        value_assigned = true;
    }
}


std::vector<VarId> QueryOptimizerProperty::assign() {
    assigned = true;
    vector<VarId> res;

    if (!element_assigned)
        res.push_back(element_var_id);

    if (!key_assigned)
        res.push_back(key_var_id);

    if (!value_assigned)
        res.push_back(value_var_id);

    return res;
}


unique_ptr<BindingIdIter> QueryOptimizerProperty::get_scan() {
    vector<pair<ObjectId, int>> terms;
    vector<pair<VarId, int>> vars;

    if (element_assigned) { // use Element|Key|Value
        vars.push_back(make_pair(element_var_id, 0));
        if (key_object_id.is_null()) {
            vars.push_back(make_pair(key_var_id, 1));
            if (!value_object_id.is_null()) {
                cout << "ERROR: at query_optimizer_property: if key is not a term, value cannot be a term\n";
            }
            vars.push_back(make_pair(value_var_id, 2));
        }
        else {
            terms.push_back(make_pair(key_object_id, 1));
            if (value_object_id.is_null()) {
                vars.push_back(make_pair(value_var_id, 2));
            }
            else {
                terms.push_back(make_pair(value_object_id, 2));
            }
        }
        if (element_type == ObjectType::node) {
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).node2prop,
                                          move(terms), move(vars));
        }
        else { // if (element_type == ObjectType::edge)
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).edge2prop,
                                          move(terms), move(vars));
        }
    }
    else { // use Key|Value|Element
        if (!key_object_id.is_null()) {
            terms.push_back(make_pair(key_object_id, 0));
            if (!value_object_id.is_null()) {
                terms.push_back(make_pair(value_object_id, 1));
            }
            else {
                vars.push_back(make_pair(value_var_id, 1));
            }
        }
        else {
            vars.push_back(make_pair(key_var_id, 0));
            if (!value_object_id.is_null()) {
                cout << "ERROR: at query_optimizer_property: if key is not a term, value cannot be a term\n";
            }
            else {
                vars.push_back(make_pair(value_var_id, 1));
            }
        }
        vars.push_back(make_pair(element_var_id, 2));

        if (element_type == ObjectType::node) {
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).prop2node,
                                          move(terms), move(vars));
        }
        else { // if (element_type == ObjectType::edge)
            return make_unique<GraphScan>(graph_id, *RelationalModel::get_graph(graph_id).prop2edge,
                                          move(terms), move(vars));
        }
    }
}
