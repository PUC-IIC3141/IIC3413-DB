#include <iostream>

#include "relational_model/physical_plan/binding_iter/match.h"
#include "relational_model/physical_plan/binding_iter/projection.h"

#include "relational_model/physical_plan/binding_id_iter/graph_scan.h"
#include "relational_model/physical_plan/binding_id_iter/index_nested_loop_join.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;

/*
    manually creates the plan for the query:
    SELECT ?n.name
    MATCH (?n :Person)
*/
unique_ptr<BindingIdIter> get_binding_id_iter() {
    auto graph_id = GraphId(0);
    auto& graph = RelationalModel::get_graph(graph_id);

    // Define Scan 1
    // Using B+Tree with tuples (LabelId, NodeId) to select all nodes with the label Person
    auto label_id = RelationalModel::get_string_unmasked_id("Person");
    std::vector<std::pair<ObjectId, int>> scan1_terms = {
        { label_id, 0 }
    };
    std::vector<std::pair<VarId, int>> scan1_vars = {
        { VarId(0), 1 } // VarId(0) => ?n
    };
    auto scan1 = make_unique<GraphScan>(graph_id, *graph.label2node, scan1_terms, scan1_vars);

    // Define Scan 2
    // Using B+Tree with tuples (NodeId, KeyId, ValueId) to select all values from a certain nodeId and KeyId = "name"
    auto key_id = RelationalModel::get_string_unmasked_id("name");
    std::vector<std::pair<ObjectId, int>> scan2_terms = {
        { key_id, 1 }
    };
    std::vector<std::pair<VarId, int>> scan2_vars = {
        { VarId(0), 0 }, // VarId(0) => ?n
        { VarId(1), 2 }  // VarId(1) => ?n.name
    };
    auto scan2 = make_unique<GraphScan>(graph_id, *graph.node2prop, scan2_terms, scan2_vars);

    // // return Index Nested Loop Join between scan1 and scan2
    return make_unique<IndexNestedLoopJoin>(move(scan1), move(scan2));
}


unique_ptr<BindingIter> get_binding_iter() {
    // Es necesario al definir var_pos que el minimo VarId es 0 y el máximo es (var_pos.size() - 1)
    std::map<std::string, VarId> var_pos = {
        { "?n.name", VarId(1) },
        { "?n", VarId(0) }
    };
    auto match = make_unique<Match>(get_binding_id_iter(), var_pos);

    std::set<std::string> projection_vars {
        "?n",
        "?n.name"
    };
    return make_unique<Projection>(move(match), projection_vars, 0); // 0 means no LIMIT
}


int main() {
    file_manager.init("test_files/db");
    buffer_manager.init();
    RelationalModel::init();
    auto root = get_binding_iter();

    try {
        root->begin();
        auto binding = root->next();
        auto count = 0;
        while (binding != nullptr) {
            ++count;
            cout << binding->to_string() << '\n';
            binding = root->next();
        }
        cout << "Found " << count << " results.\n";
    }
    catch (std::exception& e) {
        cout << "Exception in query excecution: " << e.what() << endl;
    }
}
