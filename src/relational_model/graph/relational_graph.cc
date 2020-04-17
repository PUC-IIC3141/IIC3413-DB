#include "relational_model/graph/relational_graph.h"

#include "base/graph/edge.h"
#include "base/graph/node.h"
#include "base/graph/value/value_string.h"
#include "storage/buffer_manager.h"
#include "storage/index/record.h"
#include "storage/index/bplus_tree/bplus_tree.h"
#include "relational_model/relational_model.h"

#include <openssl/md5.h>

using namespace std;

RelationalGraph::RelationalGraph(GraphId graph_id)
    : graph_id(graph_id)
{
    auto bpt_params_label2node = make_unique<BPlusTreeParams>(RelationalModel::label2node_name, 2);
    auto bpt_params_label2edge = make_unique<BPlusTreeParams>(RelationalModel::label2edge_name, 2);
    auto bpt_params_node2label = make_unique<BPlusTreeParams>(RelationalModel::node2label_name, 2);
    auto bpt_params_edge2label = make_unique<BPlusTreeParams>(RelationalModel::edge2label_name, 2);

    auto bpt_params_prop2node = make_unique<BPlusTreeParams>(RelationalModel::prop2node_name, 3);
    auto bpt_params_prop2edge = make_unique<BPlusTreeParams>(RelationalModel::prop2edge_name, 3);
    auto bpt_params_node2prop = make_unique<BPlusTreeParams>(RelationalModel::node2prop_name, 3);
    auto bpt_params_edge2prop = make_unique<BPlusTreeParams>(RelationalModel::edge2prop_name, 3);

    auto bpt_params_from_to_edge = make_unique<BPlusTreeParams>(RelationalModel::from_to_edge_name, 3);
    auto bpt_params_to_edge_from = make_unique<BPlusTreeParams>(RelationalModel::to_edge_from_name, 3);
    auto bpt_params_edge_from_to = make_unique<BPlusTreeParams>(RelationalModel::edge_from_to_name, 3);

    label2node = make_unique<BPlusTree>(move(bpt_params_label2node));
    label2edge = make_unique<BPlusTree>(move(bpt_params_label2edge));
    node2label = make_unique<BPlusTree>(move(bpt_params_node2label));
    edge2label = make_unique<BPlusTree>(move(bpt_params_edge2label));

    prop2node = make_unique<BPlusTree>(move(bpt_params_prop2node));
    prop2edge = make_unique<BPlusTree>(move(bpt_params_prop2edge));
    node2prop = make_unique<BPlusTree>(move(bpt_params_node2prop));
    edge2prop = make_unique<BPlusTree>(move(bpt_params_edge2prop));

    from_to_edge = make_unique<BPlusTree>(move(bpt_params_from_to_edge));
    to_edge_from = make_unique<BPlusTree>(move(bpt_params_to_edge_from));
    edge_from_to = make_unique<BPlusTree>(move(bpt_params_edge_from_to));
}


RelationalGraph::~RelationalGraph() = default;


uint64_t RelationalGraph::create_node() {
    return RelationalModel::get_catalog().create_node(graph_id)
           | RelationalModel::NODE_MASK
           | (graph_id << RelationalModel::GRAPH_OFFSET);
}


uint64_t RelationalGraph::create_edge() {
    return RelationalModel::get_catalog().create_edge(graph_id)
           | RelationalModel::EDGE_MASK
           | (graph_id << RelationalModel::GRAPH_OFFSET);
}


Record RelationalGraph::get_record_for_node_label(uint64_t node_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);
    RelationalModel::get_catalog().add_node_label(graph_id, label_id);
    return Record(
        node_id,
        label_id | (graph_id << RelationalModel::GRAPH_OFFSET)
    );
}


Record RelationalGraph::get_record_for_edge_label(uint64_t edge_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);
    RelationalModel::get_catalog().add_edge_label(graph_id, label_id);
    return Record(
        edge_id,
        label_id | (graph_id << RelationalModel::GRAPH_OFFSET)
    );
}


Record RelationalGraph::get_record_for_node_property(uint64_t node_id, const string& key, const Value& value) {
    uint64_t key_id = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    RelationalModel::get_catalog().add_node_key(graph_id, key_id);
    return Record(
        node_id,
        key_id | (graph_id << RelationalModel::GRAPH_OFFSET),
        value_id | (graph_id << RelationalModel::GRAPH_OFFSET)
    );
}


Record RelationalGraph::get_record_for_edge_property(uint64_t edge_id, const string& key, const Value& value) {
    uint64_t key_id = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    RelationalModel::get_catalog().add_edge_key(graph_id, key_id);
    return Record(
        edge_id,
        key_id | (graph_id << RelationalModel::GRAPH_OFFSET),
        value_id | (graph_id << RelationalModel::GRAPH_OFFSET)
    );
}
