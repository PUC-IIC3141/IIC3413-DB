#include "lonely_node_plan.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "relational_model/execution/binding_id_iter/node_enum.h"
#include "storage/catalog/catalog.h"

using namespace std;

LonelyNodePlan::LonelyNodePlan(GraphId graph_id, VarId node_var_id) :
    graph_id(graph_id),
    node_var_id(node_var_id) { }


LonelyNodePlan::LonelyNodePlan(const LonelyNodePlan& other) :
    graph_id(other.graph_id),
    node_var_id(other.node_var_id) { }


unique_ptr<JoinPlan> LonelyNodePlan::duplicate() {
    return make_unique<LonelyNodePlan>(*this);
}


double LonelyNodePlan::estimate_cost() {
    return estimate_output_size();
}

void LonelyNodePlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "LonelyNode()";
}

double LonelyNodePlan::estimate_output_size() {
    return catalog.get_node_count(graph_id);
}


vector<VarId> LonelyNodePlan::get_var_order() {
    return vector<VarId> { node_var_id };
}


void LonelyNodePlan::set_input_vars(vector<VarId>& input_var_order) {
    for (auto& var_id : input_var_order) {
        if (node_var_id == var_id) {
            // TODO: para que esto se cumpla hay que eliminar redundancia
            // EJ:
            // SELECT *
            // MATCH (?n), (?n)
            throw std::logic_error("LonelyNode should appear once and only if no other node with "
                                "the same name is present in the Graph Pattern");
        }
    }
}


unique_ptr<BindingIdIter> LonelyNodePlan::get_binding_id_iter() {
    return make_unique<NodeEnum>(graph_id, node_var_id);
}
