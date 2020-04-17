/**
 * Ids are represented as 64 bytes divided as follows:
 * 8 bytes for type | 16 bytes for graph_id | 40 bytes for id
 *
 **/

#ifndef RELATIONAL_MODEL__GRAPH_H_
#define RELATIONAL_MODEL__GRAPH_H_

#include "base/ids/graph_id.h"
#include "base/graph/node.h"
#include "base/graph/edge.h"
#include "storage/index/object_file/object_file.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "storage/index/bplus_tree/bplus_tree_params.h"
#include "base/ids/object_id.h"

#include <memory>
#include <string>

class Value;

class RelationalGraph {
public:
    const GraphId graph_id;

    std::unique_ptr<BPlusTree> label2node;
    std::unique_ptr<BPlusTree> label2edge;
    std::unique_ptr<BPlusTree> node2label;
    std::unique_ptr<BPlusTree> edge2label;

    std::unique_ptr<BPlusTree> prop2node;
    std::unique_ptr<BPlusTree> prop2edge;
    std::unique_ptr<BPlusTree> node2prop;
    std::unique_ptr<BPlusTree> edge2prop;

    std::unique_ptr<BPlusTree> from_to_edge;
    std::unique_ptr<BPlusTree> to_edge_from;
    std::unique_ptr<BPlusTree> edge_from_to;

    RelationalGraph(GraphId graph_id);
    ~RelationalGraph();

    uint64_t create_node();
    uint64_t create_edge();

    // node/edge ids received already have the graph and type masked
    // Methods used by bulk import:
    Record get_record_for_connection(uint64_t node_from_id, uint64_t node_to_id, uint64_t edge_id);

    Record get_record_for_node_label(uint64_t node_id, const std::string& label);
    Record get_record_for_edge_label(uint64_t edge_id, const std::string& label);

    Record get_record_for_node_property(uint64_t node_id, const std::string& key, const Value& value);
    Record get_record_for_edge_property(uint64_t edge_id, const std::string& key, const Value& value);
};

#endif //RELATIONAL_MODEL__GRAPH_H_
