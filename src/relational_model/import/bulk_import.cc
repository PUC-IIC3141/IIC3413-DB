#include "bulk_import.h"

#include <chrono>
#include <iostream>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "base/graph/value/value_string.h"
#include "relational_model/import/bulk_import_grammar.h"
#include "relational_model/import/bulk_import_value_visitor.h"
#include "relational_model/relational_model.h"
#include "storage/index/ordered_file/bpt_merger.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"

using namespace std;

BulkImport::BulkImport(const string& nodes_file_name, const string& edges_file_name, RelationalGraph& graph)
    : graph(graph),
      node_labels(OrderedFile("node_labels.dat", 2)),
      edge_labels(OrderedFile("edge_labels.dat", 2)),
      node_key_value(OrderedFile("node_key_value.dat", 3)),
      edge_key_value(OrderedFile("edge_key_value.dat", 3)),
      connections(OrderedFile("connections.dat", 3)),
      self_connected_nodes(OrderedFile("self_connection.dat", 2))
{
    nodes_file = ifstream(nodes_file_name);
    edges_file = ifstream(edges_file_name);

    nodes_file.unsetf(std::ios::skipws);
    edges_file.unsetf(std::ios::skipws);

    if (nodes_file.fail()) {
        std::cerr << "Nodes file: \"" << nodes_file_name << "\" could not be opened. Exiting.\n";
        exit(-1);

    } else if (edges_file.fail()){
        std::cerr << "Edges file: \"" << edges_file_name << "\" could not be opened. Exiting.\n";
        exit(-1);
    }
}


void BulkImport::start_import() {
    auto start = chrono::system_clock::now();
    auto line_number = 1;
    cout << "procesing nodes:\n";

    boost::spirit::istream_iterator node_iter( nodes_file );
    boost::spirit::istream_iterator node_end;
    do {
        bulk_import_ast::Node node;
        bool r = phrase_parse(node_iter, node_end, bulk_import_parser::node, bulk_import_parser::skipper, node);
        if (r) {
            // cout << "\r  line " << line_number << std::flush;
            process_node(node);
            line_number++;
        }
        else {
            cout << "ERROR: line " << line_number << " has wrong format in nodes file.\n";
            return;
        }
    } while(node_iter != node_end);
    cout << "\n";

    line_number = 1;
    cout << "procesing edges:\n";

    boost::spirit::istream_iterator edge_iter( edges_file );
    boost::spirit::istream_iterator edge_end;
    do {
        bulk_import_ast::Edge edge;
        bool r = phrase_parse(edge_iter, edge_end, bulk_import_parser::edge, bulk_import_parser::skipper, edge);
        if (r) {
            // cout << "\r  line " << line_number << std::flush;
            process_edge(edge);
            line_number++;
        }
        else {
            cout << "ERROR: line " << line_number << " has wrong format in edges file.\n";
            return;
        }
    } while(edge_iter != edge_end);
    auto finish_reading_files = chrono::system_clock::now();

    chrono::duration<float, milli> duration1 = finish_reading_files - start;
    cout << "Reading files & writing ordered file: " << duration1.count() << "ms\n";

    // cout << "\nCreating indexes\n";

    // INDEXES WHERE APPENDING AT END IS POSSIBLE
    // NODE - LABEL
    node_labels.order(vector<uint_fast8_t> { 0, 1 });
    relational_model.get_node2label().bulk_import(node_labels);

    // EDGE - LABEL
    edge_labels.order(vector<uint_fast8_t> { 0, 1 });
    relational_model.get_edge2label().bulk_import(edge_labels);

    // NODE - KEY - VALUE
    node_key_value.order(vector<uint_fast8_t> { 0, 1, 2 });
    relational_model.get_node_key_value().bulk_import(node_key_value);

    // EDGE - KEY - VALUE
    edge_key_value.order(vector<uint_fast8_t> { 0, 1, 2 });
    relational_model.get_edge_key_value().bulk_import(edge_key_value);

    // CONNECTIONS
    connections.order(vector<uint_fast8_t> { 0, 1, 2 });
    relational_model.get_from_to_edge().bulk_import(connections);

    connections.order(vector<uint_fast8_t> { 2, 0, 1 });
    relational_model.get_to_edge_from().bulk_import(connections);

    connections.order(vector<uint_fast8_t> { 2, 0, 1 });
    relational_model.get_edge_from_to().bulk_import(connections);

    // SELF CONNECTIONS
    self_connected_nodes.order(vector<uint_fast8_t> { 0, 1 });
    relational_model.get_nodeloop_edge().bulk_import(self_connected_nodes);

    self_connected_nodes.order(vector<uint_fast8_t> { 1, 0 });
    relational_model.get_edge_nodeloop().bulk_import(self_connected_nodes);

    auto finish_creating_non_merging_index = chrono::system_clock::now();
    chrono::duration<float, milli> duration2 = finish_creating_non_merging_index - finish_reading_files;
    cout << "Writing non-merging indexes (7): " << duration2.count() << "ms\n";

    // INDEXES WHERE APPENDING AT END IS NOT POSSIBLE AND MERGE IS NEEDED
    // LABEL - NODE
    node_labels.order(vector<uint_fast8_t> { 1, 0 });
    // relational_model.get_label2node().bulk_import(node_labels);
    merge_tree_and_ordered_file(relational_model.label2node_name, relational_model.get_label2node(),
                                node_labels);

    // LABEL - EDGE
    edge_labels.order(vector<uint_fast8_t> { 1, 0 });
    // relational_model.get_label2edge().bulk_import(edge_labels);
    merge_tree_and_ordered_file(relational_model.label2edge_name, relational_model.get_label2edge(),
                                edge_labels);

    // KEY - VALUE - NODE
    node_key_value.order(vector<uint_fast8_t> { 2, 0, 1 });
    // relational_model.get_key_value_node().bulk_import(node_key_value);
    merge_tree_and_ordered_file(relational_model.key_value_node_name, relational_model.get_key_value_node(),
                                node_key_value);

    // KEY - VALUE - EDGE
    edge_key_value.order(vector<uint_fast8_t> { 2, 0, 1 });
    // relational_model.get_key_value_edge().bulk_import(edge_key_value);
    merge_tree_and_ordered_file(relational_model.key_value_edge_name, relational_model.get_key_value_edge(),
                                edge_key_value);

    // KEY - NODE - VALUE
    node_key_value.order(vector<uint_fast8_t> { 0, 2, 1 });
    // relational_model.get_key_node_value().bulk_import(node_key_value);
    merge_tree_and_ordered_file(relational_model.key_node_value_name, relational_model.get_key_node_value(),
                                node_key_value);

    // KEY - EDGE - VALUE
    edge_key_value.order(vector<uint_fast8_t> { 0, 2, 1 });
    // relational_model.get_key_edge_value().bulk_import(edge_key_value);
    merge_tree_and_ordered_file(relational_model.key_edge_value_name, relational_model.get_key_edge_value(),
                                edge_key_value);

    auto finish_creating_index = chrono::system_clock::now();
    chrono::duration<float, milli> duration3 = finish_creating_index - finish_creating_non_merging_index;
    cout << "Writing merging indexes (6): " << duration3.count() << "ms\n";
}


