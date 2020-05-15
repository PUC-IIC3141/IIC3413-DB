#include <iostream>

#include "relational_model/physical_plan/binding_iter/match.h"
#include "relational_model/physical_plan/binding_iter/projection.h"

#include "relational_model/physical_plan/binding_id_iter/total_scan.h"

#include "relational_model/graph/relational_graph.h"
#include "relational_model/relational_model.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;

/*
    manually creates the plan for a query that select all triples (NodeId, KeyId, ValueId)
*/
unique_ptr<BindingIdIter> get_binding_id_iter() {
    auto graph_id = GraphId(0);
    auto& graph = RelationalModel::get_graph(graph_id);

    // Using B+Tree with tuples (NodeId, KeyId, ValueId) => node2prop
    std::vector<std::pair<VarId, int>> scan1_vars = {
        { VarId(0), 0 }, // VarId(0) => ?n
        { VarId(1), 1 }, // VarId(1) => n.key
        { VarId(2), 2 }  // VarId(2) => n.value
    };
    return make_unique<TotalScan>(*graph.node2prop, scan1_vars);
}


unique_ptr<BindingIter> get_binding_iter() {
    // Es necesario al definir var_pos que el minimo VarId es 0 y el máximo es (var_pos.size() - 1)
    std::map<std::string, VarId> var_pos = {
        { "?n", VarId(0) },
        { "n.key", VarId(1) },
        { "n.value", VarId(2) }
    };
    auto match = make_unique<Match>(get_binding_id_iter(), var_pos);

    std::set<std::string> projection_vars {
        "?n",
        "n.key",
        "n.value"
    };
    return make_unique<Projection>(move(match), projection_vars, 300); // LIMIT 300
}


// Antes de probar el test se espera que se cargó una base de datos en la carpeta correcte, ej:
// build/Release/bin/import_graph -d test_files/db -n test_files/graph_creation_example/nodes.txt -e test_files/graph_creation_example/edges.txt
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
