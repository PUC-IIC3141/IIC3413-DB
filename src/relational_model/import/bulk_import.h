#ifndef RELATIONAL_MODEL__BULK_IMPORT_H_
#define RELATIONAL_MODEL__BULK_IMPORT_H_

#include "storage/index/ordered_file/ordered_file.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/import/bulk_import_ast.h"

#include <string>
#include <fstream>
#include <list>
#include <map>

class BulkImport {
public:
    BulkImport(const std::string& nodes_file, const std::string& edges_file, RelationalGraph& graph);
    ~BulkImport() = default;

    void start_import();

private:
    RelationalGraph& graph;

    std::ifstream nodes_file;
    std::ifstream edges_file;

    OrderedFile node_labels;
    OrderedFile edge_labels;
    OrderedFile node_key_value;
    OrderedFile edge_key_value;
    OrderedFile connections;
    OrderedFile self_connected_nodes;

    std::map<uint64_t, uint64_t> node_dict;

    void process_node(const bulk_import_ast::Node& node);
    void process_edge(const bulk_import_ast::Edge& edge);

    void merge_tree_and_ordered_file(const std::string& original_filename, BPlusTree&, OrderedFile&);
};

#endif // RELATIONAL_MODEL__BULK_IMPORT_H_