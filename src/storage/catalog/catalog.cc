#include "catalog.h"

#include <iostream>

#include "base/parser/logical_plan/exceptions.h"
#include "storage/file_manager.h"

using namespace std;

Catalog::Catalog(const string& filename)
    : file(file_manager.get_file(file_manager.get_file_id(filename)))
{
    file.seekg(0, file.end);
    if (file.tellg() == 0) {
        graph_count = 0;
    }
    else {
        file.seekg(0, file.beg);

        graph_count = read_uint32();
        if (graph_count <= 0) {
            // throw std::logic_error("Catalog file inconsistent: graph_count must be more than 0.");
            return;
        }
        // graph_names      = std::vector<std::string>(graph_count);
        node_count       = std::vector<uint64_t>(graph_count);
        edge_count       = std::vector<uint64_t>(graph_count);
        node_label_count = std::vector<uint64_t>(graph_count);
        edge_label_count = std::vector<uint64_t>(graph_count);
        node_key_count   = std::vector<uint64_t>(graph_count);
        edge_key_count   = std::vector<uint64_t>(graph_count);

        node_label_stats = std::vector<std::map<uint64_t, uint64_t>>(graph_count);
        edge_label_stats = std::vector<std::map<uint64_t, uint64_t>>(graph_count);
        node_key_stats   = std::vector<std::map<uint64_t, uint64_t>>(graph_count);
        edge_key_stats   = std::vector<std::map<uint64_t, uint64_t>>(graph_count);


        for (uint32_t graph = 0; graph < graph_count; graph++) {
            auto graph_name_length  = read_uint32();
            string graph_name = string(graph_name_length, ' ');
            file.read((char*)graph_name.data(), graph_name_length);

            graph_names.push_back(graph_name);
            graph_ids.insert({graph_name, GraphId(graph)});

            node_count[graph] = read_uint64();
            edge_count[graph] = read_uint64();
            node_label_count[graph] = read_uint64();
            edge_label_count[graph] = read_uint64();
            node_key_count[graph]   = read_uint64();
            edge_key_count[graph]   = read_uint64();

            for (uint64_t i = 0; i < node_label_count[graph]; i++) {
                auto label_id = read_uint64();
                auto label_count = read_uint64();
                node_label_stats[graph].insert({label_id, label_count});
            }
            for (uint64_t i = 0; i < edge_label_count[graph]; i++) {
                auto label_id = read_uint64();
                auto label_count = read_uint64();
                edge_label_stats[graph].insert({label_id, label_count});
            }
            for (uint64_t i = 0; i < node_key_count[graph]; i++) {
                auto key_id = read_uint64();
                auto key_count = read_uint64();
                node_key_stats[graph].insert({key_id, key_count});
            }
            for (uint64_t i = 0; i < edge_key_count[graph]; i++) {
                auto key_id = read_uint64();
                auto key_count = read_uint64();
                edge_key_stats[graph].insert({key_id, key_count});
            }
        }
    }
}


Catalog::~Catalog() {
    save_changes();
}


GraphId Catalog::get_graph(const std::string& graph_name) {
    // Empty string => default graph, even if default graph has a explicit name
    if (graph_name.empty()) {
        if (graph_count == 0) {
            throw GraphDoesNotExist("default");
        }
        return GraphId(0);
    }
    auto search = graph_ids.find(graph_name);
    if (search == graph_ids.end()) {
        throw GraphDoesNotExist(graph_name);
    }
    return search->second;
}


GraphId Catalog::create_graph(const std::string& graph_name) {
    if (graph_name.empty()) {
        if (graph_count != 0) {
            throw std::invalid_argument("Default graph already exixsts. Try creating a graph with a name.");
        }
    }
    else {
        auto search = graph_ids.find(graph_name);
        if (search != graph_ids.end()) {
            throw std::invalid_argument("\"" + graph_name + "\" graph name already exixsts.");
        }
    }
    auto graph_id = GraphId(graph_count);
    ++graph_count;
    graph_ids.insert({graph_name, graph_id});
    graph_names.push_back(graph_name);

    node_count.push_back(0);
    edge_count.push_back(0);
    node_label_count.push_back(0);
    edge_label_count.push_back(0);
    node_key_count.push_back(0);
    edge_key_count.push_back(0);

    node_label_stats.push_back(std::map<uint64_t, uint64_t>());
    edge_label_stats.push_back(std::map<uint64_t, uint64_t>());
    node_key_stats.push_back(std::map<uint64_t, uint64_t>());
    edge_key_stats.push_back(std::map<uint64_t, uint64_t>());

    return graph_id;
}


