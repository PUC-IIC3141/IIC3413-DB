#include "base/ids/var_id.h"
#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/import/bulk_import.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/physical_plan/binding_id_iter/graph_scan.h"
#include "relational_model/physical_plan/binding_id_iter/index_nested_loop_join.h"

#include <chrono>
#include <climits>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	if (argc < 3) {
        cerr << "Error: expected node_file and edge_file from standard input.\n";
        return 1;
    }
	auto start = chrono::system_clock::now();

	RelationalModel::init();
	// TODO: support creating other graphs
	BulkImport import = BulkImport(argv[1], argv[2], RelationalModel::get_graph(GraphId(0)));
	import.start_import();

	auto end = chrono::system_clock::now();
	chrono::duration<float, milli> duration = end - start;
    cout << "Bulk Import duration: " << duration.count() << "ms\n";
	return 0;
}
