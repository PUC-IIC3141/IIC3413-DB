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
    return RelationalModel::get_catalog().create_node() | NODE_MASK;
}


uint64_t RelationalGraph::create_edge() {
    return RelationalModel::get_catalog().create_edge() | EDGE_MASK;
}

/******************************************* Methods for bulk import ***************************************/
Record RelationalGraph::get_record_for_node_label(uint64_t node_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);
    RelationalModel::get_catalog().add_node_label(label_id);
    return Record(node_id, label_id);
}


Record RelationalGraph::get_record_for_edge_label(uint64_t edge_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);
    RelationalModel::get_catalog().add_edge_label(label_id);
    return Record(edge_id, label_id);
}


Record RelationalGraph::get_record_for_node_property(uint64_t node_id, const string& key, const Value& value) {
    uint64_t key_id = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    RelationalModel::get_catalog().add_node_key(key_id);
    return Record(node_id, key_id, value_id);
}


Record RelationalGraph::get_record_for_edge_property(uint64_t edge_id, const string& key, const Value& value) {
    uint64_t key_id = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    RelationalModel::get_catalog().add_edge_key(key_id);
    return Record(edge_id, key_id, value_id);
}

/***************************************** End methods for bulk import *************************************/

/************************************* Methods to add elements one by one **********************************/
void RelationalGraph::connect_nodes(uint64_t id_from, uint64_t id_to, uint64_t id_edge) {
    from_to_edge->insert( Record(id_from, id_to, id_edge) );
    to_edge_from->insert( Record(id_to, id_edge, id_from) );
    edge_from_to->insert( Record(id_edge, id_from, id_to) );
}


void RelationalGraph::add_label_to_node(uint64_t node_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);

    label2node->insert( Record(label_id, node_id) );
    node2label->insert( Record(node_id, label_id) );

    RelationalModel::get_catalog().add_node_label(label_id);
}


void RelationalGraph::add_label_to_edge(uint64_t edge_id, const string& label) {
    uint64_t label_id = RelationalModel::get_or_create_string_unmasked_id(label);

    label2edge->insert( Record(label_id, edge_id) );
    edge2label->insert( Record(edge_id, label_id) );

    RelationalModel::get_catalog().add_edge_label(label_id);
}


void RelationalGraph::add_property_to_node(uint64_t node_id, const string& key, const Value& value) {
    uint64_t key_id   = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    node2prop->insert( Record(node_id, key_id, value_id) );
    prop2node->insert( Record(key_id, value_id, node_id) );

    RelationalModel::get_catalog().add_node_key(key_id);
}


void RelationalGraph::add_property_to_edge(uint64_t edge_id, const string& key, const Value& value) {
    uint64_t key_id   = RelationalModel::get_or_create_string_unmasked_id(key);
    uint64_t value_id = RelationalModel::get_or_create_value_masked_id(value);

    edge2prop->insert( Record(edge_id, key_id, value_id) );
    prop2edge->insert( Record(key_id, value_id, edge_id) );

    RelationalModel::get_catalog().add_edge_key(key_id);
}
/********************************** End methods to add elements one by one *********************************/
