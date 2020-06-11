#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "base/binding/binding.h"
#include "base/binding/binding_iter.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/query_optimizer/query_optimizer.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char **argv) {
    string db_folder;
    int buffer_size;

    // Parse arguments
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show this help message")
        ("buffer-size,b", po::value<int>(&buffer_size)->default_value(BufferManager::DEFAULT_BUFFER_POOL_SIZE),
                "set buffer pool size")
        ("db-folder,d", po::value<string>(&db_folder)->required(), "set database folder path")
    ;

    po::positional_options_description p;
    p.add("db-folder", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("help")) {
        cout << "Usage: server [options] DB_FOLDER\n";
        cout << desc << "\n";
        return 0;
    }
    po::notify(vm);

    RelationalModel::init(db_folder, buffer_size);

    // NODE LABELS
    std::cout << "Checking label2node\n";
    if (!relational_model.get_label2node().check())
        std::cout << "wrong BPlusTree: label2node\n";
    std::cout << "Checking node2label\n";
    if (!relational_model.get_node2label().check())
        std::cout << "wrong BPlusTree: node2label\n";

    // EDGE LABELS
    std::cout << "Checking label2edge\n";
    if (!relational_model.get_label2edge().check())
        std::cout << "wrong BPlusTree: label2edge\n";
    std::cout << "Checking edge2label\n";
    if (!relational_model.get_edge2label().check())
        std::cout << "wrong BPlusTree: edge2label\n";

    // NODE PROPERTIES
    std::cout << "Checking key_value_node\n";
    if (!relational_model.get_key_value_node().check())
        std::cout << "wrong BPlusTree: key_value_node\n";
    std::cout << "Checking key_node_value\n";
    if (!relational_model.get_key_node_value().check())
        std::cout << "wrong BPlusTree: key_node_value\n";
    std::cout << "Checking node_key_value\n";
    if (!relational_model.get_node_key_value().check())
        std::cout << "wrong BPlusTree: node_key_value\n";

    // EDGE PROPERTIES
    std::cout << "Checking key_value_edge\n";
    if (!relational_model.get_key_value_edge().check())
        std::cout << "wrong BPlusTree: key_value_edge\n";
    std::cout << "Checking key_edge_value\n";
    if (!relational_model.get_key_edge_value().check())
        std::cout << "wrong BPlusTree: key_edge_value\n";
    std::cout << "Checking edge_key_value\n";
    if (!relational_model.get_edge_key_value().check())
        std::cout << "wrong BPlusTree: edge_key_value\n";

    // CONNECTIONS
    std::cout << "Checking from_to_edge\n";
    if (!relational_model.get_from_to_edge().check())
        std::cout << "wrong BPlusTree: from_to_edge\n";
    std::cout << "Checking to_edge_from\n";
    if (!relational_model.get_to_edge_from().check())
        std::cout << "wrong BPlusTree: to_edge_from\n";
    std::cout << "Checking edge_from_to\n";
    if (!relational_model.get_edge_from_to().check())
        std::cout << "wrong BPlusTree: edge_from_to\n";

    // SELF CONNECTIONS
    std::cout << "Checking nodeloop_edge\n";
    if (!relational_model.get_nodeloop_edge().check())
        std::cout << "wrong BPlusTree: nodeloop_edge\n";
    std::cout << "Checking edge_nodeloop\n";
    if (!relational_model.get_edge_nodeloop().check())
        std::cout << "wrong BPlusTree: edge_nodeloop\n";

    RelationalModel::terminate();
}
