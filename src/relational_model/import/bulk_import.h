#ifndef RELATIONAL_MODEL__BULK_IMPORT_H_
#define RELATIONAL_MODEL__BULK_IMPORT_H_

#include "storage/index/ordered_file/ordered_file.h"
#include "relational_model/import/bulk_import_ast.h"

#include <string>
#include <fstream>
#include <list>
#include <map>

class RelationalGraph;

class BulkImport {
public:
    BulkImport(const std::string& nodes_file, const std::string& edges_file, RelationalGraph& graph);
    ~BulkImport() = default;

    void start_import();

private:
    std::ifstream nodes_file;
    std::ifstream edges_file;
    RelationalGraph& graph;

    void process_node(const bulk_import_ast::Node& node);
    void process_edge(const bulk_import_ast::Edge& edge);

    std::map<uint64_t, uint64_t> node_dict;
    // std::list<std::tuple<uint64_t, uint64_t, uint64_t>> edges_original_ids;

    OrderedFile node_labels;
    OrderedFile edge_labels;
    OrderedFile node_key_value;
    OrderedFile edge_key_value;
    OrderedFile connections;
};

#endif //RELATIONAL_MODEL__BULK_IMPORT_H_