void Catalog::save_changes(){
    file.seekg(0, file.beg);
    // cout << "Saving catalog:" << endl;

    file.write((const char *)&graph_count, sizeof(graph_count));
    for (uint32_t graph = 0; graph < graph_count; graph++) {
        // cout << "Graph " << graph << ": \"" << graph_names[graph] << "\"" << endl;
        // cout << "  node count: " << node_count[graph] << endl;
        // cout << "  edge count: " << edge_count[graph] << endl;
        // cout << "  node disinct labels: " << node_label_count[graph] << endl;
        // cout << "  edge disinct labels: " << edge_label_count[graph] << endl;
        // cout << "  node disinct keys:   " << node_key_count[graph] << endl;
        // cout << "  edge disinct keys:   " << edge_key_count[graph] << endl;

        uint32_t name_len = graph_names[graph].size();
        file.write((const char *)&name_len, sizeof(name_len));
        file.write((const char *)graph_names[graph].data(), name_len);

        file.write((const char *)&node_count[graph],       sizeof(node_count[graph]));
        file.write((const char *)&edge_count[graph],       sizeof(edge_count[graph]));
        file.write((const char *)&node_label_count[graph], sizeof(node_label_count[graph]));
        file.write((const char *)&edge_label_count[graph], sizeof(edge_label_count[graph]));
        file.write((const char *)&node_key_count[graph],   sizeof(node_key_count[graph]));
        file.write((const char *)&edge_key_count[graph],   sizeof(edge_key_count[graph]));

        for (auto&& [id, count] : node_label_stats[graph]) {
            file.write((const char *)&id, sizeof(id));
            file.write((const char *)&count, sizeof(count));
        }
        for (auto&& [id, count] : edge_label_stats[graph]) {
            file.write((const char *)&id, sizeof(id));
            file.write((const char *)&count, sizeof(count));
        }
        for (auto&& [id, count] : node_key_stats[graph]) {
            file.write((const char *)&id, sizeof(id));
            file.write((const char *)&count, sizeof(count));
        }
        for (auto&& [id, count] : edge_key_stats[graph]) {
            file.write((const char *)&id, sizeof(id));
            file.write((const char *)&count, sizeof(count));
        }
    }
}


void Catalog::print() {
    for (uint32_t graph = 0; graph < graph_count; graph++) {
        cout << "Graph " << graph << ": \"" << graph_names[graph] << "\"" << endl;
        cout << "  node count: " << node_count[graph] << endl;
        cout << "  edge count: " << edge_count[graph] << endl;
        cout << "  node disinct labels: " << node_label_count[graph] << endl;
        cout << "  edge disinct labels: " << edge_label_count[graph] << endl;
        cout << "  node disinct keys:   " << node_key_count[graph] << endl;
        cout << "  edge disinct keys:   " << edge_key_count[graph] << endl;
    }
}


uint64_t Catalog::read_uint64() {
    uint64_t res;
    file.read((char*)&res, 8);
    return res;
}


uint32_t Catalog::read_uint32() {
    uint32_t res;
    file.read((char*)&res, 4);
    return res;
}


uint64_t Catalog::get_node_count(GraphId graph_id) {
    return node_count[graph_id];
}


uint64_t Catalog::get_edge_count(GraphId graph_id) {
    return edge_count[graph_id];
}


uint64_t Catalog::create_node(GraphId graph_id) {
    return ++node_count[graph_id];
}


uint64_t Catalog::create_edge(GraphId graph_id) {
    return ++edge_count[graph_id];
}


void Catalog::add_node_label(GraphId graph_id, uint64_t label_id) {
    add_to_map(node_label_stats[graph_id], label_id, node_label_count[graph_id]);
}


void Catalog::add_edge_label(GraphId graph_id, uint64_t label_id) {
    add_to_map(edge_label_stats[graph_id], label_id, edge_label_count[graph_id]);
}


void Catalog::add_node_key(GraphId graph_id, uint64_t key_id) {
    add_to_map(node_key_stats[graph_id], key_id, node_key_count[graph_id]);
}


void Catalog::add_edge_key(GraphId graph_id, uint64_t key_id) {
    add_to_map(edge_key_stats[graph_id], key_id, edge_key_count[graph_id]);
}


void Catalog::add_to_map(map<uint64_t, uint64_t>& map, uint64_t key, uint64_t& count) {
    auto it = map.find(key);
    if ( it == map.end() ) {
        count++;
        map.insert(pair<uint64_t, uint64_t>(key, 1));
    } else {
        it->second += 1;
    }
}


uint64_t Catalog::get_count(map<uint64_t, uint64_t>& map, uint64_t key) {
    auto it = map.find(key);
    if ( it == map.end() ) {
        return 0;
    } else {
        return it->second;
    }
}


uint64_t Catalog::get_node_count_for_label(GraphId graph_id, uint64_t label_id) {
    return get_count(node_label_stats[graph_id], label_id);
}


uint64_t Catalog::get_edge_count_for_label(GraphId graph_id, uint64_t label_id) {
    return get_count(edge_label_stats[graph_id], label_id);
}


uint64_t Catalog::get_node_count_for_key(GraphId graph_id, uint64_t key_id) {
    return get_count(node_key_stats[graph_id], key_id);
}


uint64_t Catalog::get_edge_count_for_key(GraphId graph_id, uint64_t key_id){
    return get_count(edge_key_stats[graph_id], key_id);
}
