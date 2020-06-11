#include "connection_plan.h"

#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/execution/binding_id_iter/index_scan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/assigned_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/default_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/named_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/term.h"
#include "storage/catalog/catalog.h"

using namespace std;

ConnectionPlan::ConnectionPlan(GraphId graph_id, VarId node_from_var_id,
                               VarId node_to_var_id, VarId edge_var_id) :
    graph_id(graph_id),
    node_from_var_id(node_from_var_id),
    node_to_var_id(node_to_var_id),
    edge_var_id(edge_var_id),
    node_from_assigned(false),
    node_to_assigned(false),
    edge_assigned(false) { }


ConnectionPlan::ConnectionPlan(const ConnectionPlan& other) :
    graph_id(other.graph_id),
    node_from_var_id(other.node_from_var_id),
    node_to_var_id(other.node_to_var_id),
    edge_var_id(other.edge_var_id),
    node_from_assigned(other.node_from_assigned),
    node_to_assigned(other.node_to_assigned),
    edge_assigned(other.edge_assigned) { }


std::unique_ptr<JoinPlan> ConnectionPlan::duplicate() {
    return make_unique<ConnectionPlan>(*this);
}


void ConnectionPlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "Connection(node_from: " << node_from_var_id.id << (node_from_assigned ? " assigned" : " not-assigned")
         << ", node_to: " << node_to_var_id.id << (node_to_assigned ? " assigned" : " not-assigned")
         << ", edge: " << edge_var_id.id << (edge_assigned ? " assigned" : " not-assigned")
         << ")";
}


double ConnectionPlan::estimate_cost() {
    return estimate_output_size();
}


double ConnectionPlan::estimate_output_size() {
    // TODO: better estimations needed
    if (node_from_assigned) {
        if (edge_assigned) {
            if (node_to_assigned) {
                // CASE 1
                return 1;
            } else {
                // CASE 2
                return 1;
            }
        } else {
            if (node_to_assigned) {
                // CASE 3
                return static_cast<double>(catalog.get_edge_count(graph_id))
                       / static_cast<double>(catalog.get_node_count(graph_id));
            } else {
                // CASE 4
                // total_edges / total_nodes
                return static_cast<double>(catalog.get_edge_count(graph_id))
                       / static_cast<double>(catalog.get_node_count(graph_id));
            }
        }
    } else {
        if (edge_assigned) {
            if (node_to_assigned) {
                // CASE 5
                return 1;
            } else {
                // CASE 6
                return 1;
            }
        } else {
            if (node_to_assigned) {
                // CASE 7
                // total_edges / total_nodes
                return static_cast<double>(catalog.get_edge_count(graph_id))
                       / static_cast<double>(catalog.get_node_count(graph_id));
            } else {
                // CASE 8
                return static_cast<double>(catalog.get_edge_count(graph_id));
            }
        }
    }
}


void ConnectionPlan::set_input_vars(std::vector<VarId>& input_var_order) {
    for (auto& input_var : input_var_order) {
        if (node_to_var_id == input_var) {
            node_to_assigned = true;
        } else if (node_from_var_id == input_var) {
            node_from_assigned = true;
        } else if (edge_var_id == input_var) {
            edge_assigned = true;
        }
    }
}

// Must be consistent with the index scan returned in get_binding_id_iter()
vector<VarId> ConnectionPlan::get_var_order() {
    vector<VarId> result;
    if (node_from_assigned) {
        if (edge_assigned) { // CASES 1 and 2 => EFT
            result.push_back(edge_var_id);
            result.push_back(node_from_var_id);
            result.push_back(node_to_var_id);
        } else { // CASES 3 and 4 => FTE
            result.push_back(node_from_var_id);
            result.push_back(node_to_var_id);
            result.push_back(edge_var_id);
        }
    } else {
        if (node_to_assigned) { // CASES 5 and 7 => TEF
            result.push_back(node_to_var_id);
            result.push_back(edge_var_id);
            result.push_back(node_from_var_id);
        } else { // CASES 6 and 8 => EFT
            result.push_back(edge_var_id);
            result.push_back(node_from_var_id);
            result.push_back(node_to_var_id);
        }
    }
    return result;
}


/** FTE | TEF | EFT
 * ╔═╦══════════╦════════╦══════════╦══════════╗
 * ║ ║ NodeFrom ║  Edge  ║  NodeTo  ║  index   ║
 * ╠═╬══════════╬════════╬══════════╬══════════╣
 * ║1║     yes  ║   yes  ║    yes   ║    EFT   ║ => any index works for this case
 * ║2║     yes  ║   yes  ║    no    ║    EFT   ║
 * ║3║     yes  ║   no   ║    yes   ║    FTE   ║
 * ║4║     yes  ║   no   ║    no    ║    FTE   ║
 * ║5║     no   ║   yes  ║    yes   ║    TEF   ║
 * ║6║     no   ║   yes  ║    no    ║    EFT   ║
 * ║7║     no   ║   no   ║    yes   ║    TEF   ║
 * ║8║     no   ║   no   ║    no    ║    EFT   ║ => any index works for this case
 * ╚═╩══════════╩════════╩══════════╩══════════╝
 */
unique_ptr<BindingIdIter> ConnectionPlan::get_binding_id_iter() {
    vector<unique_ptr<ScanRange>> ranges;

    if (node_from_assigned) {
        if (edge_assigned) { // CASES 1 and 2 => EFT
            ranges.push_back(get_edge_range());
            ranges.push_back(get_node_from_range());
            ranges.push_back(get_node_to_range());

            return make_unique<IndexScan>(relational_model.get_edge_from_to(), move(ranges));
        } else { // CASES 3 and 4 => FTE
            ranges.push_back(get_node_from_range());
            ranges.push_back(get_node_to_range());
            ranges.push_back(get_edge_range());

            return make_unique<IndexScan>(relational_model.get_from_to_edge(), move(ranges));
        }
    } else {
        if (node_to_assigned) { // CASES 5 and 7 => TEF
            ranges.push_back(get_node_to_range());
            ranges.push_back(get_edge_range());
            ranges.push_back(get_node_from_range());

            return make_unique<IndexScan>(relational_model.get_to_edge_from(), move(ranges));
        } else { // CASES 6 and 8 => EFT
            ranges.push_back(get_edge_range());
            ranges.push_back(get_node_from_range());
            ranges.push_back(get_node_to_range());

            return make_unique<IndexScan>(relational_model.get_edge_from_to(), move(ranges));
        }
    }
}


std::unique_ptr<ScanRange> ConnectionPlan::get_node_from_range() {
    if (node_from_assigned) {
        return make_unique<AssignedVar>(node_from_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(node_from_var_id);
    } else {
        return make_unique<NamedGraphVar>(node_from_var_id, graph_id, ObjectType::node);
    }
}


std::unique_ptr<ScanRange> ConnectionPlan::get_node_to_range() {
    if (node_to_assigned) {
        return make_unique<AssignedVar>(node_to_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(node_to_var_id);
    } else {
        return make_unique<NamedGraphVar>(node_to_var_id, graph_id, ObjectType::node);
    }
}


std::unique_ptr<ScanRange> ConnectionPlan::get_edge_range() {
    if (edge_assigned) {
        return make_unique<AssignedVar>(edge_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(edge_var_id);
    } else {
        return make_unique<NamedGraphVar>(edge_var_id, graph_id, ObjectType::edge);
    }
}
