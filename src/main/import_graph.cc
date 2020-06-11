#include "base/ids/var_id.h"

#include <chrono>
#include <climits>
#include <iostream>

#include <boost/program_options.hpp>

#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/import/bulk_import.h"
#include "relational_model/binding/binding_id.h"
#include "relational_model/execution/binding_id_iter/index_scan.h"
#include "relational_model/execution/binding_id_iter/index_nested_loop_join.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char **argv) {
    string nodes_file;
    string edges_file;
    string graph_name;
    string db_folder;
    int buffer_size;

	try {
        // Parse arguments
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "show this help message")
            ("db-folder,d", po::value<string>(&db_folder)->required(), "set database folder path")
            ("buffer-size,b", po::value<int>(&buffer_size)->default_value(BufferManager::DEFAULT_BUFFER_POOL_SIZE),
                "set buffer pool size")
            ("nodes-file,n", po::value<string>(&nodes_file)->required(), "nodes file")
            ("edges-file,e", po::value<string>(&edges_file)->required(), "edges file")
            ("graph-name,g", po::value<string>(&graph_name)->required(), "graph name")
        ;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);

        if (vm.count("help")) {
            cout << "Usage: import_graph [OPTIONS]\n";
            cout << "Example: import_graph -n ./path/to/nodes.txt -e ./path/to/edges.txt -d ./path/to/db -g \"My Graph Name\"\n";
            cout << desc << "\n";
            return 0;
        }
        po::notify(vm);

        RelationalModel::init(db_folder, buffer_size);
        auto start = chrono::system_clock::now();

        auto& graph = relational_model.create_graph(graph_name);
        BulkImport import = BulkImport(nodes_file, edges_file, graph);
        import.start_import();

        auto end = chrono::system_clock::now();
        chrono::duration<float, milli> duration = end - start;
        cout << "Bulk Import duration: " << duration.count() << "ms\n";
    }
    catch (exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        RelationalModel::terminate();
        return 1;
    }
    catch (...) {
        cerr << "Exception of unknown type!\n";
        RelationalModel::terminate();
        return 1;
    }
    RelationalModel::terminate();
	return 0;
}
