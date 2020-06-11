#include "node_label_plan.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "relational_model/execution/binding_id_iter/index_scan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/assigned_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/default_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/named_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/term.h"
#include "storage/catalog/catalog.h"

using namespace std;

NodeLabelPlan::NodeLabelPlan(GraphId graph_id, VarId node_var_id, VarId label_var_id, ObjectId label_id) :
    graph_id(graph_id),
    node_var_id(node_var_id),
    label_var_id(label_var_id),
    label_id(label_id),
    node_assigned(false),
    label_assigned( !label_id.is_null() ) { }


NodeLabelPlan::NodeLabelPlan(const NodeLabelPlan& other) :
    graph_id(other.graph_id),
    node_var_id(other.node_var_id),
    label_var_id(other.label_var_id),
    label_id(other.label_id),
    node_assigned(other.node_assigned),
    label_assigned(other.label_assigned) { }


std::unique_ptr<JoinPlan> NodeLabelPlan::duplicate() {
    return make_unique<NodeLabelPlan>(*this);
}


void NodeLabelPlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "NodeLabel(node: " << node_var_id.id << (node_assigned ? " assigned" : " not-assigned")
         << ", label: " << label_var_id.id << (label_assigned ? " assigned" : " not-assigned")
         << ")";
}


double NodeLabelPlan::estimate_cost() {
    return estimate_output_size();
}


double NodeLabelPlan::estimate_output_size() {
    // TODO: better estimations needed
    if (node_assigned && label_assigned) {
        return 1;
    } else if (label_assigned) {
        if (label_id.is_null()) {
            // total_labels / distinct_labels
            return static_cast<double>(catalog.get_node_labels(graph_id))
                   / static_cast<double>(catalog.get_node_distinct_labels(graph_id));
        } else {
            return catalog.get_node_count_for_label(graph_id, label_id);
        }
    } else if (node_assigned) {
        return static_cast<double>(catalog.get_node_labels(graph_id))
               / static_cast<double>(catalog.get_node_count(graph_id)) ;
    } else {
        return catalog.get_node_labels(graph_id);
    }
}


void NodeLabelPlan::set_input_vars(std::vector<VarId>& input_var_order) {
    for (auto& input_var : input_var_order) {
        if (node_var_id == input_var) {
            node_assigned = true;
        } else if (label_var_id == input_var) {
            label_assigned = true;
        }
    }
}

// Must be consistent with the index scan returned in get_binding_id_iter()
vector<VarId> NodeLabelPlan::get_var_order() {
    vector<VarId> result;
    if (node_assigned || !label_assigned) {
        // cases 1, 2 and 4 uses NL
        result.push_back(node_var_id);
        if (!label_var_id.is_null()) {
            result.push_back(label_var_id);
        }
    } else {
        // case 3 uses LN
        if (!label_var_id.is_null()) {
            result.push_back(label_var_id);
        }
        result.push_back(node_var_id);
    }
    return result;
}


/**
 * ╔═╦══════════╦═════════╦═════════╗
 * ║ ║  NodeId  ║ LabelId ║  Index  ║
 * ╠═╬══════════╬═════════╬═════════╣
 * ║1║    yes   ║   yes   ║    NL   ║ => any index works for this case
 * ║2║    yes   ║   no    ║    NL   ║
 * ║3║    no    ║   yes   ║    LN   ║
 * ║4║    no    ║   no    ║    NL   ║ => any index works for this case
 * ╚═╩══════════╩═════════╩═════════╝
 */
unique_ptr<BindingIdIter> NodeLabelPlan::get_binding_id_iter() {
    vector<unique_ptr<ScanRange>> ranges;
    if (node_assigned || !label_assigned) {
        // cases 1, 2 and 4 uses NL
        ranges.push_back(get_node_range());
        ranges.push_back(get_label_range());

        return make_unique<IndexScan>(relational_model.get_node2label(), move(ranges));
    } else {
        // case 3 uses LN
        ranges.push_back(get_label_range());
        ranges.push_back(get_node_range());

        return make_unique<IndexScan>(relational_model.get_label2node(), move(ranges));
    }
}

std::unique_ptr<ScanRange> NodeLabelPlan::get_node_range() {
    if (node_assigned) {
        return make_unique<AssignedVar>(node_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(node_var_id);
    } else {
        return make_unique<NamedGraphVar>(node_var_id, graph_id, ObjectType::node);
    }
}


std::unique_ptr<ScanRange> NodeLabelPlan::get_label_range() {
    if (!label_id.is_null()) {
        return make_unique<Term>(label_id);
    } else if (label_assigned) {
        return make_unique<AssignedVar>(label_var_id);
    } else {
        return make_unique<DefaultGraphVar>(label_var_id);
    }
}
