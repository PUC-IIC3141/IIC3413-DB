#include "edge_label_plan.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "relational_model/execution/binding_id_iter/index_scan.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/assigned_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/default_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/named_graph_var.h"
#include "relational_model/execution/binding_id_iter/scan_ranges/term.h"

#include "storage/catalog/catalog.h"

using namespace std;

EdgeLabelPlan::EdgeLabelPlan(GraphId graph_id, VarId edge_var_id, VarId label_var_id, ObjectId label_id) :
    graph_id(graph_id),
    edge_var_id(edge_var_id),
    label_var_id(label_var_id),
    label_id(label_id),
    edge_assigned(false),
    label_assigned( !label_id.is_null() ) { }


EdgeLabelPlan::EdgeLabelPlan(const EdgeLabelPlan& other) :
    graph_id(other.graph_id),
    edge_var_id(other.edge_var_id),
    label_var_id(other.label_var_id),
    label_id(other.label_id),
    edge_assigned(other.edge_assigned),
    label_assigned(other.label_assigned) { }


std::unique_ptr<JoinPlan> EdgeLabelPlan::duplicate() {
    return make_unique<EdgeLabelPlan>(*this);
}


void EdgeLabelPlan::print(int indent) {
    for (int i = 0; i < indent; ++i) {
        cout << ' ';
    }
    cout << "EdgeLabel(edge: " << edge_var_id.id << (edge_assigned ? " assigned" : " not-assigned")
         << ", label: " << label_var_id.id << (label_assigned ? " assigned" : " not-assigned")
         << ")";
}


double EdgeLabelPlan::estimate_cost() {
    return estimate_output_size();
}


double EdgeLabelPlan::estimate_output_size() {
    // TODO: better estimations needed
    if (edge_assigned && label_assigned) {
        return 1;
    } else if (label_assigned) {
        if (label_id.is_null()) {
            // total_labels / distinct_labels
            return static_cast<double>(catalog.get_edge_labels(graph_id))
                   / static_cast<double>(catalog.get_edge_distinct_labels(graph_id));
        } else {
            return catalog.get_edge_count_for_label(graph_id, label_id);
        }
    } else if (edge_assigned) {
        return static_cast<double>(catalog.get_edge_labels(graph_id))
               / static_cast<double>(catalog.get_edge_count(graph_id)) ;
    } else {
        return catalog.get_edge_labels(graph_id);
    }
}


void EdgeLabelPlan::set_input_vars(std::vector<VarId>& input_var_order) {
    for (auto& input_var : input_var_order) {
        if (edge_var_id == input_var) {
            edge_assigned = true;
        } else if (label_var_id == input_var) {
            label_assigned = true;
        }
    }
}

// Must be consistent with the index scan returned in get_binding_id_iter()
vector<VarId> EdgeLabelPlan::get_var_order() {
    vector<VarId> result;
    if (edge_assigned || !label_assigned) {
        // cases 1, 2 and 4 uses EL
        result.push_back(edge_var_id);
        if (!label_var_id.is_null()) {
            result.push_back(label_var_id);
        }
    } else {
        // case 3 uses LE
        if (!label_var_id.is_null()) {
            result.push_back(label_var_id);
        }
        result.push_back(edge_var_id);
    }
    return result;
}


/**
 * ╔═╦══════════╦═════════╦═════════╗
 * ║ ║  EdgeId  ║ LabelId ║  Index  ║
 * ╠═╬══════════╬═════════╬═════════╣
 * ║1║    yes   ║   yes   ║    EL   ║ => any index works for this case
 * ║2║    yes   ║   no    ║    EL   ║
 * ║3║    no    ║   yes   ║    LN   ║
 * ║4║    no    ║   no    ║    EL   ║ => any index works for this case
 * ╚═╩══════════╩═════════╩═════════╝
 */
unique_ptr<BindingIdIter> EdgeLabelPlan::get_binding_id_iter() {
    vector<unique_ptr<ScanRange>> ranges;
    if (edge_assigned || !label_assigned) {
        // cases 1, 2 and 4 uses EL
        ranges.push_back(get_edge_range());
        ranges.push_back(get_label_range());

        return make_unique<IndexScan>(relational_model.get_edge2label(), move(ranges));
    } else {
        // case 3 uses EL
        ranges.push_back(get_label_range());
        ranges.push_back(get_edge_range());

        return make_unique<IndexScan>(relational_model.get_label2edge(), move(ranges));
    }
}

std::unique_ptr<ScanRange> EdgeLabelPlan::get_edge_range() {
    if (edge_assigned) {
        return make_unique<AssignedVar>(edge_var_id);
    } else if (graph_id.is_default()) {
        return make_unique<DefaultGraphVar>(edge_var_id);
    } else {
        return make_unique<NamedGraphVar>(edge_var_id, graph_id, ObjectType::edge);
    }
}


std::unique_ptr<ScanRange> EdgeLabelPlan::get_label_range() {
    if (!label_id.is_null()) {
        return make_unique<Term>(label_id);
    } else if (label_assigned) {
        return make_unique<AssignedVar>(label_var_id);
    } else {
        return make_unique<DefaultGraphVar>(label_var_id);
    }
}
