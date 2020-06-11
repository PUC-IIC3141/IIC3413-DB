#include "node_loop_plan.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "relational_model/execution/binding_id_iter/index_scan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/assigned_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/default_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/named_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/term.h"
#include "storage/catalog/catalog.h"

using namespace std;

NodeLoopPlan::NodeLoopPlan(GraphId graph_id, VarId node_var_id, VarId edge_var_id) :
    graph_id(graph_id),
    node_var_id(node_var_id),
    edge_var_id(edge_var_id),
    node_assigned(false),
    edge_assigned(false) { }


NodeLoopPlan::NodeLoopPlan(const NodeLoopPlan& other) :
    graph_id(other.graph_id),
    node_var_id(other.node_var_id),
    edge_var_id(other.edge_var_id),
    node_assigned(other.node_assigned),
    edge_assigned(other.edge_assigned) { }


std::unique_ptr<JoinPlan> NodeLoopPlan::duplicate() {
    return make_unique<NodeLoopPlan>(*this);
}


void NodeLoopPlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "NodeLoop(node: " << node_var_id.id << (node_assigned ? " assigned" : " not-assigned")
         << ", edge: " << edge_var_id.id << (edge_assigned ? " assigned" : " not-assigned")
         << ")";
}


double NodeLoopPlan::estimate_cost() {
    return estimate_output_size();
}


double NodeLoopPlan::estimate_output_size() {
    // TODO: better estimations needed
    return 1;
}


void NodeLoopPlan::set_input_vars(std::vector<VarId>& input_var_order) {
    for (auto& input_var : input_var_order) {
        if (node_var_id == input_var) {
            node_assigned = true;
        } else if (edge_var_id == input_var) {
            edge_assigned = true;
        }
    }
}

// Must be consistent with the index scan returned in get_binding_id_iter()
vector<VarId> NodeLoopPlan::get_var_order() {
    vector<VarId> result;
    if (node_assigned || !edge_assigned) {
        // cases 1, 2 and 4 uses NE
        result.push_back(node_var_id);
        result.push_back(edge_var_id);
    } else {
        // case 3 uses EN
        result.push_back(edge_var_id);
        result.push_back(node_var_id);
    }
    return result;
}


/**
 * ╔═╦══════════╦═════════╦═════════╗
 * ║ ║  NodeId  ║ EdgeId  ║  Index  ║
 * ╠═╬══════════╬═════════╬═════════╣
 * ║1║    yes   ║   yes   ║    NE   ║ => any index works for this case
 * ║2║    yes   ║   no    ║    NE   ║
 * ║3║    no    ║   yes   ║    EN   ║
 * ║4║    no    ║   no    ║    NE   ║ => any index works for this case
 * ╚═╩══════════╩═════════╩═════════╝
 */
unique_ptr<BindingIdIter> NodeLoopPlan::get_binding_id_iter() {
    vector<unique_ptr<ScanRange>> ranges;
    if (node_assigned || !edge_assigned) {
        // cases 1, 2 and 4 uses NE
        ranges.push_back(get_node_range());
        ranges.push_back(get_edge_range());

        return make_unique<IndexScan>(relational_model.get_nodeloop_edge(), move(ranges));
    } else {
        // case 3 uses EN
        ranges.push_back(get_edge_range());
        ranges.push_back(get_node_range());

        return make_unique<IndexScan>(relational_model.get_edge_nodeloop(), move(ranges));
    }
}


std::unique_ptr<ScanRange> NodeLoopPlan::get_node_range() {
    if (node_assigned) {
        return make_unique<AssignedVar>(node_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(node_var_id);
    } else {
        return make_unique<NamedGraphVar>(node_var_id, graph_id, ObjectType::node);
    }
}


std::unique_ptr<ScanRange> NodeLoopPlan::get_edge_range() {
    if (edge_assigned) {
        return make_unique<AssignedVar>(edge_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(edge_var_id);
    } else {
        return make_unique<NamedGraphVar>(edge_var_id, graph_id, ObjectType::edge);
    }
}