void BulkImport::merge_tree_and_ordered_file(const string& original_filename, BPlusTree& bpt,
                                             OrderedFile& ordered_file)
{
    auto tmp_filename = original_filename + ".tmp";
    auto bpt_params = make_unique<BPlusTreeParams>(tmp_filename, bpt.params->key_size, bpt.params->value_size);

    auto new_bpt = BPlusTree(move(bpt_params));
    auto bpt_merger = BptMerger(ordered_file, bpt);
    new_bpt.bulk_import(bpt_merger);

    buffer_manager.flush();

    file_manager.remove(bpt.params->dir_file_id);
    file_manager.remove(bpt.params->leaf_file_id);

    file_manager.rename(new_bpt.params->dir_file_id, bpt.params->dir_file_id);
    file_manager.rename(new_bpt.params->leaf_file_id, bpt.params->leaf_file_id);
}


void BulkImport::process_node(const bulk_import_ast::Node& node) {
    uint64_t node_id = graph.create_node();

    node_dict.insert(pair<uint64_t, uint64_t>(node.id, node_id));

    for (auto& label : node.labels) {
        node_labels.append_record(graph.get_record_for_node_label(node_id, label));
    }

    for (auto& property : node.properties) {
        BulkImportValueVisitor visitor;
        auto value = visitor(property.value);
        node_key_value.append_record(graph.get_record_for_node_property(node_id, property.key, *value));
    }
}


void BulkImport::process_edge(const bulk_import_ast::Edge& edge) {
    uint64_t edge_id = graph.create_edge();

    auto left_id = node_dict.find(edge.left_id);
    auto right_id = node_dict.find(edge.right_id);

    if (left_id == node_dict.end() || right_id == node_dict.end()) {
        throw logic_error("Edge using undeclared node.");
    }

    if (left_id->second == right_id->second) {
        self_connected_nodes.append_record(Record(left_id->second, edge_id));
    }

    if (edge.direction == bulk_import_ast::EdgeDirection::right) {
        connections.append_record(Record(left_id->second, right_id->second, edge_id));
    } else {
        connections.append_record(Record(right_id->second, left_id->second, edge_id));
    }

    for (auto& label : edge.labels) {
        edge_labels.append_record(graph.get_record_for_edge_label(edge_id, label));
    }

    for (auto& property : edge.properties) {
        BulkImportValueVisitor visitor;
        auto value = visitor(property.value);
        edge_key_value.append_record(graph.get_record_for_edge_property(edge_id, property.key, *value));
    }
}
