#include "relational_model/graph/relational_graph.h"

#include "base/graph/edge.h"
#include "base/graph/node.h"
#include "base/graph/value/value_string.h"
#include "relational_model/relational_model.h"
#include "storage/buffer_manager.h"
#include "storage/catalog/catalog.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"

#include <openssl/md5.h>

using namespace std;

RelationalGraph::RelationalGraph(GraphId graph_id)
    : graph_id(graph_id) { }


RelationalGraph::~RelationalGraph() = default;


uint64_t RelationalGraph::create_node() {
    return catalog.create_node(graph_id)
           | RelationalModel::NODE_MASK
           | (graph_id << RelationalModel::GRAPH_OFFSET);
}


uint64_t RelationalGraph::create_edge() {
    return catalog.create_edge(graph_id)
           | RelationalModel::EDGE_MASK
           | (graph_id << RelationalModel::GRAPH_OFFSET);
}


Record RelationalGraph::get_record_for_node_label(uint64_t node_id, const string& label) {
    uint64_t label_id = relational_model.get_string_id(label, true);
    catalog.add_node_label(graph_id, label_id);
    return Record(
        node_id,
        label_id
    );
}


Record RelationalGraph::get_record_for_edge_label(uint64_t edge_id, const string& label) {
    uint64_t label_id = relational_model.get_string_id(label, true);
    catalog.add_edge_label(graph_id, label_id);
    return Record(
        edge_id,
        label_id
    );
}


Record RelationalGraph::get_record_for_node_property(uint64_t node_id, const string& key, const Value& value) {
    uint64_t key_id = relational_model.get_string_id(key, true);
    uint64_t value_id = relational_model.get_value_id(value, true);

    catalog.add_node_key(graph_id, key_id);
    return Record(
        node_id,
        key_id,
        value_id
    );
}


Record RelationalGraph::get_record_for_edge_property(uint64_t edge_id, const string& key, const Value& value) {
    uint64_t key_id = relational_model.get_string_id(key, true);
    uint64_t value_id = relational_model.get_value_id(value, true);

    catalog.add_edge_key(graph_id, key_id);
    return Record(
        edge_id,
        key_id,
        value_id
    );
}
