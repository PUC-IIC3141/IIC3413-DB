#include <fstream>
#include <iostream>

#include "base/binding/binding.h"
#include "base/binding/binding_iter.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "relational_model/relational_model.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/query_optimizer/physical_plan_generator.h"

#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;

int main() {
    file_manager.init();
    buffer_manager.init();
    RelationalModel::init();
    auto& graph = RelationalModel::get_graph(GraphId(0));

    std::cout << "Checking hash2id\n";
    if (!RelationalModel::get_hash2id_bpt().check())
        std::cout << "wrong BPlusTree: hash2id\n";

    std::cout << "Checking label2node\n";
    if (!graph.label2node->check())
        std::cout << "wrong BPlusTree: label2node\n";
    std::cout << "Checking node2label\n";
    if (!graph.node2label->check())
        std::cout << "wrong BPlusTree: node2label\n";

    std::cout << "Checking label2edge\n";
    if (!graph.label2edge->check())
        std::cout << "wrong BPlusTree: label2edge\n";
    std::cout << "Checking edge2label\n";
    if (!graph.edge2label->check())
        std::cout << "wrong BPlusTree: edge2label\n";

    std::cout << "Checking prop2node\n";
    if (!graph.prop2node->check())
        std::cout << "wrong BPlusTree: prop2node\n";
    std::cout << "Checking prop2edge\n";
    if (!graph.prop2edge->check())
        std::cout << "wrong BPlusTree: prop2edge\n";
    std::cout << "Checking node2prop\n";
    if (!graph.node2prop->check())
        std::cout << "wrong BPlusTree: node2prop\n";
    std::cout << "Checking edge2prop\n";
    if (!graph.edge2prop->check())
        std::cout << "wrong BPlusTree: edge2prop\n";

    std::cout << "Checking from_to_edge\n";
    if (!graph.from_to_edge->check())
        std::cout << "wrong BPlusTree: from_to_edge\n";
    std::cout << "Checking to_edge_from\n";
    if (!graph.to_edge_from->check())
        std::cout << "wrong BPlusTree: to_edge_from\n";
    std::cout << "Checking edge_from_to\n";
    if (!graph.edge_from_to->check())
        std::cout << "wrong BPlusTree: edge_from_to\n";
}